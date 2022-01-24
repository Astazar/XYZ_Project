// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerAnimInstance.h"
#include "XYZ_Project/Characters/FPPlayerCharacter.h"

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

	APlayerController* Controller = CachedFPCharacterOwner->GetController<APlayerController>();
	if (IsValid(Controller))
	{
		PlayerCameraPitchAngle = Controller->GetControlRotation().Pitch;
	}
}
