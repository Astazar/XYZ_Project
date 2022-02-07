#include "WeaponBarellComponent.h"
#include "XYZ_ProjectTypes.h"
#include <Subsystems/DebugSubsystem.h>
#include <DrawDebugHelpers.h>


void UWeaponBarellComponent::Shot(FVector ShotStart, FVector ShotDirection, AController* Controller)
{
	FVector MuzzleLocation = GetComponentLocation();
	FVector ShotEnd = ShotStart + FiringRange * ShotDirection;
	FHitResult ShotResult;
	bool bDrawDebug = UDebugSubsystem::GetDebugSubsystem()->IsCategoryEnabled(DebugCategoryRangeWeapon);
	if (GetWorld()->LineTraceSingleByChannel(ShotResult, ShotStart, ShotEnd, ECC_Bullet))
	{
		ShotEnd = ShotResult.ImpactPoint;
		if (bDrawDebug)
		{
			DrawDebugSphere(GetWorld(), ShotEnd, 10.0f, 24, FColor::Red, false, 1.0f);
		}
		AActor* HitActor = ShotResult.GetActor();
		if (IsValid(HitActor))
		{
			HitActor->TakeDamage(DamageAmount, FDamageEvent(), Controller, GetOwner());
		}
	}
	if (bDrawDebug)
	{
		DrawDebugLine(GetWorld(), MuzzleLocation, ShotEnd, FColor::Red, false, 1.0f, 0, 3.0f);
	}
}
