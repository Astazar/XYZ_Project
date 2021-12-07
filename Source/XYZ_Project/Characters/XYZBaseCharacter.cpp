// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZBaseCharacter.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "../Components/MovementComponents/XYZBaseMovementComponent.h"
#include <Components/CapsuleComponent.h>
#include <Kismet/KismetSystemLibrary.h>
#include "../Components/LedgeDetectorComponent.h"



AXYZBaseCharacter::AXYZBaseCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UXYZBaseMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	XYZBaseCharacterMovementComponent = StaticCast<UXYZBaseMovementComponent*>(GetCharacterMovement());
	IKScale = GetActorScale().Z;
	IKTraceDistance = (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + UnderFeetTraceLenght)/IKScale;
	
	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));
}

void AXYZBaseCharacter::ChangeCrouchState()
{
	if (!XYZBaseCharacterMovementComponent->IsCrouching() && !XYZBaseCharacterMovementComponent->IsCrawling())
	{
		Crouch();
	}
	else if (XYZBaseCharacterMovementComponent->IsCrawling() && !XYZBaseCharacterMovementComponent->IsCrouching()) 
	{
		if (XYZBaseCharacterMovementComponent->IsEnoughSpaceToUncrawl())
		{
			Uncrawl();
			Crouch();
		}
	}
}

void AXYZBaseCharacter::ChangeCrawlState()
{
	if (!XYZBaseCharacterMovementComponent->IsCrawling() && XYZBaseCharacterMovementComponent->IsCrouching())
	{
		if(XYZBaseCharacterMovementComponent->PreviousMovementState == EMovementState::Standing)
		{ 
			UnCrouch();
			Crawl();
			XYZBaseCharacterMovementComponent->PreviousMovementState = EMovementState::Crawling;
		}
		else if (XYZBaseCharacterMovementComponent->PreviousMovementState == EMovementState::Crawling)
		{
			if (XYZBaseCharacterMovementComponent->IsEnoughSpaceToUncrouch())
			{
				UnCrouch();
				XYZBaseCharacterMovementComponent->PreviousMovementState = EMovementState::Standing;
			}
		}
	}


}

void AXYZBaseCharacter::Crawl()
{
	XYZBaseCharacterMovementComponent->bWantsToCrawl=true;
}

void AXYZBaseCharacter::Uncrawl()
{
	XYZBaseCharacterMovementComponent->bWantsToCrawl=false;
}

void AXYZBaseCharacter::OnJumped_Implementation()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

bool AXYZBaseCharacter::CanJumpInternal_Implementation() const
{
	return (bIsCrouched || Super::CanJumpInternal_Implementation()) && (XYZBaseCharacterMovementComponent->IsEnoughSpaceToUncrouch()) && !XYZBaseCharacterMovementComponent->GetIsOutOfStamina() && !XYZBaseCharacterMovementComponent->IsMantling();
}

void AXYZBaseCharacter::StartSprint()
{
	bIsSprintRequested = true;
	if (bIsCrouched)
	{
		UnCrouch();
		XYZBaseCharacterMovementComponent->PreviousMovementState = EMovementState::Standing;
	}
}

void AXYZBaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
}

void AXYZBaseCharacter::UpdateStamina(float DeltaSeconds)
{
	if (!XYZBaseCharacterMovementComponent->IsSprinting())
	{
		CurrentStamina += StaminaRestoreVelocity * DeltaSeconds;
		CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
		if (CurrentStamina == MaxStamina)
		{
			XYZBaseCharacterMovementComponent->SetIsOutOfStamina(false);
		}
	}
	if (XYZBaseCharacterMovementComponent->IsSprinting())
	{
		CurrentStamina -= SprintStaminaConsumptionVelocity * DeltaSeconds;
		CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
		if (CurrentStamina == 0.0f)
		{
			XYZBaseCharacterMovementComponent->SetIsOutOfStamina(true);
		}
	}
}

void AXYZBaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateStamina(DeltaSeconds);

	TryChangeSprintState(DeltaSeconds);
	UpdateIKOffsets(DeltaSeconds);
	GEngine->AddOnScreenDebugMessage(-1, 0 , FColor::Orange,FString::Printf(TEXT("Stamina: %f"),CurrentStamina));
}


