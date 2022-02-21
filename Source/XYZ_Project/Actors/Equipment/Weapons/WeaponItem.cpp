#include "WeaponItem.h"

void AWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	SetAmmo(MaxAmmo);
}

bool AWeaponItem::CanUse() const
{
	return Ammo > 0;
}

EAmunitionType AWeaponItem::GetAmmoType() const
{
	return AmmoType;
}

int32 AWeaponItem::GetAmmo() const
{
	return Ammo;
}

void AWeaponItem::SetAmmo(int32 NewAmmo)
{
	Ammo = NewAmmo;
}

int32 AWeaponItem::GetMaxAmmo() const
{
	return MaxAmmo;
}
