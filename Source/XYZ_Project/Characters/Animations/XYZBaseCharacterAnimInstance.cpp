// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZBaseCharacterAnimInstance.h"
#include "XYZ_Project/Characters/XYZBaseCharacter.h"
#include "XYZ_Project/Components/MovementComponents/XYZBaseMovementComponent.h"


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
	Stamina = CachedBaseCharacter->GetCurrentStamina();
	bIsOutOfStamina = CharacterMovement->GetIsOutOfStamina();
	bIsFalling = CharacterMovement->IsFalling();
	bIsCrouching = CharacterMovement->IsCrouching();
	bIsSprinting = CharacterMovement->IsSprinting();
	bIsCrawling = CharacterMovement->IsCrawling();
	bIsSwimming = CharacterMovement->IsSwimming();
	bIsMantling = CharacterMovement->IsMantling();

	RightFootEffectorLocation = FVector(CachedBaseCharacter->GetIKRightFootOffset() + CachedBaseCharacter->GetIKPelvisOffset(), 0.0f, 0.0f);
	LeftFootEffectorLocation = FVector(-(CachedBaseCharacter->GetIKLeftFootOffset() + CachedBaseCharacter->GetIKPelvisOffset()), 0.0f, 0.0f);
	PelvisOffsetLocation = FVector(0.0f, 0.0f, CachedBaseCharacter->GetIKPelvisOffset());
}
