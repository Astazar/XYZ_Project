#include "CharacterEquipmentComponent.h"
#include "Components/Weapon/WeaponBarellComponent.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Actors/Equipment/Weapons/MeleeWeaponItem.h"
#include "Actors/Equipment/Throwables/ThrowableItem.h"
#include "Characters/XYZBaseCharacter.h"
#include "XYZ_ProjectTypes.h"
#include <Net/UnrealNetwork.h>



UCharacterEquipmentComponent::UCharacterEquipmentComponent()
{
	SetIsReplicatedByDefault(true);
}

void UCharacterEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCharacterEquipmentComponent, CurrentEquippedSlot);
	DOREPLIFETIME(UCharacterEquipmentComponent, AmunitionArray);
	DOREPLIFETIME(UCharacterEquipmentComponent, ItemsArray);
}

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<AXYZBaseCharacter>(), TEXT("UCharacterEquipmentComponent::BeginPlay() can work only with AXYZBaseCharacter"));
	CachedBaseCharacter = StaticCast<AXYZBaseCharacter*>(GetOwner());
	OnEquippedItemChanged.AddUObject(this, &UCharacterEquipmentComponent::SelectMovementSettings);
	CreateLoadout();
	AutoEquip();
}

void UCharacterEquipmentComponent::SelectMovementSettings(const AEquipableItem* EquippedItem)
{
	CachedBaseCharacter->SelectMovementSettings(GetCurrentEquippedItemType() == EEquipableItemType::None);
}

void UCharacterEquipmentComponent::ReloadCurrentWeapon()
{
	checkf(IsValid(CurrentEquippedWeapon), TEXT("UCharacterEquipmentComponent::ReloadCurrentWeapon() CurrentEquippedWeapon is not valid"));
	int32 AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
	if(AvailableAmunition <= 0)
	{
		return; 	
	}

	CurrentEquippedWeapon->Server_StartReload();
}

void UCharacterEquipmentComponent::ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo /*= 0*/, bool bCheckIsFull /*= false*/)
{
	int32 AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
	int32 CurrentAmmo = CurrentEquippedWeapon->GetCurrentBarellComponent()->GetAmmo();
	int32 AmmoToReload = CurrentEquippedWeapon->GetCurrentBarellComponent()->GetMaxAmmo() - CurrentAmmo;
	int32 ReloadedAmmo = FMath::Min(AvailableAmunition, AmmoToReload);

	if (NumberOfAmmo > 0)
	{
		ReloadedAmmo = FMath::Min(ReloadedAmmo, NumberOfAmmo);
	}

	AmunitionArray[(uint32)CurrentEquippedWeapon->GetCurrentBarellComponent()->GetAmmoType()] -= ReloadedAmmo;
	CurrentEquippedWeapon->GetCurrentBarellComponent()->SetAmmo(CurrentAmmo + ReloadedAmmo);

	if (bCheckIsFull)
	{
		AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
		bool bIsFullyReloaded = CurrentEquippedWeapon->GetCurrentBarellComponent()->GetAmmo() == CurrentEquippedWeapon->GetCurrentBarellComponent()->GetMaxAmmo();
		if (AvailableAmunition == 0 || bIsFullyReloaded)
		{
			CurrentEquippedWeapon->EndReload(true);
		}
	}
}

bool UCharacterEquipmentComponent::IsEquipping() const
{
	return bIsEquiping;
}

void UCharacterEquipmentComponent::SetShouldEquipPrevious(bool bEquipPrevious)
{
	bShouldEquipPrevious = bEquipPrevious;
}

