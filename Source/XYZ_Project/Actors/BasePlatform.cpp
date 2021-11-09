// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlatform.h"

// Sets default values
ABasePlatform::ABasePlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PlatformRoot"));
	RootComponent = DefaultPlatformRoot;
	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	PlatformMesh->SetupAttachment(RootComponent);
	StartLocation = PlatformMesh->GetRelativeLocation();
}

// Called when the game starts or when spawned
void ABasePlatform::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(TimelineCurve))
	{
		FOnTimelineFloatStatic PlatformMovementTimelineUpdate;
		FOnTimelineEventStatic PlatformMovementTimelineFinished;

		PlatformMovementTimelineUpdate.BindUObject(this, &ABasePlatform::PlatformTimelineUpdate);
		PlatformTimeline.AddInterpFloat(TimelineCurve,PlatformMovementTimelineUpdate);

		PlatformMovementTimelineFinished.BindUObject(this, &ABasePlatform::PlatformTimelineFinished);
		PlatformTimeline.SetTimelineFinishedFunc(PlatformMovementTimelineFinished);

		PlatformTimeline.SetLooping(false);
		TimelineCurve->GetTimeRange(MinTimelineValue,MaxTimelineValue);
	}

	if (IsValid(PlatformInvocator))
	{
		PlatformInvocator->OnInvocatorActivated.AddUObject(this, &ABasePlatform::OnPlatformInvoked);
	}
}

void ABasePlatform::OnPlatformInvoked()
{
	PlatformTimelineStart();
}

// Called every frame
void ABasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlatformTimeline.TickTimeline(DeltaTime);
}

void ABasePlatform::PlatformTimelineStart()
{
	if((!bCanInterruptTimeline) && (PlatformTimeline.IsPlaying()))
	{
		return;
	}
	else if((bCanInterruptTimeline) && (PlatformTimeline.IsPlaying()))
	{
		ChangeMovingDirection();
	}

	if (PlatformBehaviour == EPlatformBehaviour::Loop)
	{
		GetWorld()->GetTimerManager().SetTimer(PlatformDelayTimer, this, &ABasePlatform::LoopMoving,PlatformDelay);
	}
	else if (PlatformBehaviour == EPlatformBehaviour::OnDemand)
	{
		OnDemandMoving();
	}
}

void ABasePlatform::PlatformTimelineUpdate(const float Alpha)
{
	const FVector PlatformTargetLocation = FMath::Lerp(StartLocation,EndLocation, Alpha);
	PlatformMesh->SetRelativeLocation(PlatformTargetLocation);
}

void ABasePlatform::PlatformTimelineFinished()
{
	ChangeMovingDirection();
	if (PlatformBehaviour == EPlatformBehaviour::Loop)
	{
		GetWorld()->GetTimerManager().SetTimer(PlatformDelayTimer,this, &ABasePlatform::LoopMoving,PlatformDelay);
	}
}

void ABasePlatform::LoopMoving()
{
	if (PlatformTimeline.GetPlaybackPosition() == MinTimelineValue)
	{
		PlatformTimeline.Play();
	}
	else PlatformTimeline.Reverse();
}

void ABasePlatform::OnDemandMoving()
{
	if (PlatformMovingDirection==EPlatformMovingDirection::Forward)
	{
		PlatformTimeline.Play();
	}
	else if (PlatformMovingDirection==EPlatformMovingDirection::Backward)
	{
		PlatformTimeline.Reverse();
	}
}

void ABasePlatform::ChangeMovingDirection()
{
	PlatformMovingDirection = PlatformMovingDirection == EPlatformMovingDirection::Forward ? EPlatformMovingDirection::Backward : EPlatformMovingDirection::Forward;
}

