// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZPlayerController.h"
#include "XYZ_Project/Characters/XYZBaseCharacter.h"
#include "UI/Widgets/PlayerHUDWidget.h"
#include "UI/Widgets/ReticleWidget.h"
#include "UI/Widgets/AmmoWidget.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Components/CharacterComponents/CharacterAttributesComponent.h"
#include "UI/Widgets/CharacterAttributesWidget.h"
#include "XYZ_ProjectTypes.h"
#include <GameFramework/PlayerInput.h>

void AXYZPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CachedBaseCharacter = Cast<AXYZBaseCharacter>(InPawn);
	if (CachedBaseCharacter.IsValid() && IsLocalController())
	{
		CreateAndInitializeWidgets();
		CachedBaseCharacter->OnInteractableObjectFound.BindUObject(this, &AXYZPlayerController::OnInteractableObjectFound);
	}
}

void AXYZPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &AXYZPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AXYZPlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &AXYZPlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &AXYZPlayerController::LookUp);
	InputComponent->BindAxis("SwimForward", this, &AXYZPlayerController::SwimForward);
	InputComponent->BindAxis("SwimRight", this, &AXYZPlayerController::SwimRight);
	InputComponent->BindAxis("SwimUp", this, &AXYZPlayerController::SwimUp);
	InputComponent->BindAxis("ClimbLadderUp", this, &AXYZPlayerController::ClimbLadderUp);
	InputComponent->BindAxis("ZiplineClimbForward", this, &AXYZPlayerController::ZiplineClimbForward);
	InputComponent->BindAction("InteractWithZipline", EInputEvent::IE_Pressed, this, &AXYZPlayerController::InteractWithZipline);
	InputComponent->BindAction("ZiplineTurnAround", EInputEvent::IE_Pressed, this, &AXYZPlayerController::ZiplineTurnAround);
	InputComponent->BindAction("InteractWithLadder", EInputEvent::IE_Pressed, this, &AXYZPlayerController::InteractWithLadder);
	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &AXYZPlayerController::Mantle);
	InputComponent->BindAction("Wallrun", EInputEvent::IE_Pressed, this, &AXYZPlayerController::Wallrun);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AXYZPlayerController::Jump);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AXYZPlayerController::ChangeCrouchState);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AXYZPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &AXYZPlayerController::StopSprint);
	InputComponent->BindAction("Crawl", EInputEvent::IE_Pressed, this, &AXYZPlayerController::ChangeCrawlState);
	InputComponent->BindAction("Slide", EInputEvent::IE_Pressed, this, &AXYZPlayerController::Slide);
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &AXYZPlayerController::PlayerStartFire);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &AXYZPlayerController::PlayerStopFire);
	InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &AXYZPlayerController::StartAiming);
	InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &AXYZPlayerController::StopAiming);
	InputComponent->BindAction("SwimDive", EInputEvent::IE_Pressed, this, &AXYZPlayerController::SwimDive);
	InputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &AXYZPlayerController::Reload);
	InputComponent->BindAction("NextItem", EInputEvent::IE_Pressed, this, &AXYZPlayerController::NextItem);
	InputComponent->BindAction("PreviousItem", EInputEvent::IE_Pressed, this, &AXYZPlayerController::PreviousItem);
	InputComponent->BindAction("EquipAndThrowPrimaryItem", EInputEvent::IE_Pressed, this, &AXYZPlayerController::EquipAndThrowPrimaryItem);
	InputComponent->BindAction("ThrowPrimaryItem", EInputEvent::IE_Pressed, this, &AXYZPlayerController::ThrowPrimaryItem);
	InputComponent->BindAction("PrimaryMeleeAttack", EInputEvent::IE_Pressed, this, &AXYZPlayerController::PrimaryMeleeAttack);
	InputComponent->BindAction("SecondaryMeleeAttack", EInputEvent::IE_Pressed, this, &AXYZPlayerController::SecondaryMeleeAttack);
	InputComponent->BindAction("NextWeaponBarell", EInputEvent::IE_Pressed, this, &AXYZPlayerController::NextWeaponBarell);
	FInputActionBinding& ToggleMainMenuBinding = InputComponent->BindAction("ToggleMainMenu", EInputEvent::IE_Pressed, this, &AXYZPlayerController::ToggleMainMenu);
	ToggleMainMenuBinding.bExecuteWhenPaused = true;
	InputComponent->BindAction(ActionInteract, EInputEvent::IE_Pressed, this, &AXYZPlayerController::Interact);
	//Equip Actions
	InputComponent->BindAction("EquipSideArm", EInputEvent::IE_Pressed, this, &AXYZPlayerController::EquipSideArm);
	InputComponent->BindAction("EquipPrimaryWeapon", EInputEvent::IE_Pressed, this, &AXYZPlayerController::EquipPrimaryWeapon);
	InputComponent->BindAction("EquipSecondaryWeapon", EInputEvent::IE_Pressed, this, &AXYZPlayerController::EquipSecondaryWeapon);
	InputComponent->BindAction("EquipPrimaryItem", EInputEvent::IE_Pressed, this, &AXYZPlayerController::EquipPrimaryItem);
	InputComponent->BindAction("EquipMeleeWeapon", EInputEvent::IE_Pressed, this, &AXYZPlayerController::EquipMeleeWeapon);
	InputComponent->BindAction("UnequipCurrentItem", EInputEvent::IE_Pressed, this, &AXYZPlayerController::UnequipCurrentItem);
}