void UCharacterEquipmentComponent::EquipItemInSlot(EEquipmentSlots Slot)
{
	if (bIsEquiping)
	{
		return;
	}

	UnequipCurrentItem();
	CurrentEquippedItem = ItemsArray[(uint32)Slot];
	CurrentEquippedWeapon = Cast<ARangeWeaponItem>(CurrentEquippedItem);
	CurrentThrowableItem = Cast<AThrowableItem>(CurrentEquippedItem);
	CurrentMeleeWeapon = Cast<AMeleeWeaponItem>(CurrentEquippedItem);

	if (IsValid(CurrentEquippedWeapon))
	{
		OnCurrentWeaponBarellAmmoChangedHandle = CurrentEquippedWeapon->GetCurrentBarellComponent()->OnAmmoChanged.AddUFunction(this, FName("OnCurrentWeaponAmmoChanged"));
		OnCurrentWeaponReloadedHandle = CurrentEquippedWeapon->OnReloadComplete.AddUFunction(this, FName("OnWeaponReloadComplete"));
		OnCurrentWeaponAmmoChanged(CurrentEquippedWeapon->GetCurrentBarellComponent()->GetAmmo());
	}

	if (IsValid(CurrentThrowableItem))
	{
		CurrentThrowableItem->OnThrowAmmoChanged.AddUFunction(this, FName("OnCurrentThrowItemAmmoChanged"));
		OnCurrentThrowItemAmmoChanged(CurrentThrowableItem->GetThrowAmmo());
	}

	if (IsValid(CurrentEquippedItem))
	{
		UAnimMontage* EquipMontage = CurrentEquippedItem->GetCharacterEquipAnimMontage();
		if (IsValid(EquipMontage))
		{
			bIsEquiping = true;
			float EquipDuration = CachedBaseCharacter->PlayAnimMontage(EquipMontage) / EquipMontage->RateScale;
			GetWorld()->GetTimerManager().SetTimer(EquipTimer, this, &UCharacterEquipmentComponent::EquipAnimationFinished, EquipDuration, false);
		}
		else
		{
			//It calls OnEquipAnimationFinished delegate, which needs to start throwing item, when it's not equipped. 
			EquipAnimationFinished();
			AttachCurrentItemToEquippedSocket();
		}
		CurrentEquippedSlot = Slot;
		CurrentEquippedItem->Equip();
	}

	if (OnEquippedItemChanged.IsBound())
	{
		OnEquippedItemChanged.Broadcast(CurrentEquippedItem);
	}

	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_EquipItemInSlot(CurrentEquippedSlot);
	}
}

void UCharacterEquipmentComponent::AttachCurrentItemToEquippedSocket()
{
	if (IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetEquippedSocketName());
	}
}

void UCharacterEquipmentComponent::UnequipCurrentItem()
{
	if (IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetUnequippedSocketName());
		CurrentEquippedItem->Unequip();
	}

	if (IsValid(CurrentEquippedWeapon))
	{
		CurrentEquippedWeapon->StopFire();
		CurrentEquippedWeapon->EndReload(false);
		CurrentEquippedWeapon->GetCurrentBarellComponent()->OnAmmoChanged.Remove(OnCurrentWeaponBarellAmmoChangedHandle);
		CurrentEquippedWeapon->OnReloadComplete.Remove(OnCurrentWeaponReloadedHandle);
	}
	PreviousEquippedSlot = CurrentEquippedSlot;
	CurrentEquippedSlot = EEquipmentSlots::None;
}

void UCharacterEquipmentComponent::EquipAnimationFinished()
{
	bIsEquiping = false;
	AttachCurrentItemToEquippedSocket();
	OnEquipAnimationFinished.ExecuteIfBound();
}

void UCharacterEquipmentComponent::EquipNextItem()
{
	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 NextSlotIndex = NextItemsArraySlotIndex(CurrentSlotIndex);
	while ((CurrentSlotIndex != NextSlotIndex && IgnoreSlotsWhileSwitching.Contains((EEquipmentSlots)NextSlotIndex)) || !IsValid(ItemsArray[NextSlotIndex]))
	{
		NextSlotIndex = NextItemsArraySlotIndex(NextSlotIndex);
	}
	if (CurrentSlotIndex != NextSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)NextSlotIndex);
	}
}

void UCharacterEquipmentComponent::EquipPreviousItem()
{
	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 PreviousSlotIndex = PreviousItemsArraySlotIndex(CurrentSlotIndex);
	bool IsSlotValid = IsValid(ItemsArray[PreviousSlotIndex]);
	while ((CurrentSlotIndex != PreviousSlotIndex && IgnoreSlotsWhileSwitching.Contains((EEquipmentSlots)PreviousSlotIndex)) || !IsValid(ItemsArray[PreviousSlotIndex]))
	{
		PreviousSlotIndex = PreviousItemsArraySlotIndex(PreviousSlotIndex);
	}
	if (CurrentSlotIndex != PreviousSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)PreviousSlotIndex);
	}
}

