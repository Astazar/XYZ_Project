#include "XYZTraceUtils.h"
#include <DrawDebugHelpers.h>

bool XYZTraceUtils::SweepCapsuleSingleByChannel(const UWorld* World, struct FHitResult& OutHit, const FVector& Start, const FVector& End, const float CapsuleRadius, const float CapsuleHalfHeight, const FQuat& Rot, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params /*= FCollisionQueryParams::DefaultQueryParam*/, const FCollisionResponseParams& ResponseParam /*= FCollisionResponseParams::DefaultResponseParam*/, bool bDrawDebug /*= false*/, float DrawTime /*= -1.0f*/, FColor TraceColor /*= FColor::Black*/, FColor HitColor /*= FColor::Red*/)
{
	bool bResult = false;
	FCollisionShape CollosionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	bResult = World->SweepSingleByChannel(OutHit, Start, End, Rot, TraceChannel, CollosionShape, Params, ResponseParam);

#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		DrawDebugCapsule(World, Start, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, TraceColor, false, DrawTime);
		DrawDebugCapsule(World, End, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, TraceColor, false, DrawTime);
		DrawDebugLine(World, Start, End, TraceColor, false, DrawTime);
		if (bResult)
		{
			DrawDebugCapsule(World, OutHit.Location, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, HitColor, false, DrawTime);
			DrawDebugPoint(World, OutHit.ImpactPoint, 10.0f, HitColor, false, DrawTime);
		}
	}
#endif
	return bResult;
}

bool XYZTraceUtils::SweepSphereSingleByChannel(const UWorld* World, struct FHitResult& OutHit, const FVector& Start, const FVector& End, const float Radius, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params /*= FCollisionQueryParams::DefaultQueryParam*/, const FCollisionResponseParams& ResponseParam /*= FCollisionResponseParams::DefaultResponseParam*/, bool bDrawDebug /*= false*/, float DrawTime /*= -1.0f*/, FColor TraceColor /*= FColor::Black*/, FColor HitColor /*= FColor::Red*/)
{
	bool bResult = false;
	FCollisionShape CollosionShape = FCollisionShape::MakeSphere(Radius);
	bResult = World->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, TraceChannel, CollosionShape, Params, ResponseParam);
#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		FVector DebugDrawCapsuleLocation = (Start + End) * 0.5f;
		FVector TraceVector = End - Start;
		float DebugDrawCapsuleHalfHeight = TraceVector.Size() * 0.5;
		FQuat DebugCapsuleRotation = FRotationMatrix::MakeFromZ(TraceVector).ToQuat();

		DrawDebugCapsule(World, DebugDrawCapsuleLocation, DebugDrawCapsuleHalfHeight, Radius, DebugCapsuleRotation, FColor::Black, false, DrawTime);
		if (bResult)
		{
			DrawDebugSphere(World, OutHit.Location, Radius, 32, FColor::Red, false, DrawTime);
			DrawDebugPoint(World, OutHit.ImpactPoint, 10.0f, FColor::Red, false, DrawTime);
		}
		
	}	
#endif
	return bResult;
}

bool XYZTraceUtils::OverlapCapsuleAnyByProfile(const UWorld* World, const FVector& Pos, const float CapsuleRadius, const float CapsuleHalfHeight, const FQuat& Rot, FName ProfileName, const FCollisionQueryParams& Params /*= FCollisionQueryParams::DefaultQueryParam*/, bool bDrawDebug /*= false*/, float DrawTime /*= -1.0f*/, FColor HitColor /*= FColor::Red*/)
{
	bool bResult = false;

	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	bResult = World->OverlapAnyTestByProfile(Pos, Rot, ProfileName, CollisionShape, Params);
#if ENABLE_DRAW_DEBUG
	if (bDrawDebug && bResult)
	{
		DrawDebugCapsule(World, Pos, CapsuleHalfHeight, CapsuleRadius, Rot, HitColor, false, DrawTime);
	}
#endif
	return bResult;
}

