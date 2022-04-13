#include "AIPatrollingComponent.h"
#include "Actors/Navigation/PatrollingPath.h"



bool UAIPatrollingComponent::CanPatrol() const
{
	return IsValid(PatrollingPath) && PatrollingPath->GetWaypoints().Num() > 1;
}

FVector UAIPatrollingComponent::SelectClosestWayPoint()
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	const TArray<FVector> WayPoints = PatrollingPath->GetWaypoints();
	FTransform PathTransform = PatrollingPath->GetActorTransform();

	FVector ClosestWayPoint;
	float MinSquaredDistance = FLT_MAX;
	for (int32 i = 0; i < WayPoints.Num(); ++i)
	{
		FVector WayPointWorld = PathTransform.TransformPosition(WayPoints[i]);
		float CurrentSquaredDistance = (OwnerLocation - WayPointWorld).SizeSquared();
		if (CurrentSquaredDistance < MinSquaredDistance)
		{
			MinSquaredDistance = CurrentSquaredDistance;
			ClosestWayPoint = WayPointWorld;
			CurrentWayPointIndex = i;
		}
	}
	return ClosestWayPoint;
}

FVector UAIPatrollingComponent::SelectNextWayPoint()
{
	++CurrentWayPointIndex;
	const TArray<FVector> WayPoints = PatrollingPath->GetWaypoints();
	if (CurrentWayPointIndex == PatrollingPath->GetWaypoints().Num())
	{
		CurrentWayPointIndex = 0;
	}
	FTransform PathTransform = PatrollingPath->GetActorTransform();
	return PathTransform.TransformPosition(WayPoints[CurrentWayPointIndex]);
}
