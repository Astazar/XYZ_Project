// Fill out your copyright notice in the Description page of Project Settings.


#include "Ladder.h"
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/InstancedStaticMeshComponent.h>
#include <Components/BoxComponent.h>
#include <XYZ_Project/XYZ_ProjectTypes.h>

ALadder::ALadder()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LadderRoot"));

	LeftRailMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftRail"));
	LeftRailMeshComponent->SetupAttachment(RootComponent);

	RightRailMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightRail"));
	RightRailMeshComponent->SetupAttachment(RootComponent);

	StepsMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Steps"));
	StepsMeshComponent->SetupAttachment(RootComponent);

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);
}

void ALadder::OnConstruction(const FTransform& Transform)
{
	LeftRailMeshComponent->SetRelativeLocation(FVector(0.0f, LadderWidth * 0.5f, LadderHeight * 0.5));
	RightRailMeshComponent->SetRelativeLocation(FVector(0.0f, -LadderWidth * 0.5, LadderHeight * 0.5));

	UStaticMesh* LeftRailMesh = LeftRailMeshComponent->GetStaticMesh();
	if (IsValid(LeftRailMesh))
	{
		float MeshHeight = LeftRailMesh->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			LeftRailMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, LadderHeight / MeshHeight));
		}
	}

	UStaticMesh* RightRailMesh = RightRailMeshComponent->GetStaticMesh();
	if (IsValid(RightRailMesh))
	{
		float MeshHeight = RightRailMesh->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			RightRailMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, LadderHeight / MeshHeight));
		}
	}

	UStaticMesh* StepsMesh = StepsMeshComponent->GetStaticMesh();
	if (IsValid(StepsMesh))
	{
		float MeshWidth = StepsMesh->GetBoundingBox().GetSize().Y;
		if (!FMath::IsNearlyZero(MeshWidth))
		{
			StepsMeshComponent->SetRelativeScale3D(FVector(1.0f, LadderWidth / MeshWidth, 1.0f));
		}
	}

	StepsMeshComponent->ClearInstances();

	uint32 StepsCount = FMath::FloorToInt(LadderHeight - BottomStepOffset) / StepsInterval;
	for (uint32 i = 0; i < StepsCount; i++)
	{
		FTransform InstanceTransform(FVector(0.0f, 0.0f, BottomStepOffset + i * StepsInterval));
		StepsMeshComponent->AddInstance(InstanceTransform);
	}

	float BoxDepthExtent = GetLadderInteractionBox()->GetUnscaledBoxExtent().X;
	GetLadderInteractionBox()->SetBoxExtent(FVector(BoxDepthExtent, LadderWidth * 0.5, LadderHeight * 0.5));
	GetLadderInteractionBox()->SetRelativeLocation(FVector(BoxDepthExtent, 0 , LadderHeight * 0.5));
}

UBoxComponent* ALadder::GetLadderInteractionBox() const
{
	return StaticCast<UBoxComponent*>(InteractionVolume);
}
