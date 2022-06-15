#include "PickableWeapon.h"
#include "XYZ_ProjectTypes.h"
#include "Utils/XYZDataTableUtils.h"
#include "Characters/XYZBaseCharacter.h"
#include "Inventory/Items/InventoryItem.h"


APickableWeapon::APickableWeapon()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
}

void APickableWeapon::Interact(AXYZBaseCharacter* BaseCharacter)
{
	FWeaponTableRow* WeaponRow = XYZDataTableUtils::FindWeaponData(DataTableID);
	if (WeaponRow)
	{
		BaseCharacter->AddEquipmentItem(WeaponRow->EquipableActor);
		Destroy();
	}
}

FName APickableWeapon::GetActionEventName() const
{
	return ActionInteract;
}
