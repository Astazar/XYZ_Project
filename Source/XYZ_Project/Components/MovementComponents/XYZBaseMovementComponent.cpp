// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZBaseMovementComponent.h"
#include "GameFramework/Character.h"
#include <Components/CapsuleComponent.h>
#include <Kismet/KismetSystemLibrary.h>
#include "XYZ_Project/Characters/PlayerCharacter.h"
#include <DrawDebugHelpers.h>
#include "XYZ_Project/Actors/Interactive/Environment/Ladder.h"
#include "XYZ_Project/Characters/XYZBaseCharacter.h"
#include "XYZ_Project/Actors/Interactive/Environment/Zipline.h"
#include "XYZ_Project/Utils/XYZTraceUtils.h"
#include "XYZ_Project/XYZ_ProjectTypes.h"
#include "XYZ_Project/Subsystems/DebugSubsystem.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>


void UXYZBaseMovementComponent::Wallrun()
{
	UCapsuleComponent* CharacterCapsule = GetBaseCharacterOwner()->GetCapsuleComponent();
	FHitResult TraceHitResult;	

#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool IsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryWallrun);
#else
	bool IsDebugEnabled = false;
#endif

	if (!DetectWall(TraceHitResult, CharacterCapsule->GetComponentLocation()))
	{
		return;
	}
	FVector HitNormal = TraceHitResult.ImpactNormal.GetSafeNormal();
	
	bool bIsAngleCorrect=false;
	float CharacterAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(-HitNormal, CharacterCapsule->GetForwardVector())));
	if ((CharacterAngle > WallrunMinAngleDeg) && (CharacterAngle < WallrunMaxAngleDeg))
	{
		bIsAngleCorrect=true;
	}
	else
	{
		if (IsDebugEnabled)
		{
			GEngine->AddOnScreenDebugMessage(1, 5, FColor::Purple, FString::Printf(TEXT("Angle is not correct. Angle = %s"), *FString::SanitizeFloat(CharacterAngle)));
		}
		return;
	}
	if (IsDebugEnabled)
	{
		GEngine->AddOnScreenDebugMessage(1, 5, FColor::Purple, FString::Printf(TEXT("Angle is correct. Angle = %s"), *FString::SanitizeFloat(CharacterAngle)));
	}
	
	if (FVector::DotProduct(HitNormal, CharacterCapsule->GetRightVector()) > 0)
	{
		CurrentWallrunSide = EWallrunSide::Left;
	}
	else
	{
		CurrentWallrunSide = EWallrunSide::Right;
	}
	if (CurrentWallrunSide == PreviousWallrunSide)
	{
		return;
	}

	if (IsDebugEnabled)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Purple, FString::Printf(TEXT("Wallrun side: %s"), *UEnum::GetValueAsString(CurrentWallrunSide)));
	}

	StartWallrun(CharacterCapsule, TraceHitResult);
}

bool UXYZBaseMovementComponent::DetectWall(struct FHitResult& OutHit, FVector CharacterLocation)
{
#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool IsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryWallrun);
#else
	bool IsDebugEnabled = false;
#endif

	FCollisionQueryParams QuerryParams;
	QuerryParams.bTraceComplex = true;
	QuerryParams.AddIgnoredActor(GetOwner());
	UCapsuleComponent* CharacterCapsule = GetBaseCharacterOwner()->GetCapsuleComponent();
	FVector LeftCheckStart = CharacterCapsule->GetComponentLocation();
	FVector LeftCheckEnd = CharacterCapsule->GetComponentLocation() - CharacterCapsule->GetRightVector() * WallrunTraceLenght;
	bool bIsHitedLeft = XYZTraceUtils::LineTraceSingleByChannel(GetWorld(), OutHit, LeftCheckStart, LeftCheckEnd, ECC_WallRunnable, QuerryParams, FCollisionResponseParams::DefaultResponseParam, IsDebugEnabled, 10);
	if (OutHit.bBlockingHit)
	{
		return bIsHitedLeft;
	}
	FVector RightCheckStart = LeftCheckStart;
	FVector RightCheckEnd = CharacterCapsule->GetComponentLocation() + CharacterCapsule->GetRightVector() * WallrunTraceLenght;
	bool bIsHitedRight = XYZTraceUtils::LineTraceSingleByChannel(GetWorld(), OutHit, RightCheckStart, RightCheckEnd, ECC_WallRunnable, QuerryParams, FCollisionResponseParams::DefaultResponseParam, IsDebugEnabled, 10);
	return bIsHitedRight;
}

