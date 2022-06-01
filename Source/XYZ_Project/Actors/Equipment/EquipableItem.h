#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XYZ_ProjectTypes.h"
#include "EquipableItem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentStateChanged, bool, bIsEquipped);

class UAnimMontage;
class AXYZBaseCharacter;

UCLASS(Abstract, NotBlueprintable)
class XYZ_PROJECT_API AEquipableItem : public AActor
{
	GENERATED_BODY()

public:
	AEquipableItem();

	virtual void SetOwner(AActor* NewOwner) override;

	EEquipableItemType GetItemType() const;

	UAnimMontage* GetCharacterEquipAnimMontage() const;

	FName GetUnequippedSocketName() const;

	FName GetEquippedSocketName() const;

	virtual void Equip();
	virtual void Unequip();

	virtual EReticleType GetReticleType() const;

	virtual void CreatePools() {};

protected:
	UPROPERTY(BlueprintAssignable)
	FOnEquipmentStateChanged OnEquipmentStateChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	EEquipableItemType ItemType = EEquipableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	UAnimMontage* CharacterEquipAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName UnequippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName EquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle")
	EReticleType ReticleType = EReticleType::None;

	AXYZBaseCharacter* GetCharacterOwner() const;

private:
	TWeakObjectPtr<AXYZBaseCharacter> CachedCharacterOwner;
};
