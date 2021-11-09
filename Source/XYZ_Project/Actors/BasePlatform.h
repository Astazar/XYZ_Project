// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/TimelineComponent.h>
#include "PlatformInvocator.h"
#include "BasePlatform.generated.h"


UENUM()
enum class EPlatformBehaviour: uint8
{
	OnDemand = 0,
	Loop 
};

UENUM()
enum class EPlatformMovingDirection : uint8
{
	Forward = 0,
	Backward
};


UCLASS()
class XYZ_PROJECT_API ABasePlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly,BlueprintReadOnly)
	APlatformInvocator* PlatformInvocator;

	void OnPlatformInvoked();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void PlatformTimelineStart();

	void PlatformTimelineUpdate(const float Alpha);

	void PlatformTimelineFinished();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform settings", meta=(MakeEditWidget))
	FVector EndLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform settings", Transient)
	FVector StartLocation;

	FTimeline PlatformTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform settings")
	UCurveFloat* TimelineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform settings")
	EPlatformBehaviour PlatformBehaviour = EPlatformBehaviour::OnDemand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform settings", meta = (UIMin = 0.01f, ClampMin = 0.01f, UIMax = 100.0f, ClampMax = 100.0f))
	float PlatformDelay = 0.01f;

	//If false, then we can't change moving direction until the timeline is completely played.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform settings")
	bool bCanInterruptTimeline = false;

private:
	float MaxTimelineValue;
	float MinTimelineValue;

	FTimerHandle PlatformDelayTimer;

	EPlatformMovingDirection PlatformMovingDirection=EPlatformMovingDirection::Forward;

	void LoopMoving();

	void OnDemandMoving();

	void ChangeMovingDirection();
};
