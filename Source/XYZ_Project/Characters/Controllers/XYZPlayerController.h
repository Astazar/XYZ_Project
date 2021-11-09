// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "XYZPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class XYZ_PROJECT_API AXYZPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void SetPawn(APawn* InPawn) override;
	
protected:
	virtual void SetupInputComponent() override;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void Jump();
	void ChangeCrouchState();
	void ChangeCrawlState();

	void StartSprint();
	void StopSprint();

	TSoftObjectPtr<class AXYZBaseCharacter> CachedBaseCharacter;
};
