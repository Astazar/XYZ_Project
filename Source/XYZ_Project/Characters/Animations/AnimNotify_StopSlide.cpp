// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_StopSlide.h"
#include "XYZ_Project/Characters/XYZBaseCharacter.h"
#include "XYZ_Project/Components/MovementComponents/XYZBaseMovementComponent.h"


void UAnimNotify_StopSlide::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	AXYZBaseCharacter* CharacterOwner = Cast<AXYZBaseCharacter>(MeshComp->GetOwner());
	
	if (IsValid(CharacterOwner))
	{
		CharacterOwner->GetCharacterMovementComponent()->StopSlide();
	}	
}
     