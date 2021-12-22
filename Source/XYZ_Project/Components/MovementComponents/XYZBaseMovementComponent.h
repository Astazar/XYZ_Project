// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "XYZ_Project/Components/LedgeDetectorComponent.h"
#include "XYZBaseMovementComponent.generated.h"

struct FMantlingMovementParameters
{
	FVector InitialLocation = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;

	FVector TargetLocation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;

	UPrimitiveComponent* Geometry;
	FVector InitialGeometryLocation = FVector::ZeroVector;

	FVector InitialAnimationLocation = FVector::ZeroVector;

	float Duration = 1.0f;
	float StartTime = 0.0f;

	UCurveVector* MantlingCurve;
};			

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	CMOVE_None = 0 UMETA(DisplayName = "None"),
	CMOVE_Mantling UMETA(DisplayName = "Mantling"),
	CMOVE_Ladder UMETA(DisplayName = "Ladder"),
	CMOVE_Max UMETA(Hidden)
};


UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Standing = 0,
	Crawling
};

UENUM(BlueprintType)
enum class EDetachFromLadderMethod : uint8
{
	Fall = 0,
	ReachingTheTop,
	ReachingTheBottom,
	JumpOff
};



UCLASS()
class XYZ_PROJECT_API UXYZBaseMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	bool IsSprinting() { return bIsSprinting; }
	bool IsCrawling() { return bIsCrawling; }

	virtual float GetMaxSpeed() const override;

	void StartSprint();
	void StopSprint();

	void StartMantle(const FMantlingMovementParameters& MantlingParameters);
	void EndMantle();
	bool IsMantling() const;

	void AttachToLadder(const class ALadder* Ladder);

	float GetActorToCurrentLadderProjection(const FVector& Location) const;
	void DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod = EDetachFromLadderMethod::Fall);
	bool IsOnLadder() const;
	const class ALadder* GetCurrentLadder() const;
	float GetLadderSpeedRatio() const;

	virtual void PhysicsRotation(float DeltaTime) override;


	virtual void Crawl();
	virtual void Uncrawl();
	virtual bool CanCrawlInCurrentState();

	virtual bool IsEnoughSpaceToUncrouch();
	virtual bool IsEnoughSpaceToUncrawl();

	bool GetIsOutOfStamina() const { return bIsOutOfStamina; }
	void SetIsOutOfStamina(bool bIsOutOfStamina_In);

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	bool bWantsToCrawl = false;

	EMovementState PreviousMovementState = EMovementState::Standing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Crawl", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float CrawlingHalfHeight = 34.0f;

protected:
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	void PhysMantling(float deltaTime, int32 Iterations);

	void PhysLadder(float deltaTime, int32 Iterations);

	UPROPERTY(Category = "Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SwimmingCapsuleRadius = 60.0f;
	UPROPERTY(Category = "Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SwimmingCapsuleHalfHeight = 60.0f;

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float ClimbingOnLadderMaxSpeed = 200.0f;
	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float ClimbingOnLadderBreakingDeseleration = 2048.0f;
	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float LadderToCharacterOffset = 60.0f;
	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxLadderTopOffset = 90.0f;
	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MinLadderBottomOffset = 90.0f;
	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float JumpOffFromLadderSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 1200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float OutOfStaminaSpeed = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Crawl", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float CrawlSpeed = 100.0f;

	class AXYZBaseCharacter* GetBaseCharacterOwner() const;

private:
	bool bIsSprinting = false;
	bool bIsOutOfStamina = false;
	bool bIsCrawling = false;

	FMantlingMovementParameters CurrentMantlingParameters;
	FTimerHandle MantlingTimer;

	const ALadder* CurrentLadder = nullptr;

	FRotator ForceTargetRotation = FRotator::ZeroRotator;
	bool bForceRotation = false;
};
