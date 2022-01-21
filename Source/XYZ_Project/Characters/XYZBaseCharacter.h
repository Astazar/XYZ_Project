// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Components/TimelineComponent.h>
#include <Curves/CurveVector.h>
#include "XYZBaseCharacter.generated.h"

USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SettingsName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* MantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UCurveVector* MantlingCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimationCorrectionXY = 65.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimationCorrectionZ = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimMaxHeight = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimMinHeight = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxSettingApplyHeight = 250.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxHeightStartTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinHeightStartTime = 0.5f;
};



class UXYZBaseMovementComponent;
class AInteractiveActor;
typedef TArray<AInteractiveActor*, TInlineAllocator<10>> TInteractiveActorsArray; 

UCLASS(Abstract, NotBlueprintable)
class XYZ_PROJECT_API AXYZBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AXYZBaseCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void MoveForward(float Value) {};
	virtual void MoveRight(float Value) {};
	virtual void Turn(float Value) {};
	virtual void LookUp(float Value) {};

	virtual void ChangeCrouchState();
	virtual void ChangeCrawlState();

	virtual void Crawl();
	virtual void Uncrawl();

	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;

	virtual void StartSprint();
	virtual void StopSprint();
	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentStamina() const { return CurrentStamina; };
	virtual void UpdateStamina(float DeltaSeconds);

	virtual void Tick(float DeltaSeconds) override;

	virtual void SwimForward(float Value) {};
	virtual void SwimRight(float Value) {};
	virtual void SwimUp(float Value) {};

	virtual void Mantle(bool bForce = false);
	virtual bool CanMantle() const;

	virtual UXYZBaseMovementComponent* GetCharacterMovementComponent() const { return XYZBaseCharacterMovementComponent; }

	float GetIKRightFootOffset() const { return IKRightFootOffset; }
	float GetIKLeftFootOffset() const { return IKLeftFootOffset; }
	float GetIKPelvisOffset() const { return IKPelvisOffset; }

	void RegisterInteractiveActor(AInteractiveActor* InteractiveActor);
	void UnregisterInteractiveActor(AInteractiveActor* InteractiveActor);

	bool CanInteractWithLadder();
	void ClimbLadderUp(float Value);
	void InteractWithLadder();
	const class ALadder* GetAvailableLadder() const; 

	virtual void InteractWithZipline();
	virtual void ZiplineClimbForward(float Value);
	virtual void ZiplineTurnAround();
	const class AZipline* GetAvailableZipline() const;

	virtual void Wallrun();
	virtual bool CanWallrun();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSprintStart();
	virtual void OnSprintStart_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSprintEnd();
	virtual void OnSprintEnd_Implementation();

	virtual bool CanSprint();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement | Sprint")
	float MaxStamina = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement | Sprint")
	float StaminaRestoreVelocity = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement | Sprint")
	float SprintStaminaConsumptionVelocity = 200.0f;

	UXYZBaseMovementComponent* XYZBaseCharacterMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings")
	FName RightFootSocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings")
	FName LeftFootSocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings")
	float IKInterpSpeed = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | IK settings")
	float UnderFeetTraceLenght = 50.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	class ULedgeDetectorComponent* LedgeDetectorComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	TArray<FMantlingSettings> MantlingSettingsCollection;

private:
	void UpdateIKOffsets(float DeltaSeconds);
	void TryChangeSprintState(float DeltaSeconds);
	bool bIsSprintRequested = false;

	float CurrentStamina = 0.0f;

	float GetIKOffsetForASocket(const FName& SocketName);
	float CalculateIKPelvisOffset();

	const FMantlingSettings* GetMantlingSettings(float LedgeHeight) const;

	TInteractiveActorsArray AvailableInteractiveActors;

	float IKRightFootOffset = 0.0f;
	float IKLeftFootOffset = 0.0f;
	float IKPelvisOffset = 0.0f;
	float IKTraceDistance = 0.0f;
	float IKScale = 0.0f;
};
