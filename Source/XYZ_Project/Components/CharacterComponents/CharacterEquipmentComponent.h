#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XYZ_ProjectTypes.h"
#include "CharacterEquipmentComponent.generated.h"




class ARangeWeaponItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XYZ_PROJECT_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	EEquipableItemType GetCurrentEquippedItemType() const;

	void Fire();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSubclassOf<ARangeWeaponItem> SideArmClass;

private:
	void CreateLoadout();

	ARangeWeaponItem* CurrentEquippedWeapon;
	TWeakObjectPtr<class AXYZBaseCharacter> CachedBaseCharacter;
};
