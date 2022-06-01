#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include <XYZ_ProjectTypes.h>
#include "WeaponBarellComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoChanged, int32);

UENUM(BlueprintType)
enum class EHitRegistrationType : uint8
{
	Hitscan,
	Projectile
};

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

USTRUCT(BlueprintType)
struct FShotInfo
{
	GENERATED_BODY()

	FShotInfo() : Location_Mul_10(FVector_NetQuantize100::ZeroVector), Direction(FVector_NetQuantizeNormal::ZeroVector) {};
	FShotInfo(FVector Location, FVector Direction) : Location_Mul_10(Location * 10.0f), Direction(Direction) {};

	UPROPERTY()
	FVector_NetQuantize100 Location_Mul_10;

	UPROPERTY()
	FVector_NetQuantizeNormal Direction;

	FVector GetLocation() const { return Location_Mul_10 * 0.1f; }
	FVector GetDirection() const { return Direction; }
};

class UNiagaraSystem;
class AXYZProjectile;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XYZ_PROJECT_API UWeaponBarellComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UWeaponBarellComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	void CreateProjectilePool();

	void Shot(FVector ShotStart, FVector ShotDirection, float SpreadAngle);

	bool CanShoot();

	int32 GetAmmo() const;
	void SetAmmo(int32 NewAmmo);

	int32 GetMaxAmmo() const;

	EAmunitionType GetAmmoType() const;

	FOnAmmoChanged OnAmmoChanged;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Ammo", meta = (UIMin = 0, ClampMin = 0))
	int32 Ammo = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Ammo", meta = (UIMin = 1, ClampMin = 1))
	int32 MaxAmmo = 30;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Ammo")
	EAmunitionType AmmoType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes")
	float FiringRange = 5000.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes", meta = (ClampMin = 1, UIMin = 1))
	int32 BulletsPerShot = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Hit registration")
	EHitRegistrationType HitRegistration = EHitRegistrationType::Hitscan;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Hit registration", meta = (EditCondition = "HitRegistration == EHitRegistrationType::Projectile"))
	TSubclassOf<AXYZProjectile> ProjectileClass;

	//If true damage settings will be configurable in projectile and not in the barell
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Damage", meta = (EditCondition = "HitRegistration == EHitRegistrationType::Projectile"))
	bool bSetDamageInProjectile = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Damage", meta = (EditCondition = "!bSetDamageInProjectile"))
	TSubclassOf<class UDamageType> DamageTypeClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Damage", meta = (EditCondition = "!bSetDamageInProjectile"))
	float DamageAmount = 20.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Damage", meta = (EditCondition = "!bSetDamageInProjectile"))
	class UCurveFloat* FallOffDamageDiagram;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | VFX")
	UNiagaraSystem* MuzzleFlashFX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | VFX")
	UNiagaraSystem* TraceFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Decals")
	FDecalInfo DefaultDecalInfo;

	UFUNCTION()
	void BackProjectileToPool(AXYZProjectile* Projectile);

private:
	void ShotInternal(const TArray<FShotInfo>& ShotsInfo);

	UFUNCTION(Server, Reliable)
	void Server_Shot(const TArray<FShotInfo>& ShotsInfo);

	UPROPERTY(ReplicatedUsing=OnRep_LastShotsInfo)
	TArray<FShotInfo> LastShotsInfo;

	UFUNCTION()
	void OnRep_LastShotsInfo();

	UPROPERTY(Replicated)
	TArray<AXYZProjectile*> ProjectilePool;

	int32 CurrentProjectileIndex;
	int32 ProjectilePoolSize = 0;

	const FVector ProjectilePoolLocation = FVector(0.0f, 0.0f, -300.0f);

	APawn* GetOwningPawn() const;
	AController* GetController() const;

	UFUNCTION()
	void ProcessHit(const FHitResult& HitResult, const FVector& Direction, float ShotRange);
	UFUNCTION()
	void ProcessProjectileHit(AXYZProjectile* Projectile, const FHitResult& HitResult, const FVector& Direction, float ShotRange);

	bool HitScan(const FVector& ShotStart, OUT FVector& ShotEnd, const FVector& ShotDirection);
	void LaunchProjectile(const FVector& LaunchStart, const FVector& LaunchDirection);

	FVector GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const;
};
