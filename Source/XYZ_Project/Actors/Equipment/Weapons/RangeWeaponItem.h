#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "RangeWeaponItem.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnReloadComplete);

UENUM(BlueprintType)
enum class EWeaponFireMode : uint8 
{
	Single,
	FullAuto
};

UENUM(BlueprintType)
enum class EReloadType : uint8
{
	FullClip,
	ByBullet
};

class UAnimMontage;
class UWeaponBarellComponent;

UCLASS(Blueprintable)
class XYZ_PROJECT_API ARangeWeaponItem : public AEquipableItem
{
	GENERATED_BODY()
	
public:
	ARangeWeaponItem();

	virtual void BeginPlay() override;

	void StartFire();
	void StopFire();
	bool IsFiring() const;

	void StartAim();
	void StopAim();
	bool IsReloading() const;

	void StartReload();
	void EndReload(bool bIsSuccess);

	void NextWeaponBarell();

	float GetAimFOV() const;
	float GetAimMovementMaxSpeed() const;
	float GetAimTurnModifier() const;
	float GetAimLookUpModifier() const;

	virtual void CreatePools() override;

	UWeaponBarellComponent* GetCurrentBarellComponent() const;

	virtual EReticleType GetReticleType() const override;

	FTransform GetForeGripTransform() const;

	FOnReloadComplete OnReloadComplete;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWeaponBarellComponent* WeaponBarell;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
	UAnimMontage* WeaponFireMontage;	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
	UAnimMontage* WeaponReloadMontage;
	//Fullclip reload type adds ammo only when the whole animation is successfuly played
	//ByBullet reload type requires section "ReloadEnd" in character reload animation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
	EReloadType ReloadType = EReloadType::FullClip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Character")
	UAnimMontage* CharacterFireMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Character")
	UAnimMontage* CharacterReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Firing")
	EWeaponFireMode WeaponFireMode = EWeaponFireMode::Single;

	// Rate of fire in rounds per minute
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Firing", meta = (UIMin = 1.0f, ClampMin = 1.0f))
	float RateOfFire = 600.0f;
	// Bullet spread half angle in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Firing", meta = (UIMin = 0.0f, ClampMin = 0.0f, UIMax = 5.0f, ClampMax = 5.0f))
	float SpreadAngle = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (UIMin = 0.0f, ClampMin = 0.0f, UIMax = 5.0f, ClampMax = 5.0f))
	float AimSpreadAngle = 0.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float AimMovementMaxSpeed = 200.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (UIMin = 0.0f, ClampMin = 0.0f, UIMax = 120.0f, ClampMax = 120.0f))
	float AimFOV = 60.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (UIMin = 0.0f, ClampMin = 0.0f, UIMax = 1.0f, ClampMax = 1.0f))
	float AimTurnModifier = 0.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Aiming", meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float AimLookUpModifier = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Ammo")
	bool bAutoReload = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle")
	EReticleType AimReticleType = EReticleType::Default;

	UWeaponBarellComponent* CurrentWeaponBarell;
	TArray<UWeaponBarellComponent*> BarellsArray;
	int32 CurrentBarellIndex;

private:
	bool bIsReloading = false;
	bool bIsFiring = false;

	void OnShotTimerElapsed();

	void MakeShot();
	// Returns bullet spread angle (in radians) depending on whether we aim or not 
	float GetCurrentBulletSpreadAngle() const;

	float GetShotTimerInterval() const;

	float PlayAnimMontage(UAnimMontage* AnimMontage);
	void StopAnimMontage(UAnimMontage* AnimMontage, float BlendOutTime = 0.0f);

	FTimerHandle ShotTimer;
	FTimerHandle ReloadTimer;

	bool bIsAiming = false;
};
