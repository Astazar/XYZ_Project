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
	virtual void Jump() override;

	virtual void SwimForward(float Value) override;
	virtual void SwimRight(float Value) override;
	virtual void SwimUp(float Value) override;
	virtual void SwimDive() override;
	
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnStartCrawl(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
	virtual void OnEndCrawl(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	virtual void OnStartSlide(float HalfHeightAdjust);
	virtual void OnEndSlide(float HalfHeightAdjust);

	float GetDefaultFOV() const;
	float GetAimLookUpModifier() const;
	float GetAimTurnModifier() const;

	//The value by which to move the mesh so that the collision matches the mesh.
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Character | Movement | Crawl")
	float ToHeadOffset = 50.0f;

protected:
	virtual void BeginPlay() override;
	virtual void OnSprintStart_Implementation() override;
	virtual void OnSprintEnd_Implementation() override;
	virtual void UpdateCameraSprintTimeline(const float Alpha);

	virtual void OnStartAimingInternal() override;
	virtual void OnStopAimingInternal() override;
	virtual void UpdateCameraAimingTimeline(const float Alpha);
	void UpdateFOV(float NewFOV);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Character | Camera")
	class UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float BaseTurnRate = 45.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float BaseLookUpRate = 45.0f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement | Sprint")
	UCurveFloat* CameraSprintTimelineCurve;

	FTimeline CameraSprintTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement | Sprint")
	float SprintSpringArmLenght = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	UCurveFloat* CameraAimingTimelineCurve;

	FTimeline CameraAimingTimeline;

private:
	float DefaultSpringArmLeght = 0.0f;
};
