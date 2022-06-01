#include "ThrowableItem.h"
#include "Actors/Projectiles/XYZProjectile.h"
#include "Characters/XYZBaseCharacter.h"
#include <Net/UnrealNetwork.h>


AThrowableItem::AThrowableItem()
{
	ProjectilePoolSize = MaxThrowAmmo;
}

void AThrowableItem::BeginPlay()
{
	Super::BeginPlay();
	SetThrowAmmo(MaxThrowAmmo);
}

void AThrowableItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams RepParams;
	RepParams.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS(AThrowableItem, CurrentThrowInfo, RepParams);
	DOREPLIFETIME(AThrowableItem, ProjectilePool);
	DOREPLIFETIME(AThrowableItem, bIsThrowing);
}

void AThrowableItem::CreatePools()
{
	if (GetOwner()->GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	if (!IsValid(ProjectileClass))
	{
		return;
	}

	ProjectilePool.Reserve(ProjectilePoolSize);
	for (int32 i = 0; i < ProjectilePoolSize; ++i)
	{
		AXYZProjectile* Projectile = GetWorld()->SpawnActor<AXYZProjectile>(ProjectileClass, ProjectilePoolLocation, FRotator::ZeroRotator);
		Projectile->SetOwner(GetCharacterOwner());
		Projectile->SetProjectileActive(false);
		ProjectilePool.Add(Projectile);
	}
}

void AThrowableItem::Throw()
{
	AXYZBaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner) || !IsValid(ThrowMontage) || !CanThrow())
	{
		return;
	}

	if (CharacterOwner->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_Throw();
	}

	if (CharacterOwner->HasAuthority())
	{
		bIsThrowing = true;
	}

	float ThrowDuration = CharacterOwner->PlayAnimMontage(ThrowMontage);
	GetWorld()->GetTimerManager().SetTimer(ThrowTimer, this, &AThrowableItem::ThrowAnimationFinished, ThrowDuration, false);
	CharacterOwner->OnStartThrow();
}

void AThrowableItem::Server_Throw_Implementation()
{
	bIsThrowing = true;
	PlayThrowMontage();
}

void AThrowableItem::LaunchItemProjectileInternal(const FThrowInfo& ThrowInfo)
{
	if (GetOwner()->HasAuthority())
	{
		CurrentThrowInfo = ThrowInfo;
	}

	AXYZProjectile* Projectile = ProjectilePool[CurrentProjectileIndex];
	if (!IsValid(Projectile))
	{
		return;
	}

	FVector LaunchDirection = ThrowInfo.GetDirection();
	FVector SpawnLocation = ThrowInfo.GetLocation();
	Projectile->SetActorLocation(SpawnLocation);
	Projectile->SetActorRotation(LaunchDirection.ToOrientationRotator());
	Projectile->SetProjectileActive(true);
	Projectile->OnProjectileDestroy.AddDynamic(this, &AThrowableItem::BackToPool);
	Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal(), SpawnLocation);
	SetThrowAmmo(--ThrowAmmo);
	++CurrentProjectileIndex;
	if (CurrentProjectileIndex == ProjectilePool.Num())
	{
		CurrentProjectileIndex = 0;
	}
}


void AThrowableItem::PlayThrowMontage()
{
	AXYZBaseCharacter* CharacterOwner = GetCharacterOwner();
	float ThrowDuration = CharacterOwner->PlayAnimMontage(ThrowMontage);
	GetWorld()->GetTimerManager().SetTimer(ThrowTimer, this, &AThrowableItem::ThrowAnimationFinished, ThrowDuration, false);
}

void AThrowableItem::Server_LaunchItemProjectile_Implementation(const FThrowInfo& ThrowInfo)
{
	LaunchItemProjectileInternal(ThrowInfo);
}

void AThrowableItem::OnRep_ThrowInfo()
{
	LaunchItemProjectileInternal(CurrentThrowInfo);
}

void AThrowableItem::OnRep_IsThrowing()
{
	if (bIsThrowing && GetLocalRole() == ROLE_SimulatedProxy)
	{
		PlayThrowMontage();
	}
}

void AThrowableItem::BackToPool(AXYZProjectile* Projectile)
{
	Projectile->SetProjectileActive(false);
	Projectile->SetActorLocation(ProjectilePoolLocation);
	Projectile->SetActorRotation(FRotator::ZeroRotator);
	Projectile->OnProjectileDestroy.RemoveAll(this);
}

void AThrowableItem::LaunchItemProjectile()
{
	AXYZBaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner))
	{
		return;
	}

	FVector LaunchDirection;
	FVector SpawnLocation;
	if (CharacterOwner->IsPlayerControlled())
	{
		FVector PlayerViewPoint;
		FRotator PlayerViewRotation;
		APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
		Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);
		FTransform PlayerViewTransform(PlayerViewRotation, PlayerViewPoint);
		FVector PlayerViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);
		FVector ViewUpVector = PlayerViewRotation.RotateVector(FVector::UpVector);

		LaunchDirection = PlayerViewDirection + FMath::Tan(FMath::DegreesToRadians(ThrowAngle)) * ViewUpVector;

		FVector	ThrowableSocketLocation = CharacterOwner->GetMesh()->GetSocketLocation(SocketCharacterThrowable);
		FVector SocketInViewSpace = PlayerViewTransform.InverseTransformPosition(ThrowableSocketLocation);

		SpawnLocation = PlayerViewPoint + PlayerViewDirection * SocketInViewSpace.X;
	}
	else
	{
		LaunchDirection = CharacterOwner->GetActorForwardVector() + FMath::Tan(FMath::DegreesToRadians(ThrowAngle)) * CharacterOwner->GetActorUpVector();
		SpawnLocation = CharacterOwner->GetMesh()->GetSocketLocation(SocketCharacterThrowable);
	}

	FThrowInfo ThrowInfo(SpawnLocation, LaunchDirection);

	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_LaunchItemProjectile(ThrowInfo);
	}
	else if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		LaunchItemProjectileInternal(ThrowInfo);
	}
}

bool AThrowableItem::CanThrow() const
{
	return ThrowAmmo > 0 && !bIsThrowing;
}

bool AThrowableItem::GetIsThrowing() const
{
	return bIsThrowing;
}

void AThrowableItem::ThrowAnimationFinished()
{
	bIsThrowing = false;
}

EAmunitionType AThrowableItem::GetThrowAmmoType() const
{
	return ThrowAmmoType;
}

int32 AThrowableItem::GetThrowAmmo() const
{
	return ThrowAmmo;
}

int32 AThrowableItem::GetMaxThrowAmmo() const
{
	return MaxThrowAmmo;
}

void AThrowableItem::SetThrowAmmo(int32 NewAmmo)
{
	ThrowAmmo = NewAmmo;
	if (OnThrowAmmoChanged.IsBound())
	{
		OnThrowAmmoChanged.Broadcast(ThrowAmmo);
	}
}
