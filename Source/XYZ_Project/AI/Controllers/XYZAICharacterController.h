#pragma once

#include "CoreMinimal.h"
#include "AI/Controllers/XYZAIController.h"
#include "XYZAICharacterController.generated.h"


class AXYZAICharacter;

UCLASS()
class XYZ_PROJECT_API AXYZAICharacterController : public AXYZAIController
{
	GENERATED_BODY()
	
public:
	virtual void SetPawn(APawn* InPawn) override;

	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float TargetReachRadius = 100.0f;

private:
	void TryMoveToNextTarget();

	bool IsTargetReached(FVector TargetLocation) const;

	TWeakObjectPtr<AXYZAICharacter> CachedAICharacter;

	bool bIsPatrolling = false;
};
