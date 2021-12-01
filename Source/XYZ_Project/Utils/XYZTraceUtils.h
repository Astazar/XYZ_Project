#pragma once

namespace XYZTraceUtils
{
	bool SweepCapsuleSingleByChannel(const UWorld* World, struct FHitResult& OutHit, const FVector& Start, const FVector& End, const float CapsuleRadius, const float CapsuleHalfHeight, const FQuat& Rot, ECollisionChannel TraceChannel,  const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam, bool bDrawDebug = false, float DrawTime = -1.0f, FColor TraceColor = FColor::Black, FColor HitColor = FColor::Red);
	bool SweepSphereSingleByChannel(const UWorld* World, struct FHitResult& OutHit, const FVector& Start, const FVector& End, const float Radius, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam, bool bDrawDebug = false, float DrawTime = -1.0f, FColor TraceColor = FColor::Black, FColor HitColor = FColor::Red);
	bool OverlapCapsuleAnyByProfile(const UWorld* World, const FVector& Pos, const float CapsuleRadius, const float CapsuleHalfHeight, const FQuat& Rot, FName ProfileName, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, bool bDrawDebug = false, float DrawTime = -1.0f, FColor HitColor = FColor::Red);
}