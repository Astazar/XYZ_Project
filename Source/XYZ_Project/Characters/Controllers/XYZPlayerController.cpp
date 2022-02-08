// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZPlayerController.h"
#include "XYZ_Project/Characters/XYZBaseCharacter.h"

void AXYZPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CachedBaseCharacter = Cast<AXYZBaseCharacter>(InPawn);
}

void AXYZPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &AXYZPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AXYZPlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &AXYZPlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &AXYZPlayerController::LookUp);
	InputComponent->BindAxis("SwimForward", this, &AXYZPlayerController::SwimForward);
	InputComponent->BindAxis("SwimRight", this, &AXYZPlayerController::SwimRight);
	InputComponent->BindAxis("SwimUp", this, &AXYZPlayerController::SwimUp);
	InputComponent->BindAxis("ClimbLadderUp", this, &AXYZPlayerController::ClimbLadderUp);
	InputComponent->BindAxis("ZiplineClimbForward", this, &AXYZPlayerController::ZiplineClimbForward);
	InputComponent->BindAction("InteractWithZipline", EInputEvent::IE_Pressed, this, &AXYZPlayerController::InteractWithZipline);
	InputComponent->BindAction("ZiplineTurnAround", EInputEvent::IE_Pressed, this, &AXYZPlayerController::ZiplineTurnAround);
	InputComponent->BindAction("InteractWithLadder", EInputEvent::IE_Pressed, this, &AXYZPlayerController::InteractWithLadder);
	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &AXYZPlayerController::Mantle);
	InputComponent->BindAction("Wallrun", EInputEvent::IE_Pressed, this, &AXYZPlayerController::Wallrun);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AXYZPlayerController::Jump);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AXYZPlayerController::ChangeCrouchState);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AXYZPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &AXYZPlayerController::StopSprint);
	InputComponent->BindAction("Crawl", EInputEvent::IE_Pressed, this, &AXYZPlayerController::ChangeCrawlState);
	InputComponent->BindAction("Slide", EInputEvent::IE_Pressed, this, &AXYZPlayerController::Slide);
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &AXYZPlayerController::PlayerStartFire);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &AXYZPlayerController::PlayerStopFire);
	InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &AXYZPlayerController::StartAiming);
	InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &AXYZPlayerController::StopAiming);
}

void AXYZPlayerController::MoveForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveForward(Value);
	}
}

void AXYZPlayerController::MoveRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveRight(Value);
	}
}

void AXYZPlayerController::Turn(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Turn(Value);
	}
}

void AXYZPlayerController::LookUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUp(Value);
	}
}

void AXYZPlayerController::Jump()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Jump();
	}
}

void AXYZPlayerController::ChangeCrouchState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrouchState();
	}
}

void AXYZPlayerController::ChangeCrawlState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrawlState();
	}
}

void AXYZPlayerController::Slide()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Slide();
	}
}

void AXYZPlayerController::StartSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartSprint();
	}
}

void AXYZPlayerController::StopSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopSprint();
	}
}

void AXYZPlayerController::SwimForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimForward(Value);
	}
}

void AXYZPlayerController::SwimRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimRight(Value);
	}
}

void AXYZPlayerController::SwimUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimUp(Value);
	}
}

void AXYZPlayerController::Mantle()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Mantle();
	}
}

void AXYZPlayerController::ClimbLadderUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ClimbLadderUp(Value);
	}
}

void AXYZPlayerController::InteractWithLadder()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithLadder();
	}
}

void AXYZPlayerController::InteractWithZipline()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithZipline();
	}
}

void AXYZPlayerController::ZiplineClimbForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ZiplineClimbForward(Value);
	}
}

void AXYZPlayerController::ZiplineTurnAround()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ZiplineTurnAround();
	}
}

void AXYZPlayerController::Wallrun()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Wallrun();
	}
}

void AXYZPlayerController::PlayerStartFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartFire();
	}
}

void AXYZPlayerController::PlayerStopFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopFire();
	}
}

void AXYZPlayerController::StartAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartAiming();
	}
}

void AXYZPlayerController::StopAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopAiming();
	}
}
