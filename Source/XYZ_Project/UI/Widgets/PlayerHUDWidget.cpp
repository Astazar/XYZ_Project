// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"
#include "Characters/XYZBaseCharacter.h"
#include "Components/CharacterComponents/CharacterAttributesComponent.h"
#include "ReticleWidget.h"
#include <Blueprint/WidgetTree.h>
#include "AmmoWidget.h"
#include "CharacterAttributesWidget.h"

UReticleWidget* UPlayerHUDWidget::GetReticleWidget()
{
	return WidgetTree->FindWidget<UReticleWidget>(ReticleWidgetName);
}

class UAmmoWidget* UPlayerHUDWidget::GetAmmoWidget()
{
	return WidgetTree->FindWidget<UAmmoWidget>(AmmoWidgetName);
}

class UCharacterAttributesWidget* UPlayerHUDWidget::GetCharacterAttributesWidget()
{
	return WidgetTree->FindWidget<UCharacterAttributesWidget>(CharacterAttributesWidgetName);
}

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
