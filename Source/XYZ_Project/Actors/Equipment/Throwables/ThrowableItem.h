#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/Weapons/WeaponItem.h"
#include "ThrowableItem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnThrowAmmoChanged, int32);


UCLASS(Blueprintable)
class XYZ_PROJECT_API AThrowableItem : public AWeaponItem
{
	GENERATED_BODY()
	
public:
	void Throw();

	virtual void SetAmmo(int32 NewAmmo) override;

	FOnThrowAmmoChanged OnThrowAmmoChanged;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
	TSubclassOf<class AXYZProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throwables", meta = (ClampMin = -90.0f, UIMin = -90.0f, ClampMax = 90.0f, UIMax = 90.0f))
	float ThrowAngle = 0.0f;

};
