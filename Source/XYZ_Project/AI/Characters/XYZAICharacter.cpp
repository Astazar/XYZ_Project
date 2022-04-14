#include "XYZAICharacter.h"
#include "Components/CharacterComponents/AIPatrollingComponent.h"

AXYZAICharacter::AXYZAICharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	AIPatrollingComponent = CreateDefaultSubobject<UAIPatrollingComponent>(TEXT("AIPatrolling"));
}

UAIPatrollingComponent* AXYZAICharacter::GetPatrollingComponent() const
{
	return AIPatrollingComponent;
}

UBehaviorTree* AXYZAICharacter::GetBehaviorTree() const
{
	return BehaviorTree;
}