EWallrunSide UXYZBaseMovementComponent::GetCurrentWallrunSide() const
{
	return CurrentWallrunSide;
}

void UXYZBaseMovementComponent::JumpOffWall()
{
	StopWallrun();
	FVector MovingDirection;
	FVector JumpOffVelocity;
	if (CurrentWallrunSide == EWallrunSide::Left)
	{
		MovingDirection = GetOwner()->GetActorRightVector() + Velocity.GetSafeNormal();
	}
	else if (CurrentWallrunSide == EWallrunSide::Right)
	{
		MovingDirection = -GetOwner()->GetActorRightVector() + Velocity.GetSafeNormal();
	}
	JumpOffVelocity = MovingDirection * JumpOffWallHorizontalVelocity;
	ForceTargetRotation = MovingDirection.ToOrientationRotator();
	bForceRotation = true;
	JumpOffVelocity += GetOwner()->GetActorUpVector() * JumpOffWallVerticalVelocity;
	Launch(JumpOffVelocity);
}

void UXYZBaseMovementComponent::StartWallrun(class UCapsuleComponent* CharacterCapsule, const struct FHitResult& Hit)
{
	GetWorld()->GetTimerManager().SetTimer(WallrunTimer, this, &UXYZBaseMovementComponent::StopWallrun, WallrunTime, false);
	FVector CharacterStartLocation = Hit.ImpactPoint + Hit.ImpactNormal.GetSafeNormal() * CharacterCapsule->GetScaledCapsuleRadius();
	CharacterStartLocation.Z = CharacterCapsule->GetComponentLocation().Z;
	GetOwner()->SetActorLocation(CharacterStartLocation);

	FRotator CharacterRotation = GetWallrunCharacterMovingDirection(Hit).ToOrientationRotator();
	GetOwner()->SetActorRotation(CharacterRotation);
	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Wallrun);
}

void UXYZBaseMovementComponent::StopWallrun()
{
	PreviousWallrunSide = CurrentWallrunSide;
	SetMovementMode(MOVE_Falling);
}

FVector UXYZBaseMovementComponent::GetWallrunCharacterMovingDirection(const struct FHitResult& Hit) const
{
	return CurrentWallrunSide == EWallrunSide::Left ? FVector::CrossProduct(Hit.ImpactNormal, FVector::UpVector) : FVector::CrossProduct(FVector::UpVector, Hit.ImpactNormal);
}

float UXYZBaseMovementComponent::GetMaxSpeed() const 
{
	float Result = Super::GetMaxSpeed();
	if (bIsSprinting)
	{
		Result = SprintSpeed;
	}
	else if (bIsCrawling)
	{
		Result = CrawlSpeed;
	}
	else if (bIsOutOfStamina)
	{
		Result = OutOfStaminaSpeed;
	}
	else if (IsOnLadder())
	{
		Result = ClimbingOnLadderMaxSpeed;
	}
	else if (IsZiplining() && CurrentZipline->GetZiplineMovementType() == EZiplineMovementType::Climb)
	{
		Result = ZiplineClimbMaxSpeed;
	}
	else if (IsWallrunning())
	{
		Result = WallrunSpeed;
	}
	return Result;
}

void UXYZBaseMovementComponent::StartSprint()
{
	bIsSprinting = true;
	bForceMaxAccel = 1;
}

void UXYZBaseMovementComponent::StopSprint()
{
	bIsSprinting = false;
	bForceMaxAccel = 0;
}



