// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZBasePawnMovementComponent.h"
#include "../../Characters/XYZ_ProjectBasePawn.h"

void UXYZBasePawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);

	FVector PendingInput = GetPendingInputVector().GetClampedToMaxSize(1.0f);
	Velocity = PendingInput * MaxSpeed;
	ConsumeInputVector();

	if (bEnableGravity)
	{
		FHitResult HitResult;
		FVector StartPoint = UpdatedComponent->GetComponentLocation();

		AXYZ_ProjectBasePawn* BasePawn = StaticCast<AXYZ_ProjectBasePawn*>(GetOwner());
		float SphereRadius = BasePawn->GetCollisionSphereRadius();
		float TraceDepth = 2.0f;
		
		FVector EndPoint = StartPoint - TraceDepth * FVector::UpVector;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(GetOwner());

		bool bWasFalling = bIsFalling;
		FCollisionShape Sphere = FCollisionShape::MakeSphere(SphereRadius);
		bIsFalling=!GetWorld()->SweepSingleByChannel(HitResult,StartPoint,EndPoint, FQuat::Identity, ECC_Visibility, Sphere, CollisionParams);
		if(bIsFalling)
		{ 
			VerticalVelocity += GetGravityZ() * FVector::UpVector * DeltaTime;
		}
		else if(bWasFalling && VerticalVelocity.Z < 0.0f)
		{
			VerticalVelocity = FVector::ZeroVector;
		}
	}
	Velocity+=VerticalVelocity;
	FVector Delta = Velocity * DeltaTime;
	if (!Delta.IsNearlyZero(1e-6f))
	{
		FQuat Rot = UpdatedComponent->GetComponentQuat();
		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent(Delta, Rot, true, Hit);

		if (Hit.IsValidBlockingHit())
		{
			HandleImpact(Hit, DeltaTime, Delta);
			// Try to slide the remaining distance along the surface.
			SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);
		}
	}
	UpdateComponentVelocity();
}

void UXYZBasePawnMovementComponent::JumpStart()
{	
	VerticalVelocity = InitialJumpVelocity * FVector::UpVector;
}

bool UXYZBasePawnMovementComponent::IsFalling() const
{
	return bIsFalling;
}
