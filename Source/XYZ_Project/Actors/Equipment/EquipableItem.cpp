#include "EquipableItem.h"

EEquipableItemType AEquipableItem::GetItemType() const
{
	return ItemType;
}

UAnimMontage* AEquipableItem::GetCharacterEquipAnimMontage() const
{
	return CharacterEquipAnimMontage;
}

FName AEquipableItem::GetUnequippedSocketName() const
{
	return UnequippedSocketName;
}

FName AEquipableItem::GetEquippedSocketName() const
{
	return EquippedSocketName;
}
