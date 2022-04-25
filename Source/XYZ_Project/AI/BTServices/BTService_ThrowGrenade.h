// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_ThrowGrenade.generated.h"

/**
 * 
 */
UCLASS()
class XYZ_PROJECT_API UBTService_ThrowGrenade : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_ThrowGrenade();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float MaxThrowDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float MinThrowDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float ThrowDelay = 10.0f;

private:
	FTimerHandle ThrowTimer;
};
