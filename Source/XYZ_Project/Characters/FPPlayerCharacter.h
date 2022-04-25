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

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual	void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual FRotator GetViewRotation() const override;

	virtual void ZiplineTurnAround() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* FirstPersonMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* FirstPersonCameraComponent;
	
	virtual void OnMantle(const FMantlingSettings* MantlingSettings, float MantlingAnimationStartTime) override;

	void SetOnLadderViewLimits();
	void SetOnZiplineViewLimits();
	void ResetViewLimits();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder", meta = (ClampMin = -89.0f, UIMin = -89.0f, ClampMax = 89.0f, UIMax  = 89.0f))
	float LadderCameraMinPitch = -60.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder", meta = (ClampMin = -89.0f, UIMin = -89.0f, ClampMax = 89.0f, UIMax = 89.0f))
	float LadderCameraMaxPitch = 80.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder", meta = (ClampMin = -89.0f, UIMin = -89.0f, ClampMax = 89.0f, UIMax = 89.0f))
	float LadderCameraMinYaw = -45.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder", meta = (ClampMin = -89.0f, UIMin = -89.0f, ClampMax = 89.0f, UIMax = 89.0f))
	float LadderCameraMaxYaw = 45.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Zipline", meta = (ClampMin = -89.0f, UIMin = -89.0f, ClampMax = 89.0f, UIMax = 89.0f))
	float ZiplineCameraMinPitch = -60.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Zipline", meta = (ClampMin = -89.0f, UIMin = -89.0f, ClampMax = 89.0f, UIMax = 89.0f))
	float ZiplineCameraMaxPitch = 80.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Zipline", meta = (ClampMin = -89.0f, UIMin = -89.0f, ClampMax = 89.0f, UIMax = 89.0f))
	float ZiplineCameraMinYaw = -45.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Zipline", meta = (ClampMin = -89.0f, UIMin = -89.0f, ClampMax = 89.0f, UIMax = 89.0f))
	float ZiplineCameraMaxYaw = 45.0f;

private:
	FTimerHandle FPMontageTimer;
	void OnFPMontageTimerElapsed();

	bool IsFPMontagePlaying() const;

	TWeakObjectPtr<class AXYZPlayerController> XYZPlayerController;
};
