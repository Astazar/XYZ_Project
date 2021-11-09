// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <Components/CapsuleComponent.h>
#include "../Components/MovementComponents/XYZBaseMovementComponent.h"

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
}

void APlayerCharacter::MoveForward(float Value)
{
	if (!FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (!FMath::IsNearlyZero(Value, 1e-6f))
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



