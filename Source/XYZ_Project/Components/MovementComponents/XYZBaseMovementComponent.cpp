// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZBaseMovementComponent.h"
#include "GameFramework/Character.h"
#include <Components/CapsuleComponent.h>
#include <Kismet/KismetSystemLibrary.h>
#include "XYZ_Project/Characters/PlayerCharacter.h"
#include <DrawDebugHelpers.h>

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
	if (bIsOutOfStamina)
	{
		Result = OutOfStaminaSpeed;
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

	if (MovementMode == MOVE_Custom)
	{
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
		Delta+=TargetDelta;
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, NewRotation, false, Hit);
		break;
	}
	default:
		break;
	}
	Super::PhysCustom(deltaTime, Iterations);
}

