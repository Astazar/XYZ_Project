#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <Engine/DataTable.h>
#include "InventoryItem.generated.h"


class APickableItem;
class AEquipableItem;

USTRUCT(BlueprintType)
struct FInventoryItemDescription : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	UTexture2D* Icon;
};


USTRUCT(BlueprintType)
struct FWeaponTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
	TSubclassOf<APickableItem> PickableActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
	TSubclassOf<AEquipableItem> EquipableActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
	FInventoryItemDescription WeaponItemDescription;
};

UCLASS()
class XYZ_PROJECT_API UInventoryItem : public UObject
{
	GENERATED_BODY()
};
