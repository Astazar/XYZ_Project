#include "EquipableItem.h"
#include "Characters/XYZBaseCharacter.h"

void AEquipableItem::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	if (IsValid(NewOwner))
	{
		checkf(GetOwner()->IsA<AXYZBaseCharacter>(), TEXT("AEquipableItem::SetOwner() only AXYZBaseCharacter can be an owner of AEquipableItem."));
		CachedCharacterOwner = StaticCast<AXYZBaseCharacter*>(GetOwner());
	}
	else
	{
		CachedCharacterOwner = nullptr;
	}
}

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

void AEquipableItem::Equip()
{
	if (OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(true);
	}
}

void AEquipableItem::Unequip()
{
	if (OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(false);
	}
}

EReticleType AEquipableItem::GetReticleType() const
{
	return ReticleType;
}

AXYZBaseCharacter* AEquipableItem::GetCharacterOwner() const
{
	return CachedCharacterOwner.Get();
}