void AXYZBaseCharacter::Mantle()
{
	FLedgeDescription LedgeDescription;
	bool IsDetected = LedgeDetectorComponent->DetectLedge(LedgeDescription);
	if (IsDetected)
	{
		FMantlingMovementParameters MantlingParameters;
		MantlingParameters.InitialLocation = GetActorLocation();
		MantlingParameters.InitialRotation = GetActorRotation();
		MantlingParameters.TargetLocation = LedgeDescription.Location;
		MantlingParameters.TargetRotation = LedgeDescription.Rotation;

		float MantlingHeight = (MantlingParameters.TargetLocation - MantlingParameters.InitialLocation).Z;
		const FMantlingSettings& MantlingSettings = GetMantlingSettings(MantlingHeight);

		float MinRange, MaxRange; 
		MantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);
		MantlingParameters.Duration = MaxRange - MinRange;
		MantlingParameters.MantlingCurve = MantlingSettings.MantlingCurve;

		FVector2D SourceRange(MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		FVector2D TargetRange(MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStartTime);
		MantlingParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);
		MantlingParameters.InitialAnimationLocation = MantlingParameters.TargetLocation - MantlingSettings.AnimationCorrectionZ * FVector::UpVector + MantlingSettings.AnimationCorrectionXY * LedgeDescription.LedgeNormal;

		XYZBaseCharacterMovementComponent->StartMantle(MantlingParameters);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingParameters.StartTime);
	}
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString::Printf(TEXT("Can Mantle:%s"), IsDetected ? TEXT("true") : TEXT("false")));
}

void AXYZBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentStamina=MaxStamina;
}

void AXYZBaseCharacter::OnSprintStart_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("AXYZBaseCharacter::OnSprintStart_Implementation"));
}

void AXYZBaseCharacter::OnSprintEnd_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("AXYZBaseCharacter::OnSprintEnd_Implementation"));
}

bool AXYZBaseCharacter::CanSprint()
{
	return (XYZBaseCharacterMovementComponent->Velocity != FVector::ZeroVector) && !XYZBaseCharacterMovementComponent->GetIsOutOfStamina() && !XYZBaseCharacterMovementComponent->IsCrawling();
}

void AXYZBaseCharacter::UpdateIKOffsets(float DeltaSeconds)
{
	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, GetIKOffsetForASocket(RightFootSocketName), DeltaSeconds, IKInterpSpeed);
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, GetIKOffsetForASocket(LeftFootSocketName), DeltaSeconds, IKInterpSpeed);
	IKPelvisOffset = FMath::FInterpTo(IKPelvisOffset,CalculateIKPelvisOffset(),DeltaSeconds,IKInterpSpeed);
}

void AXYZBaseCharacter::TryChangeSprintState(float DeltaSeconds)
{
	if (bIsSprintRequested && !XYZBaseCharacterMovementComponent->IsSprinting() && CanSprint())
	{
		XYZBaseCharacterMovementComponent->StartSprint();
		OnSprintStart();
	}
	if (!bIsSprintRequested && XYZBaseCharacterMovementComponent->IsSprinting())
	{
		XYZBaseCharacterMovementComponent->StopSprint();
		OnSprintEnd();
	}
}

float AXYZBaseCharacter::GetIKOffsetForASocket(const FName& SocketName)
{
	float Result = 0;
	FVector SocketLocation = GetMesh()->GetSocketLocation(SocketName);
	FVector TraceStart(SocketLocation.X,SocketLocation.Y, GetActorLocation().Z);
	FVector TraceEnd = TraceStart - IKTraceDistance * FVector::UpVector;
	float CapsuleHalfLenght = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	float StandartHigh = TraceStart.Z - CapsuleHalfLenght;
	FHitResult HitResult;
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	const FVector FootBox = FVector(1.0f, 15.0f, 7.0f);
	if(UKismetSystemLibrary::BoxTraceSingle(GetWorld(),TraceStart, TraceEnd, FootBox, GetMesh()->GetSocketRotation(SocketName), TraceType, true, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true))
	{
		Result = (StandartHigh - HitResult.Location.Z) / IKScale;
	}
	return Result;
}

float AXYZBaseCharacter::CalculateIKPelvisOffset()
{
	float OffsetTernary;
	OffsetTernary = IKRightFootOffset > IKLeftFootOffset ? -IKRightFootOffset : -IKLeftFootOffset;

	#if UE_BUILD_DEBUG
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Orange, FString::Printf(TEXT("Right Offset: %f \t Left Offset: %f"), IKRightFootOffset, IKLeftFootOffset));
	float OffsetAbs;
	OffsetAbs = -FMath::Abs(IKRightFootOffset - IKLeftFootOffset);
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Purple, FString::Printf(TEXT("OffsetAbs: %f"), OffsetAbs));
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Purple, FString::Printf(TEXT("OffsetTernary: %f"), OffsetTernary));
	#endif

	return OffsetTernary;
}

const FMantlingSettings& AXYZBaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	return LedgeHeight > LowMantleMaxHeight ? HighMantleSettings : LowMantleSettings;
}
