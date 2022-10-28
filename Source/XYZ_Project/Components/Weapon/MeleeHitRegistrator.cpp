// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeHitRegistrator.h"
#include "XYZ_ProjectTypes.h"
#include <Utils/XYZTraceUtils.h>
#include <Subsystems/DebugSubsystem.h>

UMeleeHitRegistrator::UMeleeHitRegistrator()
{
	PrimaryComponentTick.bCanEverTick = true;
	SphereRadius = 5.0f;
	SetCollisionProfileName(CollisionProfileNoCollision);
}

void UMeleeHitRegistrator::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bIsHitRegistrationEnabled)
	{
		ProcessHitRegistration();
	}
	PreviousComponentLocation = GetComponentLocation();
}

void UMeleeHitRegistrator::ProcessHitRegistration()
{
	FVector CurrentLocation = GetComponentLocation();
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.AddIgnoredActor(GetOwner()->GetOwner());
	bool bIsDebugEnabled = UDebugSubsystem::GetDebugSubsystem(GetWorld())->IsCategoryEnabled(DebugCategoryMeleeWeapon);

	bool bHasHit = XYZTraceUtils::SweepSphereSingleByChannel(
			GetWorld(),
			HitResult,
			PreviousComponentLocation,
			CurrentLocation, 
			GetScaledSphereRadius(),
			ECC_Melee,
			QueryParams,
			FCollisionResponseParams::DefaultResponseParam,
			bIsDebugEnabled,
			5.0f
	);
	if (bHasHit)
	{
		FVector Direction = (CurrentLocation - PreviousComponentLocation).GetSafeNormal();
		if (OnMeleeHitRegistred.IsBound())
		{
			OnMeleeHitRegistred.Broadcast(HitResult, Direction);
		}
	}
}

void UMeleeHitRegistrator::SetIsHitRegistrationEnabled(bool bIsEnabled_In)
{
	bIsHitRegistrationEnabled = bIsEnabled_In;
}
