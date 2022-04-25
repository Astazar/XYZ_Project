#include "ThrowableItem.h"
#include "Actors/Projectiles/XYZProjectile.h"
#include "Characters/XYZBaseCharacter.h"


void AThrowableItem::BeginPlay()
{
	Super::BeginPlay();
	SetThrowAmmo(MaxThrowAmmo);
}

void AThrowableItem::Throw()
{
	AXYZBaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner) || !IsValid(ThrowMontage) || !CanThrow())
	{
		return;
	}

	bIsThrowing = true;
	float ThrowDuration = CharacterOwner->PlayAnimMontage(ThrowMontage);
	GetWorld()->GetTimerManager().SetTimer(ThrowTimer, this, &AThrowableItem::ThrowAnimationFinished, ThrowDuration, false);
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

	AXYZProjectile* Projectile = GetWorld()->SpawnActor<AXYZProjectile>(ProjectileClass, SpawnLocation, LaunchDirection.ToOrientationRotator());
	if (IsValid(Projectile))
	{
		Projectile->SetOwner(GetOwner());
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal(), SpawnLocation);
		SetThrowAmmo(--ThrowAmmo);
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
