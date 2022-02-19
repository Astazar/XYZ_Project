// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoWidget.generated.h"

/**
 * 
 */
UCLASS()
class XYZ_PROJECT_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo | Weapon")
	int32 Ammo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo | Weapon")
	int32 TotalAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo | Throwable")
	int32 TotalThrowAmmo;

private:
	UFUNCTION()
	void UpdateAmmoCount(int32 NewAmmo, int32 NewTotalAmmo);

	UFUNCTION()
	void UpdateThrowAmmoCount(int32 NewTotalAmmo);
};
