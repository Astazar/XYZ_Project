// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZBaseCharacter.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "XYZ_Project/Components/MovementComponents/XYZBaseMovementComponent.h"
#include <Components/CapsuleComponent.h>
#include <Kismet/KismetSystemLibrary.h>
#include "XYZ_Project/Components/LedgeDetectorComponent.h"
#include "XYZ_Project/Actors/Interactive/Environment/Ladder.h"
#include "XYZ_Project/Actors/Interactive/Environment/Zipline.h"
#include <DrawDebugHelpers.h>
#include "XYZ_Project/XYZ_ProjectTypes.h"
#include <Kismet/GameplayStatics.h>
#include "XYZ_Project/Subsystems/DebugSubsystem.h"
#include "XYZ_Project/Components/CharacterComponents/CharacterAttributesComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include <Actors/Equipment/Weapons/RangeWeaponItem.h>
#include "Actors/Equipment/Weapons/MeleeWeaponItem.h"


AXYZBaseCharacter::AXYZBaseCharacter(const FObjectInitializer& ObjectInitializer)	
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UXYZBaseMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	XYZBaseCharacterMovementComponent = StaticCast<UXYZBaseMovementComponent*>(GetCharacterMovement());
	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));
	CharacterAttributesComponent = CreateDefaultSubobject<UCharacterAttributesComponent>(TEXT("CharacterAttributes"));
	CharacterEquipmentComponent = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("EquipmentComponent"));

	IKScale = GetActorScale().Z;
	IKTraceDistance = (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + UnderFeetTraceLenght)/IKScale;

	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;
}

bool AXYZBaseCharacter::CanCrouch() const
{
	return Super::CanCrouch() && !XYZBaseCharacterMovementComponent->IsSliding();
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

bool AXYZBaseCharacter::CanCrawl() const
{
	return XYZBaseCharacterMovementComponent->CanEverCrawl() && !XYZBaseCharacterMovementComponent->IsSliding();
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
	return (bIsCrouched || Super::CanJumpInternal_Implementation()) && 
	       (XYZBaseCharacterMovementComponent->IsEnoughSpaceToUncrouch()) && 
		   !XYZBaseCharacterMovementComponent->GetIsOutOfStamina() && 
		   !XYZBaseCharacterMovementComponent->IsMantling() && 
		   !XYZBaseCharacterMovementComponent->IsWallrunning() &&
		   !XYZBaseCharacterMovementComponent->IsSliding();
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


bool AXYZBaseCharacter::CanSlide()
{
	return XYZBaseCharacterMovementComponent->IsSprinting() && !XYZBaseCharacterMovementComponent->IsSliding();
}

void AXYZBaseCharacter::Slide()
{
	if (!CanSlide())
	{
		return;
	}
	XYZBaseCharacterMovementComponent->Slide();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	PlayAnimMontage(XYZBaseCharacterMovementComponent->GetSlideAnimMontage());
}

void AXYZBaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TryChangeSprintState(DeltaSeconds);
	UpdateIKOffsets(DeltaSeconds);
	UpdateOutOfOxygenDamage(DeltaSeconds);
}


void AXYZBaseCharacter::Mantle(bool bForce /*= false*/)
{
	if (!(CanMantle() || bForce))
	{
		return;
	}

	ACharacter* DefaultChar = GetClass()->GetDefaultObject<ACharacter>();
	float DefaultCapsuleHalfHeight = DefaultChar->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	float CrouchedOffset=0;
	if(bIsCrouched)
	{
		UnCrouch();
		float CurrentCapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		CrouchedOffset = DefaultCapsuleHalfHeight-CurrentCapsuleHalfHeight;
	}

	FLedgeDescription LedgeDescription;
	bool IsDetected = LedgeDetectorComponent->DetectLedge(LedgeDescription);
    
	bool IsDebugEnabled = UDebugSubsystem::GetDebugSubsystem()->IsCategoryEnabled(DebugCategoryLedgeDetection);
	if(IsDebugEnabled)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString::Printf(TEXT("Can Mantle:%s"), IsDetected ? TEXT("true") : TEXT("false")));	
	}
	

	if (IsDetected)
	{
		FMantlingMovementParameters MantlingParameters;
		MantlingParameters.InitialLocation = GetActorLocation() + CrouchedOffset;
		MantlingParameters.InitialRotation = GetActorRotation();
		MantlingParameters.TargetLocation = LedgeDescription.Location;
		MantlingParameters.TargetRotation = LedgeDescription.Rotation;
		MantlingParameters.Geometry = LedgeDescription.GeometryComponent;
		MantlingParameters.InitialGeometryLocation = LedgeDescription.InitialGeometryLocation;

		float MantlingHeight = ((MantlingParameters.TargetLocation - DefaultCapsuleHalfHeight * FVector::UpVector) - (MantlingParameters.InitialLocation - DefaultCapsuleHalfHeight * FVector::UpVector)).Z;
		
		if(IsDebugEnabled)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString::Printf(TEXT("MantlingHeight:%f"), MantlingHeight));	
		}

		const FMantlingSettings* MantlingSettings = GetMantlingSettings(MantlingHeight);
		if (MantlingSettings == nullptr)
		{
			return;
		}
		float MinRange, MaxRange;
		MantlingSettings->MantlingCurve->GetTimeRange(MinRange, MaxRange);
		MantlingParameters.Duration = MaxRange - MinRange;
		MantlingParameters.MantlingCurve = MantlingSettings->MantlingCurve;

		FVector2D SourceRange(MantlingSettings->AnimMinHeight, MantlingSettings->AnimMaxHeight);
		FVector2D TargetRange(MantlingSettings->MinHeightStartTime, MantlingSettings->MaxHeightStartTime);
		MantlingParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);
		MantlingParameters.InitialAnimationLocation = MantlingParameters.TargetLocation - MantlingSettings->AnimationCorrectionZ * FVector::UpVector + MantlingSettings->AnimationCorrectionXY * LedgeDescription.LedgeNormal;

		XYZBaseCharacterMovementComponent->StartMantle(MantlingParameters);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings->MantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingParameters.StartTime);
		OnMantle(MantlingSettings, MantlingParameters.StartTime);
	}
}


