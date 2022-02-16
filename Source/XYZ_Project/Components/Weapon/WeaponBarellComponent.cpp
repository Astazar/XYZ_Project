#include "WeaponBarellComponent.h"
#include "XYZ_ProjectTypes.h"
#include <Subsystems/DebugSubsystem.h>
#include <DrawDebugHelpers.h>
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Components/DecalComponent.h>


void UWeaponBarellComponent::Shot(FVector ShotStart, FVector ShotDirection, AController* Controller, float SpreadAngle)
{
	for (int i = 0; i < BulletsPerShot; i++)
	{
		ShotDirection += GetBulletSpreadOffset(FMath::RandRange(0.0f, SpreadAngle), ShotDirection.ToOrientationRotator());

		FVector MuzzleLocation = GetComponentLocation();
		FVector ShotEnd = ShotStart + FiringRange * ShotDirection;

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashFX, MuzzleLocation, GetComponentRotation());

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
				FPointDamageEvent DamageEvent;
				DamageEvent.HitInfo = ShotResult;
				DamageEvent.ShotDirection = ShotDirection;
				DamageEvent.DamageTypeClass = DamageTypeClass;
				HitActor->TakeDamage(AppliedDamage, DamageEvent, Controller, GetOwner());

				UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecalInfo.DecalMaterial, DefaultDecalInfo.DecalSize, ShotEnd, ShotEnd.ToOrientationRotator());
				if (IsValid(DecalComponent))
				{
					DecalComponent->SetFadeScreenSize(0.0001f);
					DecalComponent->SetFadeOut(DefaultDecalInfo.DecalLifeTime, DefaultDecalInfo.DecalFadeOutTime);
				}
			}
		}

		UNiagaraComponent* TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, MuzzleLocation, GetComponentRotation());
		TraceFXComponent->SetVectorParameter(FXParamTraceEnd, ShotEnd);

		if (bDrawDebug)
		{
			DrawDebugLine(GetWorld(), MuzzleLocation, ShotEnd, FColor::Red, false, 1.0f, 0, 3.0f);
		}
	}
}


FVector UWeaponBarellComponent::GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const
{
	float SpreadSize = FMath::Tan(Angle);
	float RotationAngle = FMath::RandRange(0.0f, 2 * PI);

	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadZ = FMath::Sin(RotationAngle);

	return (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ + ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;
}
