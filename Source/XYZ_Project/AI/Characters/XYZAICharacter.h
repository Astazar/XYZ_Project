#pragma once

#include "CoreMinimal.h"
#include "Characters/XYZBaseCharacter.h"
#include "XYZAICharacter.generated.h"


class UAIPatrollingComponent;
class UBehaviorTree;

UCLASS(Blueprintable)
class XYZ_PROJECT_API AXYZAICharacter : public AXYZBaseCharacter
{
	GENERATED_BODY()
	
public:
	AXYZAICharacter(const FObjectInitializer& ObjectInitializer);

	UAIPatrollingComponent* GetPatrollingComponent() const;

	UBehaviorTree* GetBehaviorTree() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIPatrollingComponent* AIPatrollingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;
};
