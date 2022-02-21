// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <XYZ_ProjectTypes.h>
#include "ReticleWidget.generated.h"


class AEquipableItem;

UCLASS()
class XYZ_PROJECT_API UReticleWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnAimingStateChanged(bool bIsAiming);

	UFUNCTION(BlueprintNativeEvent)
	void OnEquippedItemChanged(const AEquipableItem* EquippedItem);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reticle")
	EReticleType CurrentReticle; 

private:
	void SetupCurrentReticle();

	TWeakObjectPtr<const AEquipableItem> CurrentEquippedItem;
};