void UXYZBaseMovementComponent::StartMantle(const FMantlingMovementParameters& MantlingParameters)
{
	CurrentMantlingParameters = MantlingParameters;
	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Mantling);
}

void UXYZBaseMovementComponent::EndMantle()
{
	SetMovementMode(MOVE_Walking);
}

bool UXYZBaseMovementComponent::IsMantling() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Mantling;
}

void UXYZBaseMovementComponent::AttachToLadder(const class ALadder* Ladder)
{
	CurrentLadder = Ladder;
	FRotator TargetOrientationRotation = CurrentLadder->GetActorForwardVector().ToOrientationRotator();
	TargetOrientationRotation.Yaw += 180.0f;

	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderForwardVector = CurrentLadder->GetActorForwardVector();
	float Projection = GetActorToCurrentLadderProjection(GetActorLocation());
	FVector NewCharacterLocation = CurrentLadder->GetActorLocation() + Projection * LadderUpVector + LadderToCharacterOffset * LadderForwardVector;

	if (CurrentLadder->GetIsOnTop())
	{
		NewCharacterLocation = CurrentLadder->GetAttachFromTopAnimMontageStartingLocation();
	}

	GetOwner()->SetActorLocation(NewCharacterLocation);
	GetOwner()->SetActorRotation(TargetOrientationRotation);
	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Ladder);
}

float UXYZBaseMovementComponent::GetActorToCurrentLadderProjection(const FVector& Location) const
{
	checkf(IsValid(CurrentLadder), TEXT("UXYZBaseMovementComponent::GetCharacterToCurrentLadderProjection cannot be invoked when current ladder is null"));
	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderToCharacterDistance = Location - CurrentLadder->GetActorLocation();
	return FVector::DotProduct(LadderUpVector, LadderToCharacterDistance);
}

void UXYZBaseMovementComponent::DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod /*= EDetachFromLadderMethod::Fall*/)
{
	switch (DetachFromLadderMethod)
	{
	case EDetachFromLadderMethod::JumpOff:
	{
		FVector JumpDirection = CurrentLadder->GetActorForwardVector();
		SetMovementMode(MOVE_Falling);
		FVector JumpVelocity = JumpDirection * JumpOffFromLadderSpeed;
		ForceTargetRotation = JumpDirection.ToOrientationRotator();
		bForceRotation=true;
		Launch(JumpVelocity);
		break;
	}
	case EDetachFromLadderMethod::ReachingTheTop:
	{
		GetBaseCharacterOwner()->Mantle(true);
		break;
	}
	case EDetachFromLadderMethod::ReachingTheBottom:
	{
		SetMovementMode(MOVE_Walking);
		break;
	}
	case EDetachFromLadderMethod::Fall:
	default:
	{
		SetMovementMode(MOVE_Falling);
		break;
	}
	}
}
	

bool UXYZBaseMovementComponent::IsOnLadder() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Ladder;
}

const class ALadder* UXYZBaseMovementComponent::GetCurrentLadder() const
{
	return CurrentLadder;
}

float UXYZBaseMovementComponent::GetLadderSpeedRatio() const
{
	checkf(IsValid(CurrentLadder), TEXT("UXYZBaseMovementComponent::GetLadderSpeedRatio cannot be invoked when current ladder is null"));
	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	return FVector::DotProduct(LadderUpVector, Velocity) / ClimbingOnLadderMaxSpeed;
}

float UXYZBaseMovementComponent::GetActorToCurrentZiplineProjection(const FVector& Location)
{
	checkf(IsValid(CurrentZipline), TEXT("UXYZBaseMovementComponent::GetActorToCurrentZiplineProjection cannot be invoked when current zipline is null"));
	FVector ZiplineToActorVector = Location - CurrentZipline->GetCableMeshLocation();
	FVector ZiplineVector = CalcZiplineMovingDirection(CurrentZipline);
	ZiplineVector.Normalize();
	return FVector::DotProduct(ZiplineVector, ZiplineToActorVector);
}

