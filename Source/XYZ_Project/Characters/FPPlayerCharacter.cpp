// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerCharacter.h"
#include <Camera/CameraComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Components/CapsuleComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include "XYZ_Project/XYZ_ProjectTypes.h"
#include "Controllers/XYZPlayerController.h"
#include "XYZ_Project/Components/MovementComponents/XYZBaseMovementComponent.h"
#include <Actors/Interactive/Environment/Zipline.h>



AFPPlayerCharacter::AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonSkeletalMesh"));
	FirstPersonMeshComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
	FirstPersonMeshComponent->CastShadow = false;
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent, SocketFPCamera);
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->bCastHiddenShadow = true;

	CameraComponent->bAutoActivate = false;
	SpringArmComponent->bAutoActivate = false;
	SpringArmComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	bUseControllerRotationYaw = true;
}

void AFPPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode /*= 0*/)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if (GetCharacterMovementComponent()->IsOnLadder())
	{
		SetOnLadderViewLimits();
	}
	else if (PrevMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Ladder)
	{
		ResetViewLimits();
	}

	if (GetCharacterMovementComponent()->IsZiplining())
	{
		SetOnZiplineViewLimits();
	}
	else if (PrevMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Zipline)
	{
		ResetViewLimits();
	}
}

void AFPPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	XYZPlayerController = Cast<AXYZPlayerController>(NewController);
}

void AFPPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (IsFPMontagePlaying() && XYZPlayerController.IsValid())
	{
		FRotator TargetControlRotation = XYZPlayerController->GetControlRotation();
		TargetControlRotation.Pitch = 0.0f;
		float BlendSpeed = 30.0f;
		TargetControlRotation = FMath::RInterpTo(XYZPlayerController->GetControlRotation(), TargetControlRotation, DeltaSeconds, BlendSpeed);
		XYZPlayerController->SetControlRotation(TargetControlRotation);
	}
}

void AFPPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());
	FVector FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation();
	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z + HalfHeightAdjust;
	FirstPersonMeshComponent->SetRelativeLocation(FirstPersonMeshRelativeLocation);
}

void AFPPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());
	FVector FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation();
	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z;
	FirstPersonMeshComponent->SetRelativeLocation(FirstPersonMeshRelativeLocation);
}

void AFPPlayerCharacter::OnMantle(const FMantlingSettings* MantlingSettings, float MantlingAnimationStartTime)
{
	Super::OnMantle(MantlingSettings, MantlingAnimationStartTime);
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	if (IsValid(FPAnimInstance) && MantlingSettings->FPMantlingMontage)
	{
		if (XYZPlayerController.IsValid())
		{
			XYZPlayerController->SetIgnoreLookInput(true);
			XYZPlayerController->SetIgnoreMoveInput(true);
		}
		float MontageDuration = FPAnimInstance->Montage_Play(MantlingSettings->FPMantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingAnimationStartTime);
		GetWorld()->GetTimerManager().SetTimer(FPMontageTimer, this, &AFPPlayerCharacter::OnFPMontageTimerElapsed, MontageDuration, false);
	}
}

void AFPPlayerCharacter::SetOnLadderViewLimits()
{
	if (XYZPlayerController.IsValid())
	{
		XYZPlayerController->SetIgnoreCameraPitch(true);
		bUseControllerRotationYaw = false;
		float CharacterYaw = GetActorRotation().Yaw;
		APlayerCameraManager* CameraManager = XYZPlayerController->PlayerCameraManager;
		CameraManager->ViewPitchMin = LadderCameraMinPitch;
		CameraManager->ViewPitchMax = LadderCameraMaxPitch;
		CameraManager->ViewYawMin = LadderCameraMinYaw + CharacterYaw;
		CameraManager->ViewYawMax = LadderCameraMaxYaw + CharacterYaw;
	}
}

void AFPPlayerCharacter::SetOnZiplineViewLimits()
{
	if (XYZPlayerController.IsValid())
	{
		XYZPlayerController->SetIgnoreCameraPitch(true);
		bUseControllerRotationYaw = false;
		float CharacterYaw = GetActorRotation().Yaw;
		APlayerCameraManager* CameraManager = XYZPlayerController->PlayerCameraManager;
		CameraManager->ViewPitchMin = ZiplineCameraMinPitch;
		CameraManager->ViewPitchMax = ZiplineCameraMaxPitch;
		CameraManager->ViewYawMin = ZiplineCameraMinYaw + CharacterYaw;
		CameraManager->ViewYawMax = ZiplineCameraMaxYaw + CharacterYaw;
	}
}

void AFPPlayerCharacter::ResetViewLimits()
{
	if (XYZPlayerController.IsValid())
	{
		XYZPlayerController->SetIgnoreCameraPitch(false);
		bUseControllerRotationYaw = true;
		APlayerCameraManager* CameraManager = XYZPlayerController->PlayerCameraManager;
		APlayerCameraManager* DefaultCameraManager = CameraManager->GetClass()->GetDefaultObject<APlayerCameraManager>();
		CameraManager->ViewPitchMin = DefaultCameraManager->ViewPitchMin;
		CameraManager->ViewPitchMax = DefaultCameraManager->ViewPitchMax;
		CameraManager->ViewYawMin = DefaultCameraManager->ViewYawMin;
		CameraManager->ViewYawMax = DefaultCameraManager->ViewYawMax;
	}
}

FRotator AFPPlayerCharacter::GetViewRotation() const 
{
	FRotator Result = Super::GetViewRotation();

	if (IsFPMontagePlaying())
	{
		FRotator SocketRotation = FirstPersonMeshComponent->GetSocketRotation(SocketFPCamera);
		Result.Pitch += SocketRotation.Pitch;
		Result.Yaw = SocketRotation.Yaw;
		Result.Roll = SocketRotation.Roll;
	}
	return Result;
}

void AFPPlayerCharacter::ZiplineTurnAround()
{
	if (!(XYZBaseCharacterMovementComponent->IsZiplining() && GetAvailableZipline()->GetZiplineMovementType() == EZiplineMovementType::Climb))
	{
		return;
	}
	ResetViewLimits();
	Super::ZiplineTurnAround();
	SetOnZiplineViewLimits();
}

void AFPPlayerCharacter::HardLanded()
{
	Super::HardLanded();
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	if (!IsValid(FPAnimInstance))
	{
		return;
	}
	if (IsValid(FPHardLandingAnimMontage))
	{
		FPAnimInstance->Montage_Play(FPHardLandingAnimMontage);
	}
}

void AFPPlayerCharacter::LimitControl()
{
	Super::LimitControl();
	GetController()->SetIgnoreLookInput(true);
}

void AFPPlayerCharacter::UnlimitControl()
{
	Super::UnlimitControl();
	GetController()->SetIgnoreLookInput(false);
}

bool AFPPlayerCharacter::IsFPMontagePlaying() const
{
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	return IsValid(FPAnimInstance) && FPAnimInstance->IsAnyMontagePlaying();
}

void AFPPlayerCharacter::OnFPMontageTimerElapsed()
{
	if (XYZPlayerController.IsValid())
	{
		XYZPlayerController->SetIgnoreLookInput(false);
		XYZPlayerController->SetIgnoreMoveInput(false);
	}
}
