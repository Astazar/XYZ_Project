#include "XYZAICharacterController.h"
#include "AI/Characters/XYZAICharacter.h"
#include <Perception/AISense_Sight.h>
#include "Components/CharacterComponents/AIPatrollingComponent.h"

void AXYZAICharacterController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (IsValid(InPawn))
	{
		checkf(InPawn->IsA<AXYZAICharacter>(), TEXT("AXYZAICharacterController::SetPawn AICharacterController can work only with AXYZAICharacter"));
		CachedAICharacter = StaticCast<AXYZAICharacter*>(InPawn);
	}
	else
	{
		CachedAICharacter = nullptr;
	}
}

void AXYZAICharacterController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);
	if (!CachedAICharacter.IsValid())
	{
		return;
	}
	TryMoveToNextTarget();
}

void AXYZAICharacterController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	if (!Result.IsSuccess())
	{
		return;
	}
	TryMoveToNextTarget();
}

void AXYZAICharacterController::BeginPlay()
{
	Super::BeginPlay();
	UAIPatrollingComponent* PatrollingComponent = CachedAICharacter->GetPatrollingComponent();
	if (PatrollingComponent->CanPatrol())
	{
		FVector ClosestWayPoint = PatrollingComponent->SelectClosestWayPoint();
		MoveToLocation(ClosestWayPoint);
		bIsPatrolling = true;
	}
}

void AXYZAICharacterController::TryMoveToNextTarget()
{
	UAIPatrollingComponent* PatrollingComponent = CachedAICharacter->GetPatrollingComponent();
	AActor* ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	if (IsValid(ClosestActor))
	{
		if (!IsTargetReached(ClosestActor->GetActorLocation()))
		{
			MoveToActor(ClosestActor);
		}
		bIsPatrolling = false;
	}
	else if (PatrollingComponent->CanPatrol())
	{
		FVector WayPoint = bIsPatrolling ? PatrollingComponent->SelectNextWayPoint() : PatrollingComponent->SelectClosestWayPoint();
		if (!IsTargetReached(WayPoint))
		{
			MoveToLocation(WayPoint);
		}
		bIsPatrolling = true;
	}
}

bool AXYZAICharacterController::IsTargetReached(FVector TargetLocation) const
{
	return (TargetLocation - CachedAICharacter->GetActorLocation()).SizeSquared() <= FMath::Square(TargetReachRadius);
}
