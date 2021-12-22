// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XYZ_Project/Actors/Interactive/InteractiveActor.h"
#include "Ladder.generated.h"

class UAnimMontage;
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

	virtual void BeginPlay() override;

	float GetLadderHeight() const;

	bool GetIsOnTop() const;

	UAnimMontage* GetAttachFromTopAnimMontage() const;

	FVector GetAttachFromTopAnimMontageStartingLocation() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float LadderHeight = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float LadderWidth = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float StepsInterval = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float BottomStepOffset = 25.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* RightRailMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* LeftRailMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInstancedStaticMeshComponent* StepsMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* TopInteractionVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	UAnimMontage* AttachFromTopAnimMontage;

	// Offset from ladder`s top for starting anim montage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	FVector AttachFromTopAnimMontageInitialOffset = FVector::ZeroVector;

	UBoxComponent* GetLadderInteractionBox() const;

	virtual void OnInteractionVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual	void OnInteractionVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:
	bool bIsOnTop = false;
};
