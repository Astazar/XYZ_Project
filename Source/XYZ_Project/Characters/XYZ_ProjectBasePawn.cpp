// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZ_ProjectBasePawn.h"
#include <Components/SphereComponent.h>
#include <GameFramework/FloatingPawnMovement.h>
#include <Engine/CollisionProfile.h>
#include "XYZ_Project/Components/MovementComponents/XYZBasePawnMovementComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Camera/PlayerCameraManager.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Components/ArrowComponent.h>

// Sets default values
AXYZ_ProjectBasePawn::AXYZ_ProjectBasePawn()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetSphereRadius(CollisionSphereRadius);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	RootComponent=CollisionComponent;

	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent,UXYZBasePawnMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->SetUpdatedComponent(CollisionComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->bUsePawnControlRotation = 1;
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);

#if	WITH_EDITORONLY_DATA
	ArrowComponent=CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(RootComponent);
#endif

}


// Called to bind functionality to input
void AXYZ_ProjectBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Turn",this,&APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("MoveRight",this, &AXYZ_ProjectBasePawn::MoveRight);
	PlayerInputComponent->BindAxis("MoveForward", this, &AXYZ_ProjectBasePawn::MoveForward);
	PlayerInputComponent->BindAction("Jump",EInputEvent::IE_Pressed,this, &AXYZ_ProjectBasePawn::Jump);
}

void AXYZ_ProjectBasePawn::MoveRight(float value)
{
	InputRight = value;
	if (value != 0.0f)
	{
		AddMovementInput(CurrentViewActor->GetActorRightVector(), value);
	}
}

void AXYZ_ProjectBasePawn::MoveForward(float value)
{
	InputForward = value;
	if (value != 0.0f)
	{
		AddMovementInput(CurrentViewActor->GetActorForwardVector(), value);
	}
}

void AXYZ_ProjectBasePawn::Jump()
{
	checkf(MovementComponent->IsA<UXYZBasePawnMovementComponent>(),TEXT("AXYZ_ProjectBasePawn::Jump can work only with UXYZBasePawnMovementComponent"));
	UXYZBasePawnMovementComponent* BaseMovement = StaticCast<UXYZBasePawnMovementComponent*>(MovementComponent);
	BaseMovement->JumpStart();
}

void AXYZ_ProjectBasePawn::BeginPlay()
{
	Super::BeginPlay();
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(),0);
	CurrentViewActor = CameraManager->GetViewTarget();
	CameraManager->OnBlendComplete().AddUFunction(this, FName("OnBlendComplete"));

}

void AXYZ_ProjectBasePawn::OnBlendComplete()
{
	CurrentViewActor = GetController()->GetViewTarget();
}

