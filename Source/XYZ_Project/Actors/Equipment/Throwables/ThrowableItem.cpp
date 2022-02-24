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
	if (!CanThrow())
	{
		return;
	}

	checkf(GetOwner()->IsA<AXYZBaseCharacter>(), TEXT("AThrowableItem::Throw() can work only with AXYZBaseCharacter"));
	AXYZBaseCharacter* CharacterOwner = StaticCast<AXYZBaseCharacter*>(GetOwner());

	APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
	if (!IsValid(Controller))
	{
		return;
	}

	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;
	Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);

	FTransform PlayerViewTransform(PlayerViewRotation, PlayerViewPoint);
	FVector PlayerViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);
	FVector ViewUpVector = PlayerViewRotation.RotateVector(FVector::UpVector);

	FVector LaunchDirection = PlayerViewDirection + FMath::Tan(FMath::DegreesToRadians(ThrowAngle)) * ViewUpVector;

	FVector	ThrowableSocketLocation = CharacterOwner->GetMesh()->GetSocketLocation(SocketCharacterThrowable);
	FVector SocketInViewSpace = PlayerViewTransform.InverseTransformPosition(ThrowableSocketLocation);

	FVector SpawnLocation = PlayerViewPoint + PlayerViewDirection * SocketInViewSpace.X;
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
	return ThrowAmmo > 0;
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
