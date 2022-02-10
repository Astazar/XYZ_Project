// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"
#include "Characters/XYZBaseCharacter.h"
#include "Components/CharacterComponents/CharacterAttributesComponent.h"

float UPlayerHUDWidget::GetHealthPercent() const
{
	float Result = 1.0f;
	APawn* Pawn = GetOwningPlayerPawn();
	AXYZBaseCharacter* Character = Cast<AXYZBaseCharacter>(Pawn);
	if (IsValid(Character))
	{
		Result = Character->GetCharacterAttributesComponent()->GetCurrentHealthPercent();
	}
	return Result;
}
