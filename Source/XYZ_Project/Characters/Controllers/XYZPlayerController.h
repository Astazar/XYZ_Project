// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "XYZPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class XYZ_PROJECT_API AXYZPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void SetPawn(APawn* InPawn) override;
	
	bool GetIgnoreCameraPitch() const { return bIgnoreCameraPitch; }
	void SetIgnoreCameraPitch(bool bIgnoreCameraPitch_In) { bIgnoreCameraPitch = bIgnoreCameraPitch_In; }

protected:
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UUserWidget> MainMenuWidgetClass;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void Jump();
	void ChangeCrouchState();
	void ChangeCrawlState();

	void Slide();

	void StartSprint();
	void StopSprint();

	void SwimForward(float Value);
	void SwimRight(float Value);
	void SwimUp(float Value);
	void SwimDive();

	void Mantle();

	void ClimbLadderUp(float Value);
	void InteractWithLadder();

	void InteractWithZipline();
	void ZiplineClimbForward(float Value);
	void ZiplineTurnAround();

	void Wallrun();

	void PlayerStartFire();
	void PlayerStopFire();

	void StartAiming();
	void StopAiming();

	void Reload();

	void EquipSideArm();
	void EquipPrimaryWeapon();
	void EquipSecondaryWeapon();
	void EquipPrimaryItem();
	void EquipMeleeWeapon();
	void UnequipCurrentItem();

	void NextItem();
	void PreviousItem();

	void NextWeaponBarell();

	void EquipAndThrowPrimaryItem();
	void ThrowPrimaryItem();

	void PrimaryMeleeAttack();
	void SecondaryMeleeAttack();

	void Interact();

	void CreateAndInitializeWidgets();

	void ToggleMainMenu();

	void OnInteractableObjectFound(FName ActionName);

	TSoftObjectPtr<class AXYZBaseCharacter> CachedBaseCharacter;

	UPlayerHUDWidget* PlayerHUDWidget = nullptr;
	UUserWidget* MainMenuWidget = nullptr;

	bool bIgnoreCameraPitch = false;
};
