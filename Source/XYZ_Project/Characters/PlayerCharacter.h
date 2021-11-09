// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XYZBaseCharacter.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class XYZ_PROJECT_API APlayerCharacter : public AXYZBaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaSeconds) override;

	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void Turn(float Value) override;
	virtual void LookUp(float Value) override;
	
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnSprintStart_Implementation() override;
	virtual void OnSprintEnd_Implementation() override;
	virtual void UpdateCameraSprintTimeline(const float Alpha);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Character | Camera")
	class UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	class USpringArmComponent* SpringArmComponent;

	FTimeline CameraSprintTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	UCurveFloat* CameraSprintTimelineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	float SprintSpringArmLenght = 400.0f;

private:
	float DefaultSpringArmLeght = 0.0f;
};
