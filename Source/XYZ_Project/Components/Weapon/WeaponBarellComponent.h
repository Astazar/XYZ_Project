#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WeaponBarellComponent.generated.h"


USTRUCT(BlueprintType)
struct FDecalInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
	UMaterialInterface* DecalMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
	FVector DecalSize = FVector(5.0f, 5.0f, 5.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
	float DecalLifeTime = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal info")
	float DecalFadeOutTime = 5.0f; 
};

class UNiagaraSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XYZ_PROJECT_API UWeaponBarellComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	void Shot(FVector ShotStart, FVector ShotDirection, AController* Controller);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes")
	float FiringRange = 5000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Damage")
	float DamageAmount = 20.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Damage")
	class UCurveFloat* FallOffDamageDiagram;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | VFX")
	UNiagaraSystem* MuzzleFlashFX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | VFX")
	UNiagaraSystem* TraceFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Decals")
	FDecalInfo DefaultDecalInfo;
};