void UCharacterEquipmentComponent::NextWeaponBarell()
{
	if (!IsValid(CurrentEquippedWeapon))
	{
		return;
	}
	UWeaponBarellComponent* CurrentBarell = CurrentEquippedWeapon->GetCurrentBarellComponent();
	CurrentBarell->OnAmmoChanged.Remove(OnCurrentWeaponBarellAmmoChangedHandle);
	if (IsValid(CurrentEquippedWeapon))
	{
		CurrentEquippedWeapon->NextWeaponBarell();
		UWeaponBarellComponent* NewBarell = CurrentEquippedWeapon->GetCurrentBarellComponent();
		OnCurrentWeaponBarellAmmoChangedHandle = NewBarell->OnAmmoChanged.AddUFunction(this, FName("OnCurrentWeaponAmmoChanged"));
		OnCurrentWeaponAmmoChanged(NewBarell->GetAmmo());
	}
}

void UCharacterEquipmentComponent::LaunchCurrentThrowableItem()
{
	if (IsValid(CurrentThrowableItem))
	{
		CurrentThrowableItem->LaunchItemProjectile();
		if(bShouldEquipPrevious)
		{
			EquipItemInSlot(PreviousEquippedSlot);
			bShouldEquipPrevious = false;
		}
	}
}

void UCharacterEquipmentComponent::AddEquipmentItem(const TSubclassOf<AEquipableItem> EquipableItemClass)
{
	ARangeWeaponItem* Weapon = Cast<ARangeWeaponItem>(EquipableItemClass->GetDefaultObject());
	if (!IsValid(Weapon))
	{
		return;
	}

	//Adding a clip 
	UWeaponBarellComponent* WeaponBarell = Weapon->GetCurrentBarellComponent();
	int32 WeaponAmmoIndex = (uint32)WeaponBarell->GetAmmoType();
	int32 WeaponMaxAmmo = *(MaxAmunitionAmount.Find(WeaponBarell->GetAmmoType()));
	int32 WeaponClip = WeaponBarell->GetMaxAmmo();
	int32 Ammo = FMath::Min(AmunitionArray[WeaponAmmoIndex] + WeaponClip, WeaponMaxAmmo);
	AmunitionArray[WeaponAmmoIndex] = Ammo;
	
	if (!IsValid(CurrentEquippedWeapon))
	{
		return;
	}
	UWeaponBarellComponent* CurrentWeaponBarell = CurrentEquippedWeapon->GetCurrentBarellComponent();
	bool bIsCurrentAmmoChanged = WeaponBarell->GetAmmoType() == CurrentWeaponBarell->GetAmmoType();
	if (IsValid(CurrentEquippedWeapon) && bIsCurrentAmmoChanged)
	{
		OnCurrentWeaponAmmoChanged(CurrentWeaponBarell->GetAmmo());
	}
}

AEquipableItem* UCharacterEquipmentComponent::GetCurrentEquippedItem() const
{
	return CurrentEquippedItem;
}

EEquipableItemType UCharacterEquipmentComponent::GetCurrentEquippedItemType() const
{
	EEquipableItemType Result = EEquipableItemType::None;
	if (IsValid(CurrentEquippedItem))
	{
		Result = CurrentEquippedItem->GetItemType();
	}
	return Result;
}

ARangeWeaponItem* UCharacterEquipmentComponent::GetCurrentRangeWeapon() const
{
	return CurrentEquippedWeapon;
}


AThrowableItem* UCharacterEquipmentComponent::GetCurrentThrowableItem() const
{
	return CurrentThrowableItem;
}

AMeleeWeaponItem* UCharacterEquipmentComponent::GetCurrentMeleeWeapon() const
{
	return CurrentMeleeWeapon;
}


EEquipmentSlots UCharacterEquipmentComponent::GetCurrentEquippedSlot() const
{
	return CurrentEquippedSlot;
}

