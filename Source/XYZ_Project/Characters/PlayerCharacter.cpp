// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <Components/CapsuleComponent.h>
#include "XYZ_Project/Components/MovementComponents/XYZBaseMovementComponent.h"
#include <Actors/Equipment/Weapons/RangeWeaponItem.h>
#include <Components/CharacterComponents/CharacterEquipmentComponent.h>

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) 
	:Super(ObjectInitializer)
{
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = 1;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

}


void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CameraSprintTimeline.TickTimeline(DeltaSeconds);
	CameraAimingTimeline.TickTimeline(DeltaSeconds);
}

void APlayerCharacter::MoveForward(float Value)
{
	if ((XYZBaseCharacterMovementComponent->IsMovingOnGround() || XYZBaseCharacterMovementComponent->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f) && !XYZBaseCharacterMovementComponent->IsSliding())
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if ((XYZBaseCharacterMovementComponent->IsMovingOnGround() || XYZBaseCharacterMovementComponent->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void APlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void APlayerCharacter::Jump()
{
	if (!XYZBaseCharacterMovementComponent->IsCrawling())
	{
		Super::Jump();
	}
	else
	{
		Uncrawl();
		XYZBaseCharacterMovementComponent->PreviousMovementState = EMovementState::Standing;
	}
}

void APlayerCharacter::SwimForward(float Value)
{
	
	if (XYZBaseCharacterMovementComponent->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator PitchYawRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = PitchYawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}		

void APlayerCharacter::SwimRight(float Value)
{
	if (XYZBaseCharacterMovementComponent->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::SwimUp(float Value)
{
	if (XYZBaseCharacterMovementComponent->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		AddMovementInput(FVector::UpVector, Value);
	}
}

void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset+=FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust,ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void APlayerCharacter::OnStartCrawl(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	USkeletalMeshComponent* PlayerMesh = GetMesh();
	if (PlayerMesh)
	{
		FVector& MeshRelativeLocation = PlayerMesh->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = PlayerMesh->GetRelativeLocation().Z + HalfHeightAdjust;
		MeshRelativeLocation.X = MeshRelativeLocation.X - ToHeadOffset;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
		BaseTranslationOffset.X = MeshRelativeLocation.X;
	}

	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void APlayerCharacter::OnEndCrawl(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	const APlayerCharacter* DefaultChar = GetDefault<APlayerCharacter>(GetClass());
	USkeletalMeshComponent* DefaultCharMesh = DefaultChar->GetMesh();
	USkeletalMeshComponent* PlayerMesh = GetMesh();
	if (PlayerMesh && DefaultCharMesh)
	{
		FVector& MeshRelativeLocation = PlayerMesh->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultCharMesh->GetRelativeLocation().Z;
		MeshRelativeLocation.X = DefaultCharMesh->GetRelativeLocation().X;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
		BaseTranslationOffset.X = MeshRelativeLocation.X;
	}

	SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void APlayerCharacter::OnStartSlide(float HalfHeightAdjust)
{
	USkeletalMeshComponent* PlayerMesh = GetMesh();
	if (PlayerMesh)
	{
		FVector MeshRelativeLocation = PlayerMesh->GetRelativeLocation();
		MeshRelativeLocation.Z = PlayerMesh->GetRelativeLocation().Z + HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
		PlayerMesh->SetRelativeLocation(MeshRelativeLocation);
	}

	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void APlayerCharacter::OnEndSlide(float HalfHeightAdjust)
{
	const APlayerCharacter* DefaultChar = GetDefault<APlayerCharacter>(GetClass());
	USkeletalMeshComponent* DefaultCharMesh = DefaultChar->GetMesh();
	USkeletalMeshComponent* PlayerMesh = GetMesh();
	if (PlayerMesh && DefaultCharMesh)
	{
		FVector MeshRelativeLocation = PlayerMesh->GetRelativeLocation();
		MeshRelativeLocation.Z = DefaultCharMesh->GetRelativeLocation().Z;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
		PlayerMesh->SetRelativeLocation(MeshRelativeLocation);
	}

	SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, HalfHeightAdjust);
}

float APlayerCharacter::GetDefaultFOV() const
{
	return GetController<APlayerController>()->PlayerCameraManager->DefaultFOV;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(CameraSprintTimelineCurve))
	{
		FOnTimelineFloatStatic CameraSprintTimelineUpdate;
		CameraSprintTimelineUpdate.BindUObject(this, &APlayerCharacter::UpdateCameraSprintTimeline);
		CameraSprintTimeline.AddInterpFloat(CameraSprintTimelineCurve, CameraSprintTimelineUpdate);
		CameraSprintTimeline.SetLooping(false);
		DefaultSpringArmLeght = SpringArmComponent->TargetArmLength;
	}

	if (IsValid(CameraAimingTimelineCurve))
	{
		FOnTimelineFloatStatic CameraAimingTimelineUpdate;
		CameraAimingTimelineUpdate.BindUObject(this, &APlayerCharacter::UpdateCameraAimingTimeline);
		CameraAimingTimeline.AddInterpFloat(CameraAimingTimelineCurve, CameraAimingTimelineUpdate);
		CameraSprintTimeline.SetLooping(false);
	}
}

void APlayerCharacter::OnSprintStart_Implementation()
{
	Super::OnSprintStart_Implementation();
	CameraSprintTimeline.Play();
	UE_LOG(LogTemp, Log, TEXT("APlayerCharacter::OnSprintStart_Implementation"));
}

void APlayerCharacter::OnSprintEnd_Implementation()
{
	Super::OnSprintEnd_Implementation();
	CameraSprintTimeline.Reverse();
	UE_LOG(LogTemp, Log, TEXT("APlayerCharacter::OnSprintEnd_Implementation"));
}

void APlayerCharacter::UpdateCameraSprintTimeline(const float Alpha)
{
	SpringArmComponent->TargetArmLength = UKismetMathLibrary::Lerp(DefaultSpringArmLeght, SprintSpringArmLenght, Alpha);
}

void APlayerCharacter::OnStartAimingInternal()
{
	Super::OnStartAimingInternal();
	if (IsValid(CameraAimingTimelineCurve))
	{
		CameraAimingTimeline.Play();
	}
	else
	{	
		UpdateFOV(CharacterEquipmentComponent->GetCurrentRangeWeapon()->GetAimFOV());
	}
}

void APlayerCharacter::OnStopAimingInternal()
{
	Super::OnStopAimingInternal();
	if (IsValid(CameraAimingTimelineCurve))
	{
		CameraAimingTimeline.Reverse();
	}
	else
	{
		UpdateFOV(GetDefaultFOV());
	}
}

void APlayerCharacter::UpdateCameraAimingTimeline(const float Alpha)
{
	float PlayerFOV = UKismetMathLibrary::Lerp(GetDefaultFOV(), CharacterEquipmentComponent->GetCurrentRangeWeapon()->GetAimFOV(), Alpha);
	UpdateFOV(PlayerFOV);
}

void APlayerCharacter::UpdateFOV(float NewFOV)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (!IsValid(PlayerController))
	{
		return;
	}
	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if (IsValid(CameraManager))
	{
		CameraManager->SetFOV(NewFOV);
	}
}

