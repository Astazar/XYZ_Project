#include "Turret.h"
#include <Components/Weapon/WeaponBarellComponent.h>
#include <AIController.h>
#include <GenericTeamAgentInterface.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystem.h>
#include <Net/UnrealNetwork.h>


ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* TurretRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TurretRoot"));
	SetRootComponent(TurretRoot);

	TurrentBaseComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurrentBase"));
	TurrentBaseComponent->SetupAttachment(TurretRoot);

	TurretBarellComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBarell"));
	TurretBarellComponent->SetupAttachment(TurrentBaseComponent);
	
	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarell->SetupAttachment(TurretBarellComponent);

	SetReplicates(true);
}

void ATurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATurret, CurrentTarget);
}

void ATurret::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	OnTakeAnyDamage.AddDynamic(this, &ATurret::OnTakeDamage);
}

void ATurret::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AAIController* AIController = Cast<AAIController>(NewController);
	if (IsValid(AIController))
	{
		FGenericTeamId TeamID((uint8)Team);
		AIController->SetGenericTeamId(TeamID);
	}
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	switch (CurrentTurretState)
	{
		case ETurretState::Searching:
		{
			SearchingMovement(DeltaTime);
			break;
		}

		case ETurretState::Firing:
		{
			FiringMovement(DeltaTime);
			break;
		}
	}
}

void ATurret::OnCurrentTargetSet()
{
	ETurretState NewState;
	if (IsValid(CurrentTarget))
	{
		NewState = ETurretState::Firing;
	}
	else
	{
		NewState = ETurretState::Searching;
		CurrentTarget = nullptr;
	}
	SetCurrentTurretState(NewState);
}

FVector ATurret::GetPawnViewLocation() const
{
	return WeaponBarell->GetComponentLocation();
}

FRotator ATurret::GetViewRotation() const
{
	return WeaponBarell->GetComponentRotation();
}

void ATurret::SearchingMovement(float DeltaTime)
{
	FRotator TurretBaseRotation = TurrentBaseComponent->GetRelativeRotation();
	TurretBaseRotation.Yaw += DeltaTime * BaseSearchingRotationRate;
	TurrentBaseComponent->SetRelativeRotation(TurretBaseRotation);

	FRotator TurretBarellRotation = TurretBarellComponent->GetRelativeRotation();
	TurretBarellRotation.Pitch = FMath::FInterpTo(TurretBarellRotation.Pitch, 0.0f, DeltaTime, BarellPitchRotationRate);
	TurretBarellComponent->SetRelativeRotation(TurretBarellRotation);
}

void ATurret::FiringMovement(float DeltaTime)
{
	FVector BaseLookAtDirection = (CurrentTarget->GetActorLocation() - TurrentBaseComponent->GetComponentLocation()).GetSafeNormal2D();
	FQuat LookAtQuat = BaseLookAtDirection.ToOrientationQuat();
	FQuat TargetQuat = FMath::QInterpTo(TurrentBaseComponent->GetComponentQuat(), LookAtQuat, DeltaTime, BaseFiringInterpSpeed);
	TurrentBaseComponent->SetWorldRotation(TargetQuat);

	FVector BarellLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBarellComponent->GetComponentLocation()).GetSafeNormal();
	float LookAtPitchAngle = BarellLookAtDirection.ToOrientationRotator().Pitch;
	FRotator BarellLocalRotation = TurretBarellComponent->GetRelativeRotation();
	BarellLocalRotation.Pitch = FMath::FInterpTo(BarellLocalRotation.Pitch, LookAtPitchAngle, DeltaTime, BarellPitchRotationRate);
	TurretBarellComponent->SetRelativeRotation(BarellLocalRotation);
}

float ATurret::GetFireInterval() const
{
	return 60.0f / RateOfFire;
}

void ATurret::MakeShot()
{
	FVector ShotLocation = WeaponBarell->GetComponentLocation();
	FVector ShotDirection = WeaponBarell->GetComponentRotation().RotateVector(FVector::ForwardVector);
	float SpreadAngle = FMath::DegreesToRadians(BulletSpreadAngle);
	WeaponBarell->Shot(ShotLocation, ShotDirection, SpreadAngle);
}

void ATurret::SetCurrentTurretState(ETurretState NewState)
{
	bool bIsStateChanged = NewState != CurrentTurretState;
	CurrentTurretState = NewState;
	if (!bIsStateChanged)
	{
		return;
	}

	switch (CurrentTurretState)
	{
		case ETurretState::Searching:
		{
			GetWorldTimerManager().ClearTimer(ShotTimer);
			break;
		}
		case ETurretState::Firing:
		{
			GetWorldTimerManager().SetTimer(ShotTimer, this, &ATurret::MakeShot, GetFireInterval(), true, FireDelayTimer);
			break;
		}
	}
}

void ATurret::OnTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	if (CurrentHealth <= 0.0f)
	{
		DestroyTurret();
	}
}

void ATurret::DestroyTurret()
{
	if (IsValid(ExplosionVFX))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetActorLocation());
	}
	Destroy();
}

void ATurret::OnRep_CurrentTarget()
{
	OnCurrentTargetSet();
}