void AXYZPlayerController::MoveForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveForward(Value);
	}
}

void AXYZPlayerController::MoveRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveRight(Value);
	}
}

void AXYZPlayerController::Turn(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Turn(Value);
	}
}

void AXYZPlayerController::LookUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUp(Value);
	}
}

void AXYZPlayerController::Jump()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Jump();
	}
}

void AXYZPlayerController::ChangeCrouchState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrouchState();
	}
}

void AXYZPlayerController::ChangeCrawlState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrawlState();
	}
}

void AXYZPlayerController::Slide()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Slide();
	}
}

void AXYZPlayerController::StartSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartSprint();
	}
}

void AXYZPlayerController::StopSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopSprint();
	}
}

void AXYZPlayerController::SwimForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimForward(Value);
	}
}

void AXYZPlayerController::SwimRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimRight(Value);
	}
}

void AXYZPlayerController::SwimUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimUp(Value);
	}
}

void AXYZPlayerController::SwimDive()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimDive();
	}
}

void AXYZPlayerController::Mantle()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Mantle();
	}
}

void AXYZPlayerController::ClimbLadderUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ClimbLadderUp(Value);
	}
}

void AXYZPlayerController::InteractWithLadder()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithLadder();
	}
}

void AXYZPlayerController::InteractWithZipline()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithZipline();
	}
}

void AXYZPlayerController::ZiplineClimbForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ZiplineClimbForward(Value);
	}
}

void AXYZPlayerController::ZiplineTurnAround()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ZiplineTurnAround();
	}
}

void AXYZPlayerController::Wallrun()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Wallrun();
	}
}

void AXYZPlayerController::PlayerStartFire()
{
	UE_LOG(LogTemp, Warning, TEXT("AXYZPlayerController::PlayerStartFire"));
	if (CachedBaseCharacter.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("AXYZPlayerController::PlayerStartFire is valid"));
		CachedBaseCharacter->StartFire();
	}
}

void AXYZPlayerController::PlayerStopFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopFire();
	}
}

void AXYZPlayerController::StartAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartAiming();
	}
}

void AXYZPlayerController::StopAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopAiming();
	}
}

void AXYZPlayerController::Reload()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Reload();
	}
}

void AXYZPlayerController::EquipSideArm()
{
	if (CachedBaseCharacter.IsValid())
	{
		UCharacterEquipmentComponent* EquipmentComponent = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
		if (IsValid(EquipmentComponent))
		{
			EquipmentComponent->EquipItemInSlot(EEquipmentSlots::SideArm);
		}
	}
}

void AXYZPlayerController::EquipPrimaryWeapon()
{
	if (CachedBaseCharacter.IsValid())
	{
		UCharacterEquipmentComponent* EquipmentComponent = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
		if (IsValid(EquipmentComponent))
		{
			EquipmentComponent->EquipItemInSlot(EEquipmentSlots::PrimaryWeapon);
		}
	}
}

void AXYZPlayerController::EquipSecondaryWeapon()
{
	if (CachedBaseCharacter.IsValid())
	{
		UCharacterEquipmentComponent* EquipmentComponent = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
		if (IsValid(EquipmentComponent))
		{
			EquipmentComponent->EquipItemInSlot(EEquipmentSlots::SecondaryWeapon);
		}
	}
}

void AXYZPlayerController::EquipPrimaryItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		UCharacterEquipmentComponent* EquipmentComponent = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
		if (IsValid(EquipmentComponent))
		{
			EquipmentComponent->EquipItemInSlot(EEquipmentSlots::PrimaryItemSlot);
		}
	}
}

