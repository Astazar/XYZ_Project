// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XYZ_Project/Characters/PlayerCharacter.h"
#include "FPPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class XYZ_PROJECT_API AFPPlayerCharacter : public APlayerCharacter
{
	GENERATED_BODY()

public:
	AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual	void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First person")
	class USkeletalMeshComponent* FirstPersonMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First person")
	class UCameraComponent* FirstPersonCameraComponent;
	
};
