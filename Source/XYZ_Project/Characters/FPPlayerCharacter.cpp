// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerCharacter.h"
#include <Camera/CameraComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Components/CapsuleComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "XYZ_Project/XYZ_ProjectTypes.h"



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
