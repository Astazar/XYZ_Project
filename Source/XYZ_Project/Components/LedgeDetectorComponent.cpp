// Fill out your copyright notice in the Description page of Project Settings.


#include "LedgeDetectorComponent.h"
#include <GameFramework/Character.h>
#include <Components/CapsuleComponent.h>
#include "XYZ_Project/XYZ_ProjectTypes.h"
#include <DrawDebugHelpers.h>
#include "XYZ_Project/Utils/XYZTraceUtils.h"
#include "XYZ_Project/Characters/XYZBaseCharacter.h"
#include <Kismet/GameplayStatics.h>
#include "XYZ_Project/XYZGameInstance.h"
#include "XYZ_Project/Subsystems/DebugSubsystem.h"





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

	bool IsDebugEnabled = UDebugSubsystem::GetDebugSubsystem()->IsCategoryEnabled(DebugCategoryLedgeDetection);
	float DrawTime = 5.0f;
	float BottomZOffset = 2.0f;
	FVector CharacterBottom = CachedCharacterOwner->GetActorLocation() - (CapsuleComponent->GetScaledCapsuleHalfHeight() -BottomZOffset) * FVector::UpVector;

	//1. Forward check 
	float ForwardCheckCapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	float ForwardCheckCapsuleHalfHeight = (MaximumLedgeHeight - MinimumLedgeHeight) * 0.5;

	FHitResult ForwardCheckHitResult;
	FVector ForwardCheckStartLocation = CharacterBottom + (MinimumLedgeHeight + ForwardCheckCapsuleHalfHeight)*FVector::UpVector;
	FVector ForwardCheckEndLocation = ForwardCheckStartLocation + CachedCharacterOwner->GetActorForwardVector() * ForwardCheckDistance;

	if (!XYZTraceUtils::SweepCapsuleSingleByChannel(GetWorld(), ForwardCheckHitResult, ForwardCheckStartLocation, ForwardCheckEndLocation, ForwardCheckCapsuleRadius, ForwardCheckCapsuleHalfHeight, FQuat::Identity, ECC_Climbing, QuerryParams, FCollisionResponseParams::DefaultResponseParam, IsDebugEnabled, DrawTime))
	{
		return false;
	}

	//2. Downward check
	float DownwardCheckSphereRadius = CapsuleComponent->GetScaledCapsuleRadius();
	FHitResult DownwardCheckHitResult;

	float DownwardCheckDepthOffset = 10.0f;
	FVector DownwardCheckStartLocation = ForwardCheckHitResult.ImpactPoint - ForwardCheckHitResult.ImpactNormal * DownwardCheckDepthOffset; 
	DownwardCheckStartLocation.Z = CharacterBottom.Z + MaximumLedgeHeight + DownwardCheckSphereRadius;
	FVector DownwardCheckEndLocation(DownwardCheckStartLocation.X, DownwardCheckStartLocation.Y, CharacterBottom.Z);

	if (!XYZTraceUtils::SweepSphereSingleByChannel(GetWorld(), DownwardCheckHitResult, DownwardCheckStartLocation, DownwardCheckEndLocation, DownwardCheckSphereRadius, ECC_Climbing, QuerryParams, FCollisionResponseParams::DefaultResponseParam, IsDebugEnabled, DrawTime))
	{
		return false;
	}
	
	//3. Overlap check
	ACharacter* DefaultChar = CachedCharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	UCapsuleComponent* DefaultCapsule = DefaultChar->GetCapsuleComponent();
	float OverlapCapsuleRadius = DefaultCapsule->GetScaledCapsuleRadius();
	float OverlapCapsuleHalfHeight = DefaultCapsule->GetScaledCapsuleHalfHeight();
	float OverlapCapsuleFloorOffset = 5.0f;
	FVector OverlapLocation = DownwardCheckHitResult.ImpactPoint + (OverlapCapsuleHalfHeight + OverlapCapsuleFloorOffset)*FVector::UpVector;

	if (XYZTraceUtils::OverlapCapsuleBlockingByProfile(GetWorld(), OverlapLocation, OverlapCapsuleRadius, OverlapCapsuleHalfHeight, FQuat::Identity, CollisionProfilePawn, QuerryParams, IsDebugEnabled, DrawTime))
	{
		return false;
	}

	LedgeDescription.Location = OverlapLocation;
	LedgeDescription.Rotation = (ForwardCheckHitResult.ImpactNormal * FVector(-1.0f, -1.0f, 0.0f)).ToOrientationRotator();
	LedgeDescription.LedgeNormal = ForwardCheckHitResult.ImpactNormal; 
	LedgeDescription.GeometryComponent = DownwardCheckHitResult.GetComponent();
	LedgeDescription.InitialGeometryLocation = LedgeDescription.GeometryComponent->GetComponentLocation();

	return true;
}

