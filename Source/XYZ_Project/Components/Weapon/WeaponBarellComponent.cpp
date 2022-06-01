#include "WeaponBarellComponent.h"
#include "XYZ_ProjectTypes.h"
#include <Subsystems/DebugSubsystem.h>
#include <DrawDebugHelpers.h>
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Components/DecalComponent.h>
#include "Actors/Projectiles/XYZProjectile.h"
#include <AI/Perception/Senses/AISense_DamageSight.h>
#include <Net/UnrealNetwork.h>


UWeaponBarellComponent::UWeaponBarellComponent()
{
	SetIsReplicatedByDefault(true);
}

void UWeaponBarellComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams RepParams;
	RepParams.Condition = COND_SimulatedOnly;
	RepParams.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS(UWeaponBarellComponent, LastShotsInfo, RepParams);
	DOREPLIFETIME(UWeaponBarellComponent, ProjectilePool);
}

void UWeaponBarellComponent::BeginPlay()
{
	Super::BeginPlay();
	SetAmmo(MaxAmmo);
	if (HitRegistration == EHitRegistrationType::Projectile)
	{
		ProjectilePoolSize = MaxAmmo;
	}
}

void UWeaponBarellComponent::CreateProjectilePool()
{
	if (!IsValid(ProjectileClass) || HitRegistration != EHitRegistrationType::Projectile)
	{
		return;
	}

	ProjectilePool.Reserve(ProjectilePoolSize);
	for (int32 i = 0; i < ProjectilePoolSize; ++i)
	{
		AXYZProjectile* Projectile = GetWorld()->SpawnActor<AXYZProjectile>(ProjectileClass, ProjectilePoolLocation, FRotator::ZeroRotator);
		Projectile->SetOwner(GetOwningPawn());
		Projectile->SetProjectileActive(false);
		ProjectilePool.Add(Projectile);
	}
}

int32 UWeaponBarellComponent::GetAmmo() const
{
	return Ammo;
}

void UWeaponBarellComponent::SetAmmo(int32 NewAmmo)
{
	Ammo = FMath::Clamp(NewAmmo, 0, MaxAmmo);
	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(Ammo);
	}
}

bool UWeaponBarellComponent::CanShoot()
{
	return Ammo > 0;
}
int32 UWeaponBarellComponent::GetMaxAmmo() const
{
	return MaxAmmo;
}

EAmunitionType UWeaponBarellComponent::GetAmmoType() const
{
	return AmmoType;
}

void UWeaponBarellComponent::Shot(FVector ShotStart, FVector ShotDirection, float SpreadAngle)
{
	TArray<FShotInfo> ShotsInfo;
	for (int i = 0; i < BulletsPerShot; i++)
	{
		ShotDirection += GetBulletSpreadOffset(FMath::RandRange(0.0f, SpreadAngle), ShotDirection.ToOrientationRotator());
		ShotDirection = ShotDirection.GetSafeNormal();
		ShotsInfo.Emplace(ShotStart, ShotDirection);
	}

	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_Shot(ShotsInfo);
	}
	ShotInternal(ShotsInfo);
}


bool UWeaponBarellComponent::HitScan(const FVector& ShotStart, OUT FVector& ShotEnd, const FVector& ShotDirection)
{
	FHitResult ShotResult;
	bool bHasHit = GetWorld()->LineTraceSingleByChannel(ShotResult, ShotStart, ShotEnd, ECC_Bullet);
	if (bHasHit)
	{
		ShotEnd = ShotResult.ImpactPoint;	
		float CurrentShotRange = (ShotEnd - ShotStart).Size();
		ProcessHit(ShotResult, ShotDirection, CurrentShotRange);
	}
	return bHasHit;
}

void UWeaponBarellComponent::LaunchProjectile(const FVector& LaunchStart, const FVector& LaunchDirection)
{
	AXYZProjectile* Projectile = ProjectilePool[CurrentProjectileIndex];
	if (!IsValid(Projectile))
	{
		return;
	}
	Projectile->SetActorLocation(LaunchStart);
	Projectile->SetActorRotation(LaunchDirection.ToOrientationRotator());
	Projectile->SetProjectileActive(true);
	//If bSetInProjectile is true, the damage dealing logic should be in the projectile class 
	if (!bSetDamageInProjectile)
	{
		Projectile->OnProjectileHit.AddDynamic(this, &UWeaponBarellComponent::ProcessProjectileHit);
	}
	Projectile->OnProjectileDestroy.AddDynamic(this, &UWeaponBarellComponent::BackProjectileToPool);
	Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal(), LaunchStart);
	++CurrentProjectileIndex;
	if (CurrentProjectileIndex == ProjectilePool.Num())
	{
		CurrentProjectileIndex = 0;
	}
}