void AXYZBaseCharacter::OnMantle(const FMantlingSettings* MantlingSettings, float MantlingAnimationStartTime)
{

}


bool AXYZBaseCharacter::CanMantle() const
{
	return !XYZBaseCharacterMovementComponent->IsMantling() && 
	       !XYZBaseCharacterMovementComponent->IsCrawling() && 
		   !XYZBaseCharacterMovementComponent->IsOnLadder() && 
		   !XYZBaseCharacterMovementComponent->IsWallrunning();
		   !XYZBaseCharacterMovementComponent->IsOnLadder() &&
		   !XYZBaseCharacterMovementComponent->IsSliding();
}

UXYZBaseMovementComponent* AXYZBaseCharacter::GetCharacterMovementComponent() const
{
	return XYZBaseCharacterMovementComponent;
}

void AXYZBaseCharacter::InteractWithZipline()
{
	if (XYZBaseCharacterMovementComponent->IsZiplining())
	{
		XYZBaseCharacterMovementComponent->DetachFromZipline();
		return;
	}
	const AZipline* AvailableZipline = GetAvailableZipline();
	if (IsValid(AvailableZipline))
	{
		XYZBaseCharacterMovementComponent->AttachToZipline(AvailableZipline);
	}

}

void AXYZBaseCharacter::ZiplineClimbForward(float Value)
{
	if (XYZBaseCharacterMovementComponent->IsZiplining() && !FMath::IsNearlyZero(Value, 1e-6f) && GetAvailableZipline()->GetZiplineMovementType() == EZiplineMovementType::Climb)
	{
		XYZBaseCharacterMovementComponent->ZiplineClimbForward(Value);
	}
}

void AXYZBaseCharacter::ZiplineTurnAround()
{
	if (XYZBaseCharacterMovementComponent->IsZiplining() && GetAvailableZipline()->GetZiplineMovementType() == EZiplineMovementType::Climb)
	{
		XYZBaseCharacterMovementComponent->ZiplineTurnAround();
	}
}

const class AZipline* AXYZBaseCharacter::GetAvailableZipline() const
{
	const AZipline* Result = nullptr;
	for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	{
		if (InteractiveActor->IsA<AZipline>())
		{
			Result = StaticCast<const AZipline*>(InteractiveActor);
			break;
		}
	}
	return Result;
}

void AXYZBaseCharacter::Wallrun()
{
	if (!CanWallrun())
	{
		return; 
	}
	if (XYZBaseCharacterMovementComponent->IsWallrunning())
	{
		XYZBaseCharacterMovementComponent->JumpOffWall();
		return;
	}
	XYZBaseCharacterMovementComponent->Wallrun();

}

bool AXYZBaseCharacter::CanWallrun()
{
	return !XYZBaseCharacterMovementComponent->IsMantling() && 
		    XYZBaseCharacterMovementComponent->MovementMode != MOVE_Walking && 
		    XYZBaseCharacterMovementComponent->MovementMode != MOVE_Swimming;
}


void AXYZBaseCharacter::Falling()
{
	GetCharacterMovement()->bNotifyApex = true;
}

void AXYZBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	float FallHeight = (CurrentFallApex - GetActorLocation()).Z * 0.01;
	if (IsValid(FallDamageCurve))
	{
		float DamageAmount = FallDamageCurve->GetFloatValue(FallHeight);
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.Actor.Get());
	}
}

void AXYZBaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	CurrentFallApex = GetActorLocation();
}

const UCharacterEquipmentComponent* AXYZBaseCharacter::GetCharacterEquipmentComponent() const
{
	return CharacterEquipmentComponent;
}

UCharacterEquipmentComponent* AXYZBaseCharacter::GetCharacterEquipmentComponent_Mutable() const
{
	return CharacterEquipmentComponent;
}

void AXYZBaseCharacter::RegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.AddUnique(InteractiveActor);
}

void AXYZBaseCharacter::UnregisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.RemoveSingleSwap(InteractiveActor);
}

bool AXYZBaseCharacter::CanInteractWithLadder()
{
	return !XYZBaseCharacterMovementComponent->IsSprinting() &&
		   !XYZBaseCharacterMovementComponent->IsSliding();
}

void AXYZBaseCharacter::ClimbLadderUp(float Value)
{
	if (XYZBaseCharacterMovementComponent->IsOnLadder() && !FMath::IsNearlyZero(Value))
	{
		FVector LadderUpVector = XYZBaseCharacterMovementComponent->GetCurrentLadder()->GetActorUpVector();
		AddMovementInput(LadderUpVector, Value);
	}
}

void AXYZBaseCharacter::InteractWithLadder()
{
	if (!CanInteractWithLadder())
	{
		return;
	}
	if (XYZBaseCharacterMovementComponent->IsOnLadder())
	{
		XYZBaseCharacterMovementComponent->DetachFromLadder(EDetachFromLadderMethod::JumpOff);
	}
	else
	{
		const ALadder* AvailableLadder = GetAvailableLadder();
		if (IsValid(AvailableLadder))
		{
			if (AvailableLadder->GetIsOnTop())
			{
				PlayAnimMontage(AvailableLadder->GetAttachFromTopAnimMontage());
			}
			XYZBaseCharacterMovementComponent->AttachToLadder(AvailableLadder);
		}
	}
	
}

const class ALadder* AXYZBaseCharacter::GetAvailableLadder() const
{
	const ALadder* Result = nullptr;
	for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	{
		if (InteractiveActor->IsA<ALadder>())
		{
			Result = StaticCast<const ALadder*>(InteractiveActor);
			break;
		}
	}
	return Result;
}

void AXYZBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	CharacterAttributesComponent->OnDeathEvent.AddUObject(this, &AXYZBaseCharacter::OnDeath);
	CharacterAttributesComponent->OutOfStaminaEvent.AddUObject(XYZBaseCharacterMovementComponent, &UXYZBaseMovementComponent::SetIsOutOfStamina);
}

void AXYZBaseCharacter::StartFire()
{
	UE_LOG(LogTemp, Warning, TEXT("AXYZBaseCharacter::StartFire()"));
	if (CharacterEquipmentComponent->IsEquipping())
	{
		return;
	}
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StartFire();
	}
}

void AXYZBaseCharacter::StopFire()
{
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopFire();
	}
}

void AXYZBaseCharacter::Reload()
{
	if (IsValid(CharacterEquipmentComponent->GetCurrentRangeWeapon()))
	{
		CharacterEquipmentComponent->ReloadCurrentWeapon();
	}
}

void AXYZBaseCharacter::StartAiming()
{
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if (!IsValid(CurrentRangeWeapon))
	{
		return;
	}

	bIsAiming = true;
	CurrentAimingMovementSpeed = CurrentRangeWeapon->GetAimMovementMaxSpeed();
	CurrentRangeWeapon->StartAim();
	OnStartAiming();
}

void AXYZBaseCharacter::StopAiming()
{
	if (!bIsAiming)
	{
		return;
	}

	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopAim();
	}

	bIsAiming = false;
	CurrentAimingMovementSpeed = 0.0f;
	OnStopAiming();
}

bool AXYZBaseCharacter::IsAiming() const
{
	return bIsAiming;
}

float AXYZBaseCharacter::GetAimingMovementSpeed() const
{
	return CurrentAimingMovementSpeed;
}

void AXYZBaseCharacter::OnStartAiming_Implementation()
{
	OnStartAimingInternal();
}

void AXYZBaseCharacter::OnStopAiming_Implementation()
{
	OnStopAimingInternal();
}


