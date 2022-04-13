#include "AIPatrollingComponent.h"
#include "Actors/Navigation/PatrollingPath.h"



bool UAIPatrollingComponent::CanPatrol() const
{
	return IsValid(PatrolSettings.PatrollingPath) && PatrolSettings.PatrollingPath->GetWaypoints().Num() > 1;
}

FVector UAIPatrollingComponent::SelectClosestWayPoint()
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	const TArray<FVector> WayPoints = PatrolSettings.PatrollingPath->GetWaypoints();
	FTransform PathTransform = PatrolSettings.PatrollingPath->GetActorTransform();

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
	switch (PatrolSettings.PatrolType)
	{
	case EPatrolType::Circle:
	{
		NextPointCircle();
		break;
	}
	case EPatrolType::PingPing:
	{
		NextPointPingPong();
		break;
	}
	}

	const TArray<FVector> WayPoints = PatrolSettings.PatrollingPath->GetWaypoints();
	FTransform PathTransform = PatrolSettings.PatrollingPath->GetActorTransform();
	return PathTransform.TransformPosition(WayPoints[CurrentWayPointIndex]);
}

void UAIPatrollingComponent::NextPointCircle()
{
	++CurrentWayPointIndex;
	const TArray<FVector> WayPoints = PatrolSettings.PatrollingPath->GetWaypoints();
	if (CurrentWayPointIndex == PatrolSettings.PatrollingPath->GetWaypoints().Num())
	{
		CurrentWayPointIndex = 0;
	}
}

void UAIPatrollingComponent::NextPointPingPong()
{
	CurrentWayPointIndex+=PingPongIncrement;
	const TArray<FVector> WayPoints = PatrolSettings.PatrollingPath->GetWaypoints();
	if (CurrentWayPointIndex >= WayPoints.Num())
	{
		PingPongIncrement*=-1;
		CurrentWayPointIndex = (WayPoints.Num()-1) + PingPongIncrement;
	}
	if (CurrentWayPointIndex < 0)
	{
		PingPongIncrement *= -1;
		CurrentWayPointIndex+=2;
	}
}