void AXYZPlayerController::EquipMeleeWeapon()
{
	if (CachedBaseCharacter.IsValid())
	{
		UCharacterEquipmentComponent* EquipmentComponent = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
		if (IsValid(EquipmentComponent))
		{
			EquipmentComponent->EquipItemInSlot(EEquipmentSlots::MeleeWeapon);
		}
	}
}

void AXYZPlayerController::UnequipCurrentItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		UCharacterEquipmentComponent* EquipmentComponent = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
		if (IsValid(EquipmentComponent))
		{
			EquipmentComponent->EquipItemInSlot(EEquipmentSlots::None);
		}
	}
}

void AXYZPlayerController::NextItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->NextItem();
	}
}

void AXYZPlayerController::PreviousItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PreviousItem();
	}
}

void AXYZPlayerController::NextWeaponBarell()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->NextWeaponBarell();
	}
}

void AXYZPlayerController::EquipAndThrowPrimaryItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ThrowPrimaryItem(true);
	}
}

void AXYZPlayerController::ThrowPrimaryItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ThrowPrimaryItem(false);
	}
}

void AXYZPlayerController::PrimaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PrimaryMeleeAttack();
	}
}

void AXYZPlayerController::SecondaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SecondaryMeleeAttack();
	}
}

void AXYZPlayerController::Interact()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Interact();
	}
}

void AXYZPlayerController::CreateAndInitializeWidgets()
{
	if (!IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
		if (IsValid(PlayerHUDWidget))
		{
			PlayerHUDWidget->AddToViewport();
		}
	}

	if (!IsValid(MainMenuWidget))
	{
		MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
	}
	
	if (IsValid(PlayerHUDWidget) && CachedBaseCharacter.IsValid())
	{
		UReticleWidget* ReticleWidget = PlayerHUDWidget->GetReticleWidget();
		if (IsValid(ReticleWidget))
		{
			CachedBaseCharacter->OnAmimingStateChanged.AddUFunction(ReticleWidget, FName("OnAimingStateChanged"));
			CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable()->OnEquippedItemChanged.AddUFunction(ReticleWidget, FName("OnEquippedItemChanged"));
		}

		UAmmoWidget* AmmoWidget = PlayerHUDWidget->GetAmmoWidget();
		if (IsValid(AmmoWidget))
		{
			CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable()->OnCurrentWeaponAmmoChangedEvent.AddUObject(AmmoWidget, &UAmmoWidget::UpdateAmmoCount);
			CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable()->OnCurrentThrowItemAmmoChangedEvent.AddUObject(AmmoWidget, &UAmmoWidget::UpdateThrowAmmoCount);
		}

		UCharacterAttributesWidget* AttributesWidget = PlayerHUDWidget->GetCharacterAttributesWidget();
		if (IsValid(AttributesWidget))
		{
			UCharacterAttributesComponent* AttributesComponent = CachedBaseCharacter->GetCharacterAttributesComponent();
			AttributesComponent->OnCurrentHealthChangedEvent.AddUFunction(AttributesWidget, FName("UpdateHealth"));
			AttributesComponent->OnCurrentStaminaChangedEvent.AddUFunction(AttributesWidget, FName("UpdateStamina"));
			AttributesComponent->OnCurrentOxygenChangedEvent.AddUFunction(AttributesWidget, FName("UpdateOxygen"));
		}
	}
	SetInputMode(FInputModeGameOnly{});
	bShowMouseCursor = false;
}

void AXYZPlayerController::ToggleMainMenu()
{
	if (!IsValid(MainMenuWidget) || !IsValid(PlayerHUDWidget))
	{
		return;
	}

	if (MainMenuWidget->IsVisible())
	{
		MainMenuWidget->RemoveFromParent();
		PlayerHUDWidget->AddToViewport();
		SetInputMode(FInputModeGameOnly {});
		SetPause(false);
		bShowMouseCursor = false;
	}
	else
	{
		MainMenuWidget->AddToViewport();
		PlayerHUDWidget->RemoveFromParent();
		SetInputMode(FInputModeGameAndUI {});
		SetPause(true);
		bShowMouseCursor = true;
	}
}

void AXYZPlayerController::OnInteractableObjectFound(FName ActionName)
{
	if (!IsValid(PlayerHUDWidget))
	{
		return;
	}
	TArray<FInputActionKeyMapping> ActionKeys = PlayerInput->GetKeysForAction(ActionName);
	const bool HasAnyKeys = ActionKeys.Num() != 0;
	if (HasAnyKeys)
	{
		FName ActionKey = ActionKeys[0].Key.GetFName();
		PlayerHUDWidget->SetHighlightInteractableActionText(ActionKey);
	}
	PlayerHUDWidget->SetHighlightInteractableVisibility(HasAnyKeys);
}