void UXYZBaseMovementComponent::AttachToZipline(const class AZipline* Zipline)
{
	CurrentZipline = Zipline;
	FVector MovingDirection = CalcZiplineMovingDirection(CurrentZipline);
	FRotator TargetOrientationRotation = MovingDirection.ToOrientationRotator();
	GetOwner()->SetActorRelativeRotation(TargetOrientationRotation);

	FVector ZiplineUpVector = CurrentZipline->GetCableMesh()->GetUpVector();
	float Projection = GetActorToCurrentZiplineProjection(GetActorLocation());
	FVector NewCharacterLocation = CurrentZipline->GetCableMeshLocation() + Projection * MovingDirection - ZiplineCharacterZOffset * ZiplineUpVector;
	GetOwner()->SetActorRelativeLocation(NewCharacterLocation);

	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Zipline);
}

void UXYZBaseMovementComponent::DetachFromZipline()
{
	SetMovementMode(MOVE_Falling);
}

bool UXYZBaseMovementComponent::IsZiplining() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Zipline;
}

FVector UXYZBaseMovementComponent::CalcZiplineMovingDirection(const class AZipline* Zipline)
{
	FVector MovingDirection;
	if (Zipline->GetZiplineMovementType() == EZiplineMovementType::Slide)
	{
		//the direction of movement directed from higher to lower
		Zipline->GetEndPillarTopWorldLocation().Z > Zipline->GetStartPillarTopWorldLocation().Z ? MovingDirection = Zipline->GetStartPillarTopWorldLocation() - Zipline->GetEndPillarTopWorldLocation() : MovingDirection = Zipline->GetEndPillarTopWorldLocation() - Zipline->GetStartPillarTopWorldLocation();
	}
	else if (Zipline->GetZiplineMovementType() == EZiplineMovementType::Climb)
	{
		//the direction of movement depends on the direction of the character
		FVector CableDirection = Zipline->GetStartPillarTopWorldLocation() - Zipline->GetEndPillarTopWorldLocation();
		FVector OwnerForwardVector = GetOwner()->GetActorForwardVector();
		if (FVector::DotProduct(CableDirection, OwnerForwardVector) < 0.0f)
		{
			MovingDirection = CableDirection * (-1);
		}
		else MovingDirection = CableDirection;
	}
	MovingDirection.Normalize();
	return MovingDirection;
}


void UXYZBaseMovementComponent::ZiplineClimbForward(float Value)
{
	GetBaseCharacterOwner()->AddMovementInput(GetOwner()->GetActorForwardVector(), Value);
}

void UXYZBaseMovementComponent::ZiplineTurnAround()
{
	FVector MovingDirection = CalcZiplineMovingDirection(GetBaseCharacterOwner()->GetAvailableZipline());
	MovingDirection *= -1;
	FRotator MovingDirectionRotator = MovingDirection.ToOrientationRotator();
	GetOwner()->SetActorRotation(MovingDirectionRotator);
}

void UXYZBaseMovementComponent::PhysicsRotation(float DeltaTime)
{	
	if (bForceRotation)
	{
		FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
		CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

		FRotator DeltaRot = GetDeltaRotation(DeltaTime);
		DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));
		
		// Accumulate a desired new rotation.
		const float AngleTolerance = 1e-3f;

		if (!CurrentRotation.Equals(ForceTargetRotation, AngleTolerance))
		{
			FRotator DesiredRotation = ForceTargetRotation;
			// PITCH
			if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
			{
				DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
			}

			// YAW
			if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
			{
				DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
			}

			// ROLL
			if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
			{
				DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
			}

			// Set the new rotation.
			DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
			MoveUpdatedComponent(FVector::ZeroVector, DesiredRotation, /*bSweep*/ false);
		}
		else
		{
			ForceTargetRotation = FRotator::ZeroRotator;
			bForceRotation = false;
		}
		return;
	}
	if (IsOnLadder())
	{
		return;
	}
	Super::PhysicsRotation(DeltaTime);
}

