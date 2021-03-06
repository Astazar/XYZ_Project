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
#include "Actors/Equipment/Throwables/ThrowableItem.h"
#include "Actors/Interactive/Interface/Interactable.h"
#include <AIController.h>
#include <GameFramework/PhysicsVolume.h>
#include <Net/UnrealNetwork.h>
#include <Components/WidgetComponent.h>
#include "UI/Widgets/World/XYZAttributeProgressBar.h"



AXYZBaseCharacter::AXYZBaseCharacter(const FObjectInitializer& ObjectInitializer)	
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UXYZBaseMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	XYZBaseCharacterMovementComponent = StaticCast<UXYZBaseMovementComponent*>(GetCharacterMovement());
	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));
	CharacterAttributesComponent = CreateDefaultSubobject<UCharacterAttributesComponent>(TEXT("CharacterAttributes"));
	CharacterEquipmentComponent = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("EquipmentComponent"));
	HealthBarProgressComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarProgressComponent"));
	HealthBarProgressComponent->SetupAttachment(GetCapsuleComponent());

	IKScale = GetActorScale().Z;
	IKTraceDistance = (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + UnderFeetTraceLenght)/IKScale;

	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;
}

void AXYZBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AXYZBaseCharacter, bIsMantling);
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
	TraceLineOfSight();
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
		bIsMantling = true;

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

		if (IsLocallyControlled() || GetLocalRole() == ROLE_Authority)
		{
			XYZBaseCharacterMovementComponent->StartMantle(MantlingParameters);
		}

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

void AXYZBaseCharacter::OnRep_IsMantling(bool bWasMantling)
{
	if (GetLocalRole() == ROLE_SimulatedProxy && !bWasMantling && bIsMantling)
	{
		Mantle(true);
	}
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
	//in meters
	float FallHeight = (CurrentFallApex - GetActorLocation()).Z * 0.01;
	if (IsValid(FallDamageCurve))
	{
		float DamageAmount = FallDamageCurve->GetFloatValue(FallHeight);
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.Actor.Get());
	}
	APhysicsVolume* Volume = Cast<APhysicsVolume>(Hit.Actor);
	bool bIsWater = Volume != nullptr ? Volume->bWaterVolume : false;
	if (FallHeight >= HardLandingHeight && !bIsWater)
	{
		HardLanded();
	}
}

void AXYZBaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	CurrentFallApex = GetActorLocation();
}

void AXYZBaseCharacter::HardLanded()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		return;
	}
	if (IsValid(HardLandingAnimMontage))
	{
		float Duration = AnimInstance->Montage_Play(HardLandingAnimMontage, 1.0f, EMontagePlayReturnType::Duration);
		LimitControl();
		GetWorld()->GetTimerManager().SetTimer(HardLandingTimer, this, &AXYZBaseCharacter::UnlimitControl, Duration, false);
	}
}

void AXYZBaseCharacter::LimitControl()
{
	GetController()->SetIgnoreMoveInput(true);
}

void AXYZBaseCharacter::UnlimitControl()
{
	GetController()->SetIgnoreMoveInput(false);
}

void AXYZBaseCharacter::SelectMovementSettings(bool bEnableFreeLook)
{
	XYZBaseCharacterMovementComponent->SelectMovementSettings(bEnableFreeLook);
}

void AXYZBaseCharacter::AddEquipmentItem(const TSubclassOf<class AEquipableItem> EquipableItemClass)
{
	CharacterEquipmentComponent->AddEquipmentItem(EquipableItemClass);
}

FGenericTeamId AXYZBaseCharacter::GetGenericTeamId() const
{
	return FGenericTeamId((uint8)Team);
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
	InitializeHealthProgress();
}

void AXYZBaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (OnInteractableObjectFound.IsBound())
	{
		OnInteractableObjectFound.Unbind();
	}
	Super::EndPlay(EndPlayReason);
}

void AXYZBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AAIController* AIController = Cast<AAIController>(NewController);
	if (IsValid(AIController))
	{
		FGenericTeamId TeamID((uint8)Team);
		AIController->SetGenericTeamId(TeamID);
	}
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

FRotator AXYZBaseCharacter::GetAimOffsetLocal()
{
	FVector AimDirectionWorld = GetBaseAimRotation().Vector();
	FVector AimDirectionLocal = GetTransform().InverseTransformVectorNoScale(AimDirectionWorld);
	return AimDirectionLocal.ToOrientationRotator();
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

void AXYZBaseCharacter::TraceLineOfSight()
{
	if (!IsPlayerControlled())
	{
		return;
	}

	FVector ViewLocation;
	FRotator ViewRotation;

	APlayerController* PlayerController = GetController<APlayerController>();
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation); 
	FVector ViewDirection = ViewRotation.Vector();
	FVector TraceEnd = ViewLocation + ViewDirection * LineOfSightDistance;

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, TraceEnd, ECC_Visibility);
	if (LineOfSightObject.GetObject() != HitResult.Actor)
	{
		LineOfSightObject = HitResult.Actor.Get();
		FName ActionName = LineOfSightObject.GetInterface() ? LineOfSightObject->GetActionEventName() : FName(NAME_None);
		OnInteractableObjectFound.ExecuteIfBound(ActionName);
	}
}

void AXYZBaseCharacter::InitializeHealthProgress()
{
	UXYZAttributeProgressBar* ProgressBarWidget = Cast<UXYZAttributeProgressBar>(HealthBarProgressComponent->GetUserWidgetObject());
	if (!IsValid(ProgressBarWidget))
	{
		HealthBarProgressComponent->SetVisibility(false);
		return;
	}

	if (IsPlayerControlled() && IsLocallyControlled())
	{
		HealthBarProgressComponent->SetVisibility(false);
	}

	CharacterAttributesComponent->OnCurrentHealthChangedEvent.AddUObject(ProgressBarWidget, &UXYZAttributeProgressBar::OnProgressChanged);
	CharacterAttributesComponent->OnDeathEvent.AddLambda([=]() { HealthBarProgressComponent->SetVisibility(false);});
	ProgressBarWidget->SetProgressPercentage(CharacterAttributesComponent->GetCurrentHealthPercent());
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
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void AXYZBaseCharacter::NextWeaponBarell()
{
	CharacterEquipmentComponent->NextWeaponBarell();
}

void AXYZBaseCharacter::ThrowPrimaryItem(bool bShouldEquip)
{
	AThrowableItem* ThrowableItem = Cast<AThrowableItem>(CharacterEquipmentComponent->GetItemInSlot(EEquipmentSlots::PrimaryItemSlot));
	if (!IsValid(ThrowableItem) || CharacterEquipmentComponent->IsEquipping())
	{
		return;
	}

	if (bShouldEquip)
	{
		if (CharacterEquipmentComponent->GetCurrentEquippedSlot() != EEquipmentSlots::PrimaryItemSlot)
		{
			CharacterEquipmentComponent->OnEquipAnimationFinished.BindUObject(ThrowableItem, &AThrowableItem::Throw);
			CharacterEquipmentComponent->EquipItemInSlot(EEquipmentSlots::PrimaryItemSlot);
			CharacterEquipmentComponent->SetShouldEquipPrevious(true);
		}
		else
		{
			ThrowableItem->Throw();
		}
	}
	else if(CharacterEquipmentComponent->GetCurrentEquippedSlot() == EEquipmentSlots::PrimaryItemSlot)
	{
		ThrowableItem->Throw();
	}
}

void AXYZBaseCharacter::OnStartThrow()
{
	CharacterEquipmentComponent->OnEquipAnimationFinished.Unbind();
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

void AXYZBaseCharacter::Interact()
{
	if (LineOfSightObject.GetInterface())
	{
		LineOfSightObject->Interact(this);
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
