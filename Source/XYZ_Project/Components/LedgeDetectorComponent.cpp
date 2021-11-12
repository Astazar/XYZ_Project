// Fill out your copyright notice in the Description page of Project Settings.


#include "LedgeDetectorComponent.h"
#include <GameFramework/Character.h>
#include <Components/CapsuleComponent.h>
#include "../XYZ_ProjectTypes.h"
#include <DrawDebugHelpers.h>




// Called when the game starts
void ULedgeDetectorComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<ACharacter>(), TEXT("ULedgeDetectorComponent::BeginPlay() only ACharacter can use ULedgeDetectorComponent"));
	CachedCharacterOwner = StaticCast<ACharacter*>(GetOwner());
	// ...
	
}

bool ULedgeDetectorComponent::DetectLedge(OUT FLedgeDescription& LedgeDescription)
{
	UCapsuleComponent* CapsuleComponent = CachedCharacterOwner->GetCapsuleComponent();

	FCollisionQueryParams QuerryParams;
	QuerryParams.bTraceComplex = true;
	QuerryParams.AddIgnoredActor(GetOwner());

	float BottomZOffset = 2.0f;
	FVector CharacterBottom = CachedCharacterOwner->GetActorLocation() - (CapsuleComponent->GetScaledCapsuleHalfHeight() -BottomZOffset) * FVector::UpVector;

	//1. Forward check 
	float ForwardCheckCapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	float ForwardCheckCapsuleHalfHeight = (MaximumLedgeHeight - MinimumLedgeHeight) * 0.5;

	FHitResult ForwardCheckHitResult;
	FCollisionShape ForwardCheckCapsuleShape = FCollisionShape::MakeCapsule(ForwardCheckCapsuleRadius,ForwardCheckCapsuleHalfHeight);
	FVector ForwardCheckStartLocation = CharacterBottom + (MinimumLedgeHeight + ForwardCheckCapsuleHalfHeight)*FVector::UpVector;
	FVector ForwardCheckEndLocation = ForwardCheckStartLocation + CachedCharacterOwner->GetActorForwardVector() * ForwardCheckDistance;

	float DrawTime = 5.0f;
	DrawDebugCapsule(GetWorld(),ForwardCheckStartLocation,ForwardCheckCapsuleHalfHeight,ForwardCheckCapsuleRadius,FQuat::Identity,FColor::Black, false, DrawTime);
	DrawDebugCapsule(GetWorld(), ForwardCheckEndLocation, ForwardCheckCapsuleHalfHeight, ForwardCheckCapsuleRadius, FQuat::Identity, FColor::Black, false, DrawTime);
	DrawDebugLine(GetWorld(),ForwardCheckStartLocation,ForwardCheckEndLocation, FColor::Black, false, DrawTime);
	if (!GetWorld()->SweepSingleByChannel(ForwardCheckHitResult, ForwardCheckStartLocation, ForwardCheckEndLocation, FQuat::Identity, ECC_Climbing, ForwardCheckCapsuleShape, QuerryParams))
	{
		return false;
	}
	DrawDebugCapsule(GetWorld(), ForwardCheckHitResult.Location, ForwardCheckCapsuleHalfHeight, ForwardCheckCapsuleRadius, FQuat::Identity, FColor::Red, false, DrawTime);
	DrawDebugPoint(GetWorld(), ForwardCheckHitResult.ImpactPoint, 10.0f, FColor::Red, false, DrawTime);

	//2. Downward check
	float DownwardCheckSphereRadius = CapsuleComponent->GetScaledCapsuleRadius();

	FHitResult DownwardCheckHitResult;
	FCollisionShape DownwardCheckSphereShape = FCollisionShape::MakeSphere(DownwardCheckSphereRadius);

	float DownwardCheckDepthOffset = 10.0f;
	FVector DownwardCheckStartLocation = ForwardCheckHitResult.ImpactPoint - ForwardCheckHitResult.ImpactNormal * DownwardCheckDepthOffset; 
	DownwardCheckStartLocation.Z = CharacterBottom.Z + MaximumLedgeHeight + DownwardCheckSphereRadius;
	FVector DownwardCheckEndLocation(DownwardCheckStartLocation.X, DownwardCheckStartLocation.Y, CharacterBottom.Z);

	FVector DebugDrawCapsuleLocation = (DownwardCheckStartLocation + DownwardCheckEndLocation) * 0.5f;
	float DebugDrawCapsuleHalfHeight = (DownwardCheckEndLocation - DownwardCheckStartLocation).Size() * 0.5;

	DrawDebugCapsule(GetWorld(), DebugDrawCapsuleLocation, DebugDrawCapsuleHalfHeight, DownwardCheckSphereRadius, FQuat::Identity, FColor::Black, false, DrawTime);
	if (!GetWorld()->SweepSingleByChannel(DownwardCheckHitResult, DownwardCheckStartLocation, DownwardCheckEndLocation, FQuat::Identity, ECC_Climbing, DownwardCheckSphereShape, QuerryParams))
	{
		return false;
	}
	DrawDebugSphere(GetWorld(),DownwardCheckHitResult.Location, DownwardCheckSphereRadius, 32, FColor::Red, false, DrawTime);
	DrawDebugPoint(GetWorld(), DownwardCheckHitResult.ImpactPoint, 10.0f, FColor::Red, false, DrawTime);
	//3. Overlap check
	float OverlapCapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	float OverlapCapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	FCollisionShape OverlapCapsuleShape = FCollisionShape::MakeCapsule(OverlapCapsuleRadius,OverlapCapsuleHalfHeight);
	FVector OverlapLocation = DownwardCheckHitResult.ImpactPoint + OverlapCapsuleHalfHeight*FVector::UpVector + 2.0f;
	if (GetWorld()->OverlapAnyTestByProfile(OverlapLocation, FQuat::Identity, FName("Pawn"), OverlapCapsuleShape, QuerryParams))
	{
		DrawDebugCapsule(GetWorld(), OverlapLocation, OverlapCapsuleHalfHeight, OverlapCapsuleRadius, FQuat::Identity, FColor::Red, false, DrawTime);
		return false;
	}

	LedgeDescription.Location = DownwardCheckHitResult.ImpactPoint;
	LedgeDescription.Rotation = (ForwardCheckHitResult.ImpactNormal * FVector(-1.0f, -1.0f, 0.0f)).ToOrientationRotator();
	return true;
}

