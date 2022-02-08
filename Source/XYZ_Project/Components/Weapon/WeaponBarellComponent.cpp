#include "WeaponBarellComponent.h"
#include "XYZ_ProjectTypes.h"
#include <Subsystems/DebugSubsystem.h>
#include <DrawDebugHelpers.h>
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


void UWeaponBarellComponent::Shot(FVector ShotStart, FVector ShotDirection, AController* Controller)
{
	FVector MuzzleLocation = GetComponentLocation();
	FVector ShotEnd = ShotStart + FiringRange * ShotDirection;

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),MuzzleFlashFX, MuzzleLocation, GetComponentRotation());

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
			float AppliedDamage = DamageAmount;
			if (IsValid(FallOffDamageDiagram))
			{
				float CurrentShotRange = (ShotEnd - ShotStart).Size();
				AppliedDamage = DamageAmount * FallOffDamageDiagram->GetFloatValue(CurrentShotRange);
			}
			HitActor->TakeDamage(AppliedDamage, FDamageEvent(), Controller, GetOwner());
		}
	}

	UNiagaraComponent* TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, MuzzleLocation, GetComponentRotation());
	TraceFXComponent->SetVectorParameter(FXParamTraceEnd, ShotEnd);

	if (bDrawDebug)
	{
		DrawDebugLine(GetWorld(), MuzzleLocation, ShotEnd, FColor::Red, false, 1.0f, 0, 3.0f);
	}
}