void UXYZBaseMovementComponent::Crawl()
{
	if (!HasValidData())
	{
		return;
	}

	if (!CanCrawlInCurrentState())
	{
		return;
	}

	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();/**/
	const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	const float UnscaledHalfHeight = CrawlingHalfHeight;
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, UnscaledHalfHeight);

	float HalfHeightAdjust = (OldUnscaledHalfHeight - UnscaledHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	FVector CapsuleLocation = CharacterOwner->GetCapsuleComponent()->GetRelativeLocation();
	FVector CrawlingCapsuleLocation = FVector(CapsuleLocation.X, CapsuleLocation.Y, CapsuleLocation.Z - HalfHeightAdjust);
	CharacterOwner->GetCapsuleComponent()->SetRelativeLocation(CrawlingCapsuleLocation);

	APlayerCharacter* CachedCharacter = StaticCast<APlayerCharacter*>(GetOwner());
	CachedCharacter->OnStartCrawl(HalfHeightAdjust,ScaledHalfHeightAdjust);

	CharacterOwner->bIsCrouched=false;
	bIsCrawling = true;
}

void UXYZBaseMovementComponent::Uncrawl()
{
	if (!HasValidData())
	{
		return;
	}

	const APlayerCharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<APlayerCharacter>();
	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - OldUnscaledHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	FVector PawnLocation = UpdatedComponent->GetComponentLocation();
	PawnLocation.Z += HalfHeightAdjust;
	CharacterOwner->GetCapsuleComponent()->SetRelativeLocation(PawnLocation);
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);

	APlayerCharacter* CachedCharacter = StaticCast<APlayerCharacter*>(GetOwner());
	CachedCharacter->OnEndCrawl(HalfHeightAdjust, ScaledHalfHeightAdjust);
	bIsCrawling = false;
}

bool UXYZBaseMovementComponent::CanCrawlInCurrentState()
{
	return IsMovingOnGround() && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();
}

bool UXYZBaseMovementComponent::IsEnoughSpaceToUncrouch()
{
	const float CurrentCrouchedCapsuleHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	const float DefaultCapsuleHalfHeight = DefaultCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float HalfHeightAdjust = DefaultCapsuleHalfHeight - CurrentCrouchedCapsuleHalfHeight;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
	const FVector OverlapCapsuleLocation = FVector(PawnLocation.X, PawnLocation.Y, PawnLocation.Z + HalfHeightAdjust);

	const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(UCharacterMovementComponent::SHRINK_HeightCustom, - HalfHeightAdjust);
	FCollisionQueryParams CapsuleParams("CrouchOverlapTest",false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleParams, ResponseParam);

	bool IsEnoughSpace = !(GetWorld()->OverlapBlockingTestByChannel(OverlapCapsuleLocation,FQuat::Identity, ECC_Visibility, StandingCapsuleShape, CapsuleParams, ResponseParam));
	return IsEnoughSpace;
}

bool UXYZBaseMovementComponent::IsEnoughSpaceToUncrawl()
{
	const float CurrentCrawledCapsuleHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float HalfHeightAdjust = CrouchedHalfHeight - CurrentCrawledCapsuleHalfHeight;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
	const FVector OverlapCapsuleLocation = FVector(PawnLocation.X, PawnLocation.Y, PawnLocation.Z + HalfHeightAdjust);

	const FCollisionShape CrouchedCapsuleShape = GetPawnCapsuleCollisionShape(UCharacterMovementComponent::SHRINK_HeightCustom, -HalfHeightAdjust);
	FCollisionQueryParams CapsuleParams("CrawlOverlapTest", false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleParams, ResponseParam);

	bool IsEnoughSpace = !(GetWorld()->OverlapBlockingTestByChannel(OverlapCapsuleLocation, FQuat::Identity, ECC_Visibility, CrouchedCapsuleShape, CapsuleParams, ResponseParam));
	return IsEnoughSpace;
}