AEquipableItem* UCharacterEquipmentComponent::GetItemInSlot(EEquipmentSlots Slot) const
{
	return ItemsArray[uint32(Slot)];
}

void UCharacterEquipmentComponent::Server_EquipItemInSlot_Implementation(EEquipmentSlots Slot)
{
	EquipItemInSlot(Slot);
}

void UCharacterEquipmentComponent::CreateLoadout()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	AmunitionArray.AddZeroed((uint32)EAmunitionType::MAX);
	for (const TPair<EAmunitionType, int32>& AmmoPair : MaxAmunitionAmount)
	{
		AmunitionArray[(uint8)AmmoPair.Key] = FMath::Max(AmmoPair.Value, 0);
	}

	ItemsArray.AddZeroed((uint32)EEquipmentSlots::MAX);
	for (const TPair<EEquipmentSlots, TSubclassOf<AEquipableItem>>& ItemPair : ItemsLoadout)
	{
		if (!IsValid(ItemPair.Value))
		{
			continue;
		}
		AEquipableItem* Item = GetWorld()->SpawnActor<AEquipableItem>(ItemPair.Value);
		Item->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Item->GetUnequippedSocketName());
		Item->SetOwner(CachedBaseCharacter.Get());
		Item->Unequip();
		ItemsArray[(uint32)ItemPair.Key] = Item;
		Item->CreatePools();

		if (Item->GetItemType() == EEquipableItemType::Throwable)
		{
			AThrowableItem* ThrowItem = Cast<AThrowableItem>(Item);
			OnCurrentThrowItemAmmoChanged(ThrowItem->GetMaxThrowAmmo());
		}
	}
}

void UCharacterEquipmentComponent::AutoEquip()
{
	if (AutoEquippedSlot != EEquipmentSlots::None)
	{
		EquipItemInSlot(AutoEquippedSlot);
	}
}

uint32 UCharacterEquipmentComponent::NextItemsArraySlotIndex(uint32 CurrentSlotIndex)
{
	if (CurrentSlotIndex == ItemsArray.Num() - 1)
	{
		return 0;
	}
	else
	{
		return CurrentSlotIndex + 1;
	}
}

uint32 UCharacterEquipmentComponent::PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex)
{
	if (CurrentSlotIndex == 0)
	{
		return ItemsArray.Num() - 1;
	}
	else
	{
		return CurrentSlotIndex - 1;
	}
}

int32 UCharacterEquipmentComponent::GetAvailableAmunitionForCurrentWeapon()
{
	checkf(IsValid(CurrentEquippedWeapon), TEXT("UCharacterEquipmentComponent::GetAvailableAmunitionForCurrentWeapon() CurrentEquippedWeapon is not valid"));
	return AmunitionArray[(uint32)CurrentEquippedWeapon->GetCurrentBarellComponent()->GetAmmoType()];
}

void UCharacterEquipmentComponent::OnWeaponReloadComplete()
{
	ReloadAmmoInCurrentWeapon();
}

void UCharacterEquipmentComponent::OnCurrentWeaponAmmoChanged(int32 Ammo)
{
	if (OnCurrentWeaponAmmoChangedEvent.IsBound())
	{
		OnCurrentWeaponAmmoChangedEvent.Broadcast(Ammo, GetAvailableAmunitionForCurrentWeapon());
	}
}

void  UCharacterEquipmentComponent::OnCurrentThrowItemAmmoChanged(int32 Ammo)
{
	if (OnCurrentThrowItemAmmoChangedEvent.IsBound())
	{
		OnCurrentThrowItemAmmoChangedEvent.Broadcast(Ammo);
	}
}

void UCharacterEquipmentComponent::OnRep_ItemsArray()
{
	for (AEquipableItem* Item : ItemsArray)
	{
		if (IsValid(Item))
		{
			Item->Unequip();
		}
	}
}

void UCharacterEquipmentComponent::OnRep_CurrentEquippedSlot(EEquipmentSlots CurrentEquippedSlot_Old)
{
	EquipItemInSlot(CurrentEquippedSlot);
}
