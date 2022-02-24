#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "ThrowableItem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnThrowAmmoChanged, int32);


UCLASS(Blueprintable)
class XYZ_PROJECT_API AThrowableItem : public AEquipableItem
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	void Throw();

	bool CanThrow() const;

	EAmunitionType GetThrowAmmoType() const;

	int32 GetThrowAmmo() const;
	void SetThrowAmmo(int32 NewAmmo);

	int32 GetMaxThrowAmmo() const;

	FOnThrowAmmoChanged OnThrowAmmoChanged;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables | Ammo", meta = (ClampMin = 0, UIMin = 0))
	int32 ThrowAmmo = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables | Ammo", meta = (ClampMin = 0, UIMin = 0))
	int32 MaxThrowAmmo = 5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables | Ammo")
	EAmunitionType ThrowAmmoType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
	TSubclassOf<class AXYZProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throwables", meta = (ClampMin = -90.0f, UIMin = -90.0f, ClampMax = 90.0f, UIMax = 90.0f))
	float ThrowAngle = 0.0f;

};
