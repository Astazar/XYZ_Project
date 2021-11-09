// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XYZ_ProjectBasePawn.h"
#include "SpiderPawn.generated.h"

/**
 * 
 */
 
UCLASS()
class XYZ_PROJECT_API ASpiderPawn : public AXYZ_ProjectBasePawn
{
	GENERATED_BODY()

public:
	ASpiderPawn();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable,BlueprintPure)
	FORCEINLINE float GetIKRightFrontFootOffset() const { return IKRightFrontFootOffset; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKRightRearFootOffset() const { return IKRightRearFootOffset; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKLeftFrontFootOffset() const { return IKLeftFrontFootOffset; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKLeftRearFootOffset() const { return IKLeftRearFootOffset; }


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spider Bot")
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spider Bot|IK settings")
	FName RightFrontFootSocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spider Bot|IK settings")
	FName RightRearFootSocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spider Bot|IK settings")
	FName LeftFrontFootSocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spider Bot|IK settings")
	FName LeftRearFootSocketName;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Spider Bot|IK settings")
	float IKInterpSpeed = 20.0f;
private:
	float GetIKOffsetForASocket(const FName& SocketName);

	float IKRightFrontFootOffset = 0.0f;
	float IKRightRearFootOffset = 0.0f;
	float IKLeftFrontFootOffset = 0.0f;
	float IKLeftRearFootOffset = 0.0f;

	float IKTraceDistance = 0.0f; 
	float IKScale = 0.0f;
};
