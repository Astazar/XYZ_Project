#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "WeaponItem.generated.h"


UCLASS(Blueprintable)
class XYZ_PROJECT_API AWeaponItem : public AEquipableItem
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	virtual bool CanUse() const;

	EAmunitionType GetAmmoType() const;

	int32 GetAmmo() const;
	virtual void SetAmmo(int32 NewAmmo);

	int32 GetMaxAmmo() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Ammo", meta = (ClampMin = 0, UIMin = 0))
	int32 Ammo = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Ammo", meta = (ClampMin = 0, UIMin = 0))
	int32 MaxAmmo = 5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Ammo")
	EAmunitionType AmmoType;
};
