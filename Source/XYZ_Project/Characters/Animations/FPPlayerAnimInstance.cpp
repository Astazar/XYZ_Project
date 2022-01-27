// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerAnimInstance.h"
#include "XYZ_Project/Characters/FPPlayerCharacter.h"
#include "XYZ_Project/Characters/Controllers/XYZPlayerController.h"

void UFPPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AFPPlayerCharacter>(), TEXT("UFPPlayerAnimInstance::NativeBeginPlay() Only AFPPlayerCharacter can work with UFPPlayerAnimInstance::NativeBeginPlay()"));
	CachedFPCharacterOwner = StaticCast<AFPPlayerCharacter*>(GetOwningActor());
}

void UFPPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CachedFPCharacterOwner.IsValid())
	{
		return;
	}

	PlayerCameraPitchAngle = CalculateCameraPitchAngle();
}

float UFPPlayerAnimInstance::CalculateCameraPitchAngle() const
{
	float Result = 0.0f;
	AXYZPlayerController* Controller = CachedFPCharacterOwner->GetController<AXYZPlayerController>();
	if (IsValid(Controller) && !Controller->GetIgnoreCameraPitch())
	{
		Result = Controller->GetControlRotation().Pitch;
	}
	return Result;
}
