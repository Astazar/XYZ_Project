// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZBaseCharacterAnimInstance.h"
#include "Characters/XYZBaseCharacter.h"
#include "Components/CharacterComponents/CharacterAttributesComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Components/MovementComponents/XYZBaseMovementComponent.h"
#include <Actors/Equipment/Weapons/RangeWeaponItem.h>




void UXYZBaseCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AXYZBaseCharacter>(), TEXT("void UXYZBaseCharacterAnimInstance::NativeBeginPlay() UXYZBaseCharacterAnimInstance can be used only with AXYZBaseCharacter"));
	CachedBaseCharacter = StaticCast<AXYZBaseCharacter*>(TryGetPawnOwner());
}

void UXYZBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CachedBaseCharacter.IsValid())
	{
		return;
	}
	UXYZBaseMovementComponent* CharacterMovement = CachedBaseCharacter->GetCharacterMovementComponent();
	Speed = CharacterMovement->Velocity.Size();
	Stamina = CachedBaseCharacter->GetCharacterAttributesComponent()->GetCurrentStamina();
	bIsOutOfStamina = CharacterMovement->GetIsOutOfStamina();
	bIsFalling = CharacterMovement->IsFalling();
	bIsCrouching = CharacterMovement->IsCrouching();
	bIsSprinting = CharacterMovement->IsSprinting();
	bIsCrawling = CharacterMovement->IsCrawling();
	bIsSwimming = CharacterMovement->IsSwimming();
	bIsMantling = CharacterMovement->IsMantling();
	bIsOnLadder = CharacterMovement->IsOnLadder();
	bIsZiplining = CharacterMovement->IsZiplining();
	bIsWallrunning = CharacterMovement->IsWallrunning();
	bIsAiming = CachedBaseCharacter->IsAiming();
	LastInputSize = CharacterMovement->GetLastInputVector().Size();
	if (bIsWallrunning)
	{
		CurrentWallrunSide = CharacterMovement->GetCurrentWallrunSide();
	}
	if(bIsOnLadder)
	{ 
		LadderSpeedRatio = CharacterMovement->GetLadderSpeedRatio();
	}

	bIsStrafing = !CharacterMovement->bOrientRotationToMovement;
	Direction = CalculateDirection(CharacterMovement->Velocity, CachedBaseCharacter->GetActorRotation());

	RightFootEffectorLocation = FVector(CachedBaseCharacter->GetIKRightFootOffset() + CachedBaseCharacter->GetIKPelvisOffset(), 0.0f, 0.0f);
	LeftFootEffectorLocation = FVector(-(CachedBaseCharacter->GetIKLeftFootOffset() + CachedBaseCharacter->GetIKPelvisOffset()), 0.0f, 0.0f);
	PelvisOffsetLocation = FVector(0.0f, 0.0f, CachedBaseCharacter->GetIKPelvisOffset());

	AimRotation = CachedBaseCharacter->GetAimOffsetLocal();

	const UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent();
	CurrentEquippedItemType = CharacterEquipment->GetCurrentEquippedItemType();

	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipment->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		ForeGripSocketTransform = CurrentRangeWeapon->GetForeGripTransform();
	}
}