void AXYZBaseCharacter::OutOfOxygenTakeDamage()
{
	TakeDamage(OutOfOxygenDamage, FDamageEvent(), GetController(), nullptr);
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
	return (XYZBaseCharacterMovementComponent->Velocity != FVector::ZeroVector) && 
		   !XYZBaseCharacterMovementComponent->GetIsOutOfStamina() && 
		   !XYZBaseCharacterMovementComponent->IsCrawling() && 
		   !XYZBaseCharacterMovementComponent->IsZiplining() && 
		   !XYZBaseCharacterMovementComponent->IsMantling() && 
		   !XYZBaseCharacterMovementComponent->IsOnLadder() && 
		   (XYZBaseCharacterMovementComponent->MovementMode != MOVE_Falling) &&
		   !XYZBaseCharacterMovementComponent->IsWallrunning(); 
}

void AXYZBaseCharacter::OnDeath()
{
	GetCharacterMovement()->DisableMovement();
	if (IsValid(OnDeathAnimMontage) && GetCharacterMovementComponent()->MovementMode == MOVE_Walking)
	{
		PlayAnimMontage(OnDeathAnimMontage);
	}
	else
	{
		EnableRagdoll();
	}
}

void AXYZBaseCharacter::OnStartAimingInternal()
{
	if (OnAmimingStateChanged.IsBound())
	{
		OnAmimingStateChanged.Broadcast(true);
	}
}

void AXYZBaseCharacter::OnStopAimingInternal()
{
	if (OnAmimingStateChanged.IsBound())
	{
		OnAmimingStateChanged.Broadcast(false);
	}
}

void AXYZBaseCharacter::NextItem()
{
	CharacterEquipmentComponent->EquipNextItem();
}

void AXYZBaseCharacter::PreviousItem()
{
	CharacterEquipmentComponent->EquipPreviousItem();
}

void AXYZBaseCharacter::EquipPrimaryItem()
{
	CharacterEquipmentComponent->EquipItemInSlot(EEquipmentSlots::PrimaryItemSlot);
}

void AXYZBaseCharacter::PrimaryMeleeAttack()
{
	AMeleeWeaponItem* CurrentMeleeWeapon = CharacterEquipmentComponent->GetCurrentMeleeWeapon();
	if (IsValid(CurrentMeleeWeapon))
	{
		CurrentMeleeWeapon->StartAttack(EMeleeAttackTypes::PrimaryAttack);
	}
}

void AXYZBaseCharacter::SecondaryMeleeAttack()
{
	AMeleeWeaponItem* CurrentMeleeWeapon = CharacterEquipmentComponent->GetCurrentMeleeWeapon();
	if (IsValid(CurrentMeleeWeapon))
	{
		CurrentMeleeWeapon->StartAttack(EMeleeAttackTypes::SecondaryAttack);
	}
}

void AXYZBaseCharacter::EnableRagdoll()
{
	GetMesh()->SetCollisionProfileName(CollisionProfileRagdoll);
	GetMesh()->SetSimulatePhysics(true);
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
	else if (XYZBaseCharacterMovementComponent->GetIsOutOfStamina())
	{
		XYZBaseCharacterMovementComponent->StopSprint();
		OnSprintEnd();
	}
}

void AXYZBaseCharacter::UpdateOutOfOxygenDamage(float DeltaSeconds)
{
	float CurrentOxygen = CharacterAttributesComponent->GetCurrentOxygen();
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (CurrentOxygen <= 0.0f)
	{
		if (CurrentOxygen <= 0 && !TimerManager.IsTimerActive(OutOfOxygenDamageTimer))
		{
			TimerManager.SetTimer(OutOfOxygenDamageTimer, this, &AXYZBaseCharacter::OutOfOxygenTakeDamage, OutOfOxygenDamageInterval, true);
		}

		if (TimerManager.IsTimerActive(OutOfOxygenDamageTimer) && (CharacterAttributesComponent->GetCurrentHealth() <= 0.0f || CurrentOxygen > 0.0f))
		{
			TimerManager.ClearTimer(OutOfOxygenDamageTimer);
		}
	}
	if (CurrentOxygen > 0.0f && TimerManager.IsTimerActive(OutOfOxygenDamageTimer))
	{
		TimerManager.ClearTimer(OutOfOxygenDamageTimer);
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

	float OffsetAbs;
	OffsetAbs = -FMath::Abs(IKRightFootOffset - IKLeftFootOffset);

	return OffsetTernary;
}

const FMantlingSettings* AXYZBaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	const FMantlingSettings* ResultSetting = nullptr;

	//this is the lowest of maximum applying heights
	float LowestMaxSettingApplyHeight = BIG_NUMBER;
	float SmallOfset = 3.0f;
	for (const FMantlingSettings& Setting : MantlingSettingsCollection)
	{	
		if ((LedgeHeight - SmallOfset < Setting.MaxSettingApplyHeight) && (Setting.MaxSettingApplyHeight < LowestMaxSettingApplyHeight))
		{
			LowestMaxSettingApplyHeight = Setting.MaxSettingApplyHeight;
			ResultSetting = &Setting;
		}
	}
	return ResultSetting;
}
