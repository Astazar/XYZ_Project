#include "XYZDataTableUtils.h"
#include <Engine/DataTable.h>


FWeaponTableRow* XYZDataTableUtils::FindWeaponData(FName WeaponID)
{
	static const FString ContextString(TEXT("Find Weapon Data"));
	UDataTable* WeaponDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/XYZ_Project/Core/Data/DataTables/DT_WeaponList.DT_WeaponList"));
	return WeaponDataTable->FindRow<FWeaponTableRow>(WeaponID, ContextString);
}
