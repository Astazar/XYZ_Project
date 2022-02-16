// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterAttributesWidget.generated.h"

/**
 * 
 */
UCLASS()
class XYZ_PROJECT_API UCharacterAttributesWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION()
	void UpdateHealth(float NewHealth, float MaxHealth);

	UFUNCTION()
	void UpdateStamina(float NewStamina, float MaxStamina);

	UFUNCTION()
	void UpdateOxygen(float NewOxygen, float MaxOxygen);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes | Health")
	float Health;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes | Health")
	float HealthPercent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes | Health")
	bool bIsHealthRestored = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes | Health")
	bool bRoundHealthValue = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes | Health", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float HealthWidgetFadeOutTime = 2.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes | Stamina")
	float Stamina;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes | Stamina")
	float StaminaPercent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes | Stamina")
	bool bIsStaminaRestored = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes | Stamina")
	bool bRoundStaminaValue = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes | Stamina", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float StaminaWidgetFadeOutTime = 2.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes | Oxygen")
	float Oxygen;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes | Oxygen")
	float OxygenPercent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes | Oxygen")
	bool bIsOxygenRestored = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes | Oxygen")
	bool bRoundOxygenValue = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes | Oxygen", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float OxygenWidgetFadeOutTime = 2.0f;
};
