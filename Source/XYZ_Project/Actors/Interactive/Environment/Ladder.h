// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XYZ_Project/Actors/Interactive/InteractiveActor.h"
#include "Ladder.generated.h"


class UStaticMeshComponent;
class UBoxComponent;
/**
 * 
 */
UCLASS(Blueprintable)
class XYZ_PROJECT_API ALadder : public AInteractiveActor
{
	GENERATED_BODY()
	
public: 
	ALadder();

	virtual	void OnConstruction(const FTransform& Transform) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float LadderHeight = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float LadderWidth = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float StepsInterval = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float BottomStepOffset = 25.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	UStaticMeshComponent* RightRailMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	UStaticMeshComponent* LeftRailMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	UInstancedStaticMeshComponent* StepsMeshComponent;

	UBoxComponent* GetLadderInteractionBox() const;
};
