#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XYZ_ProjectTypes.h"
#include "CharacterEquipmentComponent.generated.h"

typedef TArray<class AEquipableItem*, TInlineAllocator<(uint32)EEquipmentSlots::MAX>> TItemsArray;
typedef TArray<int32, TInlineAllocator<(uint32)EAmunitionType::MAX>> TAmunitionArray;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChanged, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentThrowItemAmmoChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedItemChanged, const AEquipableItem*);

class ARangeWeaponItem;
class AThrowableItem;
class AMeleeWeaponItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XYZ_PROJECT_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	void ReloadCurrentWeapon();
	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo = 0, bool bCheckIsFull = false);

	bool IsEquipping() const;

	void EquipItemInSlot(EEquipmentSlots Slot);
	void UnequipCurrentItem();

	void EquipAnimationFinished();

	void AttachCurrentItemToEquippedSocket();

	void EquipNextItem();
	void EquipPreviousItem();

	void NextWeaponBarell();

	void LaunchCurrentThrowableItem();

	EEquipableItemType GetCurrentEquippedItemType() const;
	ARangeWeaponItem* GetCurrentRangeWeapon() const;
	AThrowableItem* GetCurrentThrowableItem() const;
	AMeleeWeaponItem* GetCurrentMeleeWeapon() const;


	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChangedEvent;
	FOnCurrentThrowItemAmmoChanged OnCurrentThrowItemAmmoChangedEvent;
	FOnEquippedItemChanged OnEquippedItemChanged;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EAmunitionType, int32> MaxAmunitionAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EEquipmentSlots, TSubclassOf<class AEquipableItem>> ItemsLoadout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSet<EEquipmentSlots> IgnoreSlotsWhileSwitching;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	EEquipmentSlots AutoEquippedSlot = EEquipmentSlots::None;

private:
	void CreateLoadout();

	void AutoEquip();

	uint32 NextItemsArraySlotIndex(uint32 CurrentSlotIndex);
	uint32 PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex);

	int32 GetAvailableAmunitionForCurrentWeapon();

	UFUNCTION()
	void OnWeaponReloadComplete();

	UFUNCTION()
	void OnCurrentWeaponAmmoChanged(int32 Ammo);

	UFUNCTION()
	void OnCurrentThrowItemAmmoChanged(int32 Ammo);

	bool bIsEquiping = false;
	FTimerHandle EquipTimer;

	FDelegateHandle OnCurrentWeaponBarellAmmoChangedHandle;
	FDelegateHandle OnCurrentWeaponReloadedHandle;

	TItemsArray ItemsArray;
	TAmunitionArray AmunitionArray;

	EEquipmentSlots CurrentEquippedSlot;
	EEquipmentSlots PreviousEquippedSlot;
	AEquipableItem* CurrentEquippedItem;
	ARangeWeaponItem* CurrentEquippedWeapon;
	AThrowableItem* CurrentThrowableItem;
	AMeleeWeaponItem* CurrentMeleeWeapon;

	TWeakObjectPtr<class AXYZBaseCharacter> CachedBaseCharacter;
};