void UXYZBaseMovementComponent::SetIsOutOfStamina(bool bIsOutOfStamina_In)
{
	bIsOutOfStamina = bIsOutOfStamina_In;
	if (IsSprinting())
	{
		StopSprint();
	}
}

void UXYZBaseMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	//must be before base method. Uncrawling and uncrouching should calls before crawling and crouching.
	if ((!bWantsToCrawl || !CanCrawlInCurrentState()) && IsCrawling())
	{
		Uncrawl();
	}
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
	if (bWantsToCrawl && CanCrawlInCurrentState() && !IsCrawling())
	{
		Crawl();
	}
}

bool UXYZBaseMovementComponent::IsWallrunning() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Wallrun;;
}

void UXYZBaseMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	if (MovementMode == MOVE_Swimming)
	{
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(SwimmingCapsuleRadius,SwimmingCapsuleHalfHeight);
	}
	else if (PreviousMovementMode == MOVE_Swimming)
	{
		ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
	}

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Ladder)
	{
		CurrentLadder = nullptr;
	}

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Zipline)
	{
		CurrentZipline = nullptr;
	}

	if(MovementMode == MOVE_Walking && PreviousMovementMode == MOVE_Falling)
	{
		PreviousWallrunSide = EWallrunSide::None;
	}

	if (MovementMode == MOVE_Custom)
	{
		//fixing wrong attaching position (ladder, zipline) because of initial velocity while attaching
		Velocity = FVector::ZeroVector;
		switch (CustomMovementMode)
		{
		case (uint8)ECustomMovementMode::CMOVE_Mantling:
		{
			GetWorld()->GetTimerManager().SetTimer(MantlingTimer, this, &UXYZBaseMovementComponent::EndMantle, CurrentMantlingParameters.Duration, false);
			break;
		}
		default:
			break;
		}
	}
}

void UXYZBaseMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
	case (uint8)ECustomMovementMode::CMOVE_Mantling:
	{
		PhysMantling(deltaTime, Iterations);
		break;
	}
	case (uint8)ECustomMovementMode::CMOVE_Ladder:
	{
		PhysLadder(deltaTime,Iterations);
		break;
	}
	case (uint8)ECustomMovementMode::CMOVE_Zipline:
	{
		FVector MovingDirection = CalcZiplineMovingDirection(CurrentZipline);
		if (CurrentZipline->GetZiplineMovementType() == EZiplineMovementType::Climb)
		{
			PhysZiplineClimb(deltaTime, Iterations);
		}
		else if (CurrentZipline->GetZiplineMovementType() == EZiplineMovementType::Slide)
		{
			PhysZiplineSlide(deltaTime, Iterations);
		}
		break;
	}
	case (uint8)ECustomMovementMode::CMOVE_Wallrun:
	{
		PhysWallrun(deltaTime, Iterations);
		break;
	}
	default:
		break;
	}
	Super::PhysCustom(deltaTime, Iterations);
}

void UXYZBaseMovementComponent::PhysMantling(float deltaTime, int32 Iterations)
{
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimer) + CurrentMantlingParameters.StartTime;
	FVector MantlingCurveValue = CurrentMantlingParameters.MantlingCurve->GetVectorValue(ElapsedTime);
	float PositionAlpha = MantlingCurveValue.X;
	float XYCorrectionAlpha = MantlingCurveValue.Y;
	float ZCorrectionAlpha = MantlingCurveValue.Z;

	FVector CorrectedInitialLocation = FMath::Lerp(CurrentMantlingParameters.InitialLocation, CurrentMantlingParameters.InitialAnimationLocation, XYCorrectionAlpha);
	CorrectedInitialLocation.Z = FMath::Lerp(CurrentMantlingParameters.InitialLocation.Z, CurrentMantlingParameters.InitialAnimationLocation.Z, ZCorrectionAlpha);
	FVector NewLocation = FMath::Lerp(CorrectedInitialLocation, CurrentMantlingParameters.TargetLocation, PositionAlpha);
	FRotator NewRotation = FMath::Lerp(CurrentMantlingParameters.InitialRotation, CurrentMantlingParameters.TargetRotation, PositionAlpha);

	FVector Delta = NewLocation - GetActorLocation();
	FVector TargetDelta = CurrentMantlingParameters.Geometry->GetComponentLocation() - CurrentMantlingParameters.InitialGeometryLocation;
	Delta += TargetDelta;
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, NewRotation, false, Hit);
}

