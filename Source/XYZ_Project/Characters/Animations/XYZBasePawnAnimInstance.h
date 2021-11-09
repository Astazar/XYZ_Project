// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "XYZBasePawnAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class XYZ_PROJECT_API UXYZBasePawnAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	UPROPERTY(BlueprintReadOnly,Transient, Category = "Base Pawn animation instance")
	float InputForward;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Base Pawn animation instance")
	float InputRight;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Base Pawn animation instance")
	bool bIsInAir;

private:
	TWeakObjectPtr <class AXYZ_ProjectBasePawn> CachedBasePawn;
};
