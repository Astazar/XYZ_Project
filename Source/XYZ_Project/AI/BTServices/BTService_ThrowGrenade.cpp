#include "BTService_ThrowGrenade.h"
#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <Characters/XYZBaseCharacter.h>

UBTService_ThrowGrenade::UBTService_ThrowGrenade()
{
	NodeName = "Throw grenade";
}

void UBTService_ThrowGrenade::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (GetWorld()->GetTimerManager().IsTimerActive(ThrowTimer))
	{
		return;
	}

	AAIController* AIC = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!IsValid(AIC) || !IsValid(Blackboard))
	{
		return;
	}

	AXYZBaseCharacter* Character = Cast<AXYZBaseCharacter>(AIC->GetPawn());
	if (!IsValid(Character))
	{
		return;
	}

	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!IsValid(CurrentTarget))
	{
		return;
	}

	float DistanceSquared = FVector::DistSquared(CurrentTarget->GetActorLocation(), Character->GetActorLocation());
	if (DistanceSquared > FMath::Square(MaxThrowDistance) && DistanceSquared < MinThrowDistance)
	{
		return;
	}

	Character->ThrowPrimaryItem(true);
	GetWorld()->GetTimerManager().SetTimer(ThrowTimer, ThrowDelay, false, ThrowDelay);
}
