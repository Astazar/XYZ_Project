// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributesWidget.h"

void UCharacterAttributesWidget::UpdateHealth(float NewHealth, float MaxHealth)
{
	Health = bRoundHealthValue ? FMath::RoundToFloat(NewHealth) : NewHealth;
	HealthPercent = Health / MaxHealth;
	bIsHealthRestored = Health == MaxHealth ? true : false;
}

void UCharacterAttributesWidget::UpdateStamina(float NewStamina, float MaxStamina)
{
	Stamina = bRoundStaminaValue ? FMath::RoundToFloat(NewStamina) : NewStamina;
	StaminaPercent = Stamina / MaxStamina;
	bIsStaminaRestored = Stamina == MaxStamina ? true : false;
}

void UCharacterAttributesWidget::UpdateOxygen(float NewOxygen, float MaxOxygen)
{
	Oxygen = bRoundOxygenValue ? FMath::RoundToFloat(NewOxygen) : NewOxygen;
	OxygenPercent = Oxygen / MaxOxygen;
	bIsOxygenRestored = Oxygen == MaxOxygen ? true : false;
}
