// Fill out your copyright notice in the Description page of Project Settings.


#include "Zipline.h"
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/CapsuleComponent.h>
#include "XYZ_Project/XYZ_ProjectTypes.h"
#include <Components/ArrowComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <DrawDebugHelpers.h>


AZipline::AZipline()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ZiplineRoot"));

	StartPillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StartPillar"));
	StartPillarMesh->SetupAttachment(RootComponent);

	EndPillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EndPillar"));
	EndPillarMesh->SetupAttachment(RootComponent);

	CableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cable"));
	CableMesh->SetupAttachment(RootComponent);

	InteractionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);
}

void AZipline::OnConstruction(const FTransform& Transform)
{
	UStaticMesh* StartPillar = StartPillarMesh->GetStaticMesh();
	if (IsValid(StartPillar))
	{
		float MeshHeight = StartPillar->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			StartPillarMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, StartPillarHeight/MeshHeight));
		}
	}

	UStaticMesh* EndPillar = EndPillarMesh->GetStaticMesh();
	if (IsValid(EndPillar))
	{
		float MeshHeight = EndPillar->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			EndPillarMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, EndPillarHeight / MeshHeight));
		}
	}

	StartPillarMesh->SetRelativeLocation(StartPillarLocation+StartPillarHeight * 0.5 * FVector::UpVector);
	EndPillarMesh->SetRelativeLocation(EndPillarLocation + EndPillarHeight * 0.5 * FVector::UpVector);
	StartPillarTop = StartPillarLocation + FVector::UpVector * StartPillarHeight;
	EndPillarTop = EndPillarLocation + FVector::UpVector * EndPillarHeight;

	UStaticMesh* Cable = CableMesh->GetStaticMesh();
	if (IsValid(Cable))
	{
		float MeshLenght = Cable->GetBoundingBox().GetSize().X;
		if (!FMath::IsNearlyZero(MeshLenght))
		{
			CableLenght = (StartPillarTop - EndPillarTop).Size();
			CableMesh->SetRelativeScale3D(FVector(CableLenght / MeshLenght, 1.0,  1.0f));
			UCapsuleComponent* InteractionCapsuleVolume = StaticCast<UCapsuleComponent*>(InteractionVolume);
			InteractionCapsuleVolume->SetCapsuleHalfHeight(CableLenght*0.5);
			InteractionCapsuleVolume->SetCapsuleRadius(InteractionCapsuleRadius);
		}
	}
	FVector CableLocation = (StartPillarTop+EndPillarTop)*0.5;
	CableMesh->SetRelativeLocation(CableLocation);
	FRotator CableRotation = CableMesh->GetRelativeRotation();
	CableRotation = (EndPillarTop - StartPillarTop).ToOrientationRotator();
	CableMesh->SetRelativeRotation(CableRotation);
	CableRotation.Pitch-=90;
	InteractionVolume->SetRelativeRotation(CableRotation);
	InteractionVolume->SetRelativeLocation(CableLocation);
}

float AZipline::GetCableLenght() const
{
	return CableLenght;
}

FVector AZipline::GetStartPillarTopWorldLocation() const
{
	return StartPillarMesh->GetComponentLocation() + StartPillarHeight * 0.5 * FVector::UpVector;
}

FVector AZipline::GetEndPillarTopWorldLocation() const
{
	return EndPillarMesh->GetComponentLocation() + EndPillarHeight * 0.5 * FVector::UpVector;;
}



FVector AZipline::GetCableMeshLocation() const
{
	return CableMesh->GetComponentLocation();
}


const UStaticMeshComponent* AZipline::GetCableMesh() const
{
	return CableMesh;
}

EZiplineMovementType AZipline::GetZiplineMovementType() const
{
	return ZiplineMovementType;
}

void AZipline::OnInteractionVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnInteractionVolumeBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}
}

void AZipline::OnInteractionVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnInteractionVolumeEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}
}