void UWeaponBarellComponent::BackProjectileToPool(AXYZProjectile* Projectile)
{
	Projectile->SetProjectileActive(false);
	Projectile->SetActorLocation(ProjectilePoolLocation);
	Projectile->SetActorRotation(FRotator::ZeroRotator);
	Projectile->OnProjectileDestroy.RemoveAll(this);
}

void UWeaponBarellComponent::ShotInternal(const TArray<FShotInfo>& ShotsInfo)
{
	if (GetOwner()->HasAuthority())
	{
		LastShotsInfo = ShotsInfo;
	}

	FVector MuzzleLocation = GetComponentLocation();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashFX, MuzzleLocation, GetComponentRotation());

	for (const FShotInfo& ShotInfo : ShotsInfo)
	{
		FVector ShotStart = ShotInfo.GetLocation();
		FVector ShotDirection = ShotInfo.GetDirection();
		FVector ShotEnd = ShotStart + FiringRange * ShotDirection;

		bool bDrawDebug = UDebugSubsystem::GetDebugSubsystem()->IsCategoryEnabled(DebugCategoryRangeWeapon);
		switch (HitRegistration)
		{
		case EHitRegistrationType::Hitscan:
		{
			bool bHasHit = HitScan(ShotStart, ShotEnd, ShotDirection);
			if (bDrawDebug && bHasHit)
			{
				DrawDebugSphere(GetWorld(), ShotEnd, 10.0f, 24, FColor::Red, false, 1.0f);
			}
			break;
		}
		case  EHitRegistrationType::Projectile:
		{
			LaunchProjectile(ShotStart, ShotDirection);
			break;
		}
		}

		UNiagaraComponent* TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, MuzzleLocation, GetComponentRotation());
		if (IsValid(TraceFXComponent))
		{	
			TraceFXComponent->SetVectorParameter(FXParamTraceEnd, ShotEnd);
		}
		if (bDrawDebug)
		{
			DrawDebugLine(GetWorld(), MuzzleLocation, ShotEnd, FColor::Red, false, 1.0f, 0, 3.0f);
		}
	}
}

void UWeaponBarellComponent::Server_Shot_Implementation(const TArray<FShotInfo>& ShotsInfo)
{
	ShotInternal(ShotsInfo);
}

void UWeaponBarellComponent::OnRep_LastShotsInfo()
{
	ShotInternal(LastShotsInfo);
}

APawn* UWeaponBarellComponent::GetOwningPawn() const
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (!IsValid(PawnOwner))
	{
		PawnOwner = Cast<APawn>(GetOwner()->GetOwner());
	}
	return PawnOwner;
}

AController* UWeaponBarellComponent::GetController() const
{
	APawn* PawnOwner = GetOwningPawn();
	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;
}

void UWeaponBarellComponent::ProcessHit(const FHitResult& HitResult, const FVector& Direction, float ShotRange)
{
	AActor* HitActor = HitResult.GetActor();
	if (IsValid(HitActor) && GetOwner()->HasAuthority())
	{
		float AppliedDamage = DamageAmount;
		if (IsValid(FallOffDamageDiagram))
		{
			AppliedDamage = DamageAmount * FallOffDamageDiagram->GetFloatValue(ShotRange);
		}
		FPointDamageEvent DamageEvent;
		DamageEvent.HitInfo = HitResult;
		DamageEvent.ShotDirection = Direction;
		DamageEvent.DamageTypeClass = DamageTypeClass;
		HitActor->TakeDamage(AppliedDamage, DamageEvent, GetController(), GetOwner());
		UAISense_DamageSight::ReportDamageSightEvent(GetWorld(), HitActor, GetOwningPawn(), AppliedDamage, GetOwningPawn()->GetActorLocation(), HitResult.ImpactPoint);
	}

	UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecalInfo.DecalMaterial, DefaultDecalInfo.DecalSize, HitResult.ImpactPoint, HitResult.ImpactNormal.ToOrientationRotator());
	if (IsValid(DecalComponent))
	{
		DecalComponent->SetFadeScreenSize(0.0001f);
		DecalComponent->SetFadeOut(DefaultDecalInfo.DecalLifeTime, DefaultDecalInfo.DecalFadeOutTime);
	}
}

void UWeaponBarellComponent::ProcessProjectileHit(AXYZProjectile* Projectile, const FHitResult& HitResult, const FVector& Direction, float ShotRange)
{
	Projectile->OnProjectileHit.RemoveAll(this);
	ProcessHit(HitResult, Direction, ShotRange);
}

FVector UWeaponBarellComponent::GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const
{
	float SpreadSize = FMath::Tan(Angle);
	float RotationAngle = FMath::RandRange(0.0f, 2 * PI);

	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadZ = FMath::Sin(RotationAngle);

	return (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ + ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;
}
