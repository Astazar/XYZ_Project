// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "XYZ_Project/Components/LedgeDetectorComponent.h"
#include "XYZ_Project/XYZ_ProjectTypes.h"
#include "XYZBaseMovementComponent.generated.h"

#define FLAG_IsSprinting FLAG_Custom_0
#define FLAG_IsMantling FLAG_Custom_1

UENUM(BlueprintType)
enum class ESwimState : uint8 
{
	None = 0,
	OnWaterSurface,
	UnderWater
};

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
	CMOVE_Zipline UMETA(DisplayName = "Zipline"),
	CMOVE_Wallrun UMETA(DisplayName = "Wallrun"),
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

class FSavedMove_XYZCharacter : public FSavedMove_Character
{
	typedef FSavedMove_Character Super;

public: 
	virtual void Clear() override;

	virtual uint8 GetCompressedFlags() const override;

	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const override;

	virtual void SetMoveFor(ACharacter* InCharacter, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;

	virtual void PrepMoveFor(ACharacter* Character) override;

private:
	uint8 bSavedIsSprinting : 1;
	uint8 bSavedIsMantling : 1;
};

class FNetworkPredictionData_Client_XYZCharacter : public FNetworkPredictionData_Client_Character
{
	typedef FNetworkPredictionData_Client_Character Super;

public:
	FNetworkPredictionData_Client_XYZCharacter(const UCharacterMovementComponent& ClientMovement);

	virtual FSavedMovePtr AllocateNewMove() override;
};

UCLASS()
class XYZ_PROJECT_API UXYZBaseMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend class FSavedMove_XYZCharacter;

public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual void Wallrun();
	/** 
	* Detecting a hit with wall.
	* Return true if hit was detected.
	*/
	virtual bool DetectWall(struct FHitResult& OutHit, FVector CharacterLocation);
	EWallrunSide GetCurrentWallrunSide() const;
	virtual void JumpOffWall();
	virtual void StartWallrun(class UCapsuleComponent* CharacterCapsule, const struct FHitResult& Hit);
	virtual void StopWallrun();
	FVector GetWallrunCharacterMovingDirection(const struct FHitResult& Hit) const;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	bool IsSprinting() const { return bIsSprinting; }
	UFUNCTION(BlueprintCallable)
	bool IsCrawling() const { return bIsCrawling; }

	virtual float GetMaxSpeed() const override;

	void StartSprint();
	void StopSprint();

	bool CanSlideInCurrentState();
	void Slide();
	void StartSlide();
	void StopSlide();
	UFUNCTION(BlueprintCallable)
	bool IsSliding() const;
	UAnimMontage* GetSlideAnimMontage() const;
	bool bShouldResetSlideVelocity = false;

	void StartMantle(const FMantlingMovementParameters& MantlingParameters);
	void EndMantle();
	UFUNCTION(BlueprintCallable)
	bool IsMantling() const;

	void AttachToLadder(const class ALadder* Ladder);

	float GetActorToCurrentLadderProjection(const FVector& Location) const;
	void DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod = EDetachFromLadderMethod::Fall);
	UFUNCTION(BlueprintCallable)
	bool IsOnLadder() const;
	const class ALadder* GetCurrentLadder() const;
	float GetLadderSpeedRatio() const;

	float GetActorToCurrentZiplineProjection(const FVector& Location);
	void AttachToZipline(const class AZipline* Zipline);
	void DetachFromZipline();
	UFUNCTION(BlueprintCallable)
	bool IsZiplining() const;
	FVector CalcZiplineMovingDirection(const class AZipline* Zipline);
	void ZiplineClimbForward(float Value);
	void ZiplineTurnAround();

	virtual void PhysicsRotation(float DeltaTime) override;
	virtual void ForcePhysicsRotation(float DeltaTime); 
	virtual void SwimPhysicsRotation(float DeltaTime);

	bool CanEverCrawl();
	virtual void Crawl();
	virtual void Uncrawl();
	virtual bool CanCrawlInCurrentState();
	virtual bool IsEnoughSpaceToUncrouch();
	virtual bool IsEnoughSpaceToUncrawl();
	virtual bool IsEnoughSpaceToStandUp(bool bIsDebugEnabled = false);
	virtual bool IsEnoughSpaceToCrouch(bool bIsDebugEnabled = false);

	bool GetIsOutOfStamina() const { return bIsOutOfStamina; }
	void SetIsOutOfStamina(bool bIsOutOfStamina_In);

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	bool bWantsToCrawl = false;

	EMovementState PreviousMovementState = EMovementState::Standing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Crawl", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float CrawlingHalfHeight = 34.0f;

	bool IsWallrunning() const;

	void SwimDive();
	ESwimState GetCurrentSwimState() const { return CurrentSwimState; }
	void SetCurrentSwimState(ESwimState NewSwimState) { CurrentSwimState = NewSwimState; }

	void SelectMovementSettings(bool bEnableFreeLook);

