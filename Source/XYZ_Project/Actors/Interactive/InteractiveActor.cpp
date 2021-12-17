// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveActor.h"
#include "XYZ_Project/Characters/XYZBaseCharacter.h"
#include <Components/CapsuleComponent.h>

void AInteractiveActor::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(InteractionVolume))
	{
		InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveActor::OnInteractionVolumeBeginOverlap);
		InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &AInteractiveActor::OnInteractionVolumeEndOverlap);
	}
}

void AInteractiveActor::OnInteractionVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AXYZBaseCharacter* BaseCharacter = Cast<AXYZBaseCharacter>(OtherActor);
	if (IsValid(BaseCharacter))
	{
		return;
	}

	if (Cast<UCapsuleComponent>(OtherComp) != BaseCharacter->GetCapsuleComponent())
	{
		return;
	}
	
	BaseCharacter->RegisterInteractiveActor(this);
}

void AInteractiveActor::OnInteractionVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AXYZBaseCharacter* BaseCharacter = Cast<AXYZBaseCharacter>(OtherActor);
	if (IsValid(BaseCharacter))
	{
		return;
	}

	if (Cast<UCapsuleComponent>(OtherComp) != BaseCharacter->GetCapsuleComponent())
	{
		return;
	}

	BaseCharacter->UnregisterInteractiveActor(this);
}

