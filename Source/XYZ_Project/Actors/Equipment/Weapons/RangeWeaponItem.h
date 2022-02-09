#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "RangeWeaponItem.generated.h"


UENUM(BlueprintType)
enum class EWeaponFireMode : uint8 
{
	Single,
	FullAuto
};

class UAnimMontage;

UCLASS(Blueprintable)
class XYZ_PROJECT_API ARangeWeaponItem : public AEquipableItem
{
	GENERATED_BODY()
	
public:
	ARangeWeaponItem();

	void StartFire();
	void StopFire();

	void StartAim();
	void StopAim();

	float GetAimFOV() const;
	float GetAimMovementMaxSpeed() const;
	float GetAimTurnModifier() const;
	float GetAimLookUpModifier() const;

	FTransform GetForeGripTransform() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWeaponBarellComponent* WeaponBarell;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
	UAnimMontage* WeaponFireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Character")
	UAnimMontage* CharacterFireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters")
	EWeaponFireMode WeaponFireMode = EWeaponFireMode::Single;

	// Rate of fire in rounds per minute
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters", meta = (UIMin = 1.0f, ClampMin = 1.0f))
	float RateOfFire = 600.0f;
	// Bullet spread half angle in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters", meta = (UIMin = 0.0f, ClampMin = 0.0f, UIMax = 5.0f, ClampMax = 5.0f))
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

private:
	void MakeShot();
	// Returns bullet spread angle (in radians) depending on whether we aim or not 
	float GetCurrentBulletSpreadAngle() const;

	float GetShotTimerInterval() const;

	float PlayAnimMontage(UAnimMontage* AnimMontage);

	FVector GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const;

	FTimerHandle ShotTimer;

	bool bIsAiming = false;
};