protected:
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void PhysSwimming(float deltaTime, int32 Iterations) override;
	virtual void PhysSwimmingOnWaterSurface(float deltaTime, int32 Iterations);

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	void PhysMantling(float deltaTime, int32 Iterations);

	void PhysLadder(float deltaTime, int32 Iterations);

	void PhysZiplineClimb(float deltaTime, int32 Iterations);
	void PhysZiplineSlide(float deltaTime, int32 Iterations);
	void PhysMoveAlongZipline(float deltaTime, int32 Iterations);

	void PhysWallrun(float deltaTime, int32 Iterations);

	UPROPERTY(Category = "Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SwimmingCapsuleRadius = 60.0f;
	UPROPERTY(Category = "Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SwimmingCapsuleHalfHeight = 60.0f;
	/** Defines the maximum value of velocity Z at which the character will remain on the surface when jumping into the water.  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Swimming", meta = (ClampMin = "0", UIMin = "0"))
	float MaxSwimmingOnSurfaceVelocityZ = 500.0f;
	/** Defines at what speed character will dive under water*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Swimming", meta = (ClampMin = "0", UIMin = "0"))
	float DiveUnderWaterSpeed = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Swimming", meta = (ClampMin = "0", UIMin = "0"))
	float WaterLineOffset = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Swimming", meta = (ClampMin = "0", UIMin = "0"))
	float SprintSwimSpeed = 600.0f;

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float ClimbingOnLadderMaxSpeed = 200.0f;
	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float ClimbingOnLadderBreakingDeseleration = 2048.0f;
	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float LadderToCharacterOffset = 60.0f;
	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxLadderTopOffset = 50.0f;
	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MinLadderBottomOffset = 90.0f;
	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float JumpOffFromLadderSpeed = 500.0f;

	UPROPERTY(Category = "Character Movement: Zipline", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float ZiplineSlideSpeed = 500.0f;
	UPROPERTY(Category = "Character Movement: Zipline", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float ZiplineClimbMaxSpeed = 400.0f;
	UPROPERTY(Category = "Character Movement: Zipline", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float ZiplineCharacterZOffset = 125.0f;
	UPROPERTY(Category = "Character Movement: Zipline", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float FromPillarOffset = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 1200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float OutOfStaminaSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Crawl", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float CrawlSpeed = 100.0f;
	/** If true, this Pawn is capable of crawling. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NavMovement, meta = (DisplayName = "Can Crawl"))
	bool bCanEverCrawl = false;

	//Defines from what distance wallrun will be detected
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character movement: Wallrun", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float WallrunTraceLenght = 60.0f;
	//Defines the minimum angle at which the character must stand to start wallrun
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character movement: Wallrun", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 180.0f, UIMax = 180.0f))
	float WallrunMinAngleDeg = 45.0f;
	//Defines the maximum angle at which the character must stand to start wallrun
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character movement: Wallrun", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 180.0f, UIMax = 180.0f))
	float WallrunMaxAngleDeg = 135.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character movement: Wallrun", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float WallrunSpeed = 400.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character movement: Wallrun", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float WallrunTime = 10.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character movement: Wallrun", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float JumpOffWallVerticalVelocity = 100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character movement: Wallrun", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float JumpOffWallHorizontalVelocity = 600.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character movement: Wallrun", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float WallrunRotationInterpSpeed = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Slide", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SlideCapsuleHalfHeight = 55.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Slide", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SlideCapsuleRadius = 55.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Slide", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SlideSpeed = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Slide", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SlideStrafeAngleValue = 30.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character movement: Slide")
	UAnimMontage* SlideAnimMontage;

	class AXYZBaseCharacter* GetBaseCharacterOwner() const;

private:
	bool bIsSprinting = false;
	bool bIsOutOfStamina = false;
	bool bIsCrawling = false;
	bool bIsSliding = false;
	bool bIsDiving = false;

	FVector SlidingMovingDirection = FVector::ZeroVector;

	FMantlingMovementParameters CurrentMantlingParameters;
	FTimerHandle MantlingTimer;

	const ALadder* CurrentLadder = nullptr;

	const AZipline* CurrentZipline = nullptr;

	FRotator ForceTargetRotation = FRotator::ZeroRotator;
	bool bForceRotation = false;

	FTimerHandle WallrunTimer;
	EWallrunSide CurrentWallrunSide = EWallrunSide::None;
	EWallrunSide PreviousWallrunSide = EWallrunSide::None;

	ESwimState CurrentSwimState = ESwimState::None;
};
