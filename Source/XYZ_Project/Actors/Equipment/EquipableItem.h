#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XYZ_ProjectTypes.h"
#include "EquipableItem.generated.h"

class UAnimMontage;

UCLASS(Abstract, NotBlueprintable)
class XYZ_PROJECT_API AEquipableItem : public AActor
{
	GENERATED_BODY()

public:
	EEquipableItemType GetItemType() const;

	UAnimMontage* GetCharacterEquipAnimMontage() const;

	FName GetUnequippedSocketName() const;

	FName GetEquippedSocketName() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	EEquipableItemType ItemType = EEquipableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	UAnimMontage* CharacterEquipAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName UnequippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName EquippedSocketName = NAME_None;
};