void UXYZBaseMovementComponent::PhysLadder(float deltaTime, int32 Iterations)
{
	CalcVelocity(deltaTime, 1.0f, false, ClimbingOnLadderBreakingDeseleration);
	FVector Delta = Velocity * deltaTime;

	if (HasAnimRootMotion())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit);
		return;
	}

	FVector NewPos = GetActorLocation() + Delta;
	float NewPosProjection = GetActorToCurrentLadderProjection(NewPos);

	if (NewPosProjection < MinLadderBottomOffset)
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheBottom);
		return;
	}
	else if (NewPosProjection > (CurrentLadder->GetLadderHeight() - MaxLadderTopOffset))
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheTop);
		return;
	}

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
}

AXYZBaseCharacter* UXYZBaseMovementComponent::GetBaseCharacterOwner() const
{
	return StaticCast<AXYZBaseCharacter*>(GetOwner());
}

void UXYZBaseMovementComponent::PhysZiplineClimb(float deltaTime, int32 Iterations)
{
	CalcVelocity(deltaTime, 1.0f, false, ClimbingOnLadderBreakingDeseleration);
	PhysMoveAlongZipline(deltaTime, Iterations);
}

void UXYZBaseMovementComponent::PhysZiplineSlide(float deltaTime, int32 Iterations)
{
	FVector MovingDirection = CalcZiplineMovingDirection(CurrentZipline);
	Velocity = MovingDirection * ZiplineSlideSpeed;
	PhysMoveAlongZipline(deltaTime, Iterations);
}

void UXYZBaseMovementComponent::PhysMoveAlongZipline(float deltaTime, int32 Iterations)
{
	FVector Delta = deltaTime * Velocity;
	FVector SupposedLocation = GetActorLocation() + Delta;
	FVector OnZiplineSupposedLocation = SupposedLocation + GetOwner()->GetActorUpVector() * ZiplineCharacterZOffset;

	float DistanceToEndPillar = (CurrentZipline->GetEndPillarTopWorldLocation() - OnZiplineSupposedLocation).Size();
	float DistanceToStartPillar = (CurrentZipline->GetStartPillarTopWorldLocation() - OnZiplineSupposedLocation).Size();
	if ((DistanceToEndPillar < FromPillarOffset) || (DistanceToStartPillar < FromPillarOffset))
	{
		Velocity = FVector::ZeroVector;
		DetachFromZipline();
	}

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
	if (Hit.bBlockingHit)
	{
		DetachFromZipline();
	}
}

void UXYZBaseMovementComponent::PhysWallrun(float deltaTime, int32 Iterations)
{
	FHitResult WallHit;
	if (!DetectWall(WallHit, GetOwner()->GetActorLocation()))
	{
		StopWallrun();
		return;
	}

	FVector MovingDirection = GetWallrunCharacterMovingDirection(WallHit);

	FRotator CharacterRotation = UKismetMathLibrary::RInterpTo(GetOwner()->GetActorRotation(), MovingDirection.ToOrientationRotator(), deltaTime, WallrunRotationInterpSpeed);
	GetOwner()->SetActorRotation(CharacterRotation);

	Velocity = MovingDirection * WallrunSpeed;
	FVector Delta = deltaTime * Velocity;
	SlideAlongSurface(Delta, 1, WallHit.ImpactNormal, WallHit, true);

	if (WallHit.bBlockingHit)
	{
		StopWallrun();
		return;
	}
}

