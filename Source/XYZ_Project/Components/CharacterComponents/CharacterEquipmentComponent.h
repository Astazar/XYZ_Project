#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XYZ_ProjectTypes.h"
#include "CharacterEquipmentComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentWeaponAmmoChanged, int32);

class ARangeWeaponItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XYZ_PROJECT_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	EEquipableItemType GetCurrentEquippedItemType() const;

	ARangeWeaponItem* GetCurrentRangeWeapon() const;

	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChangedEvent;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSubclassOf<ARangeWeaponItem> SideArmClass;

private:
	void CreateLoadout();

	UFUNCTION()
	void OnCurrentWeaponAmmoChanged(int32 Ammo);

	ARangeWeaponItem* CurrentEquippedWeapon;
	TWeakObjectPtr<class AXYZBaseCharacter> CachedBaseCharacter;
};
