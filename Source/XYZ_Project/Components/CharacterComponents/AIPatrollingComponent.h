// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIPatrollingComponent.generated.h"

class APatrollingPath;

UENUM(BlueprintType)
enum class EPatrolType : uint8
{
	Circle = 0,
	PingPing
};

USTRUCT(BlueprintType)
struct FPatrolSettings
{
	GENERATED_BODY();
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	EPatrolType PatrolType = EPatrolType::Circle;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	APatrollingPath* PatrollingPath;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XYZ_PROJECT_API UAIPatrollingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	bool CanPatrol() const;
	FVector SelectClosestWayPoint();
	FVector SelectNextWayPoint();

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Path")
	FPatrolSettings PatrolSettings;

	int32 CurrentWayPointIndex = -1;

private:
	void NextPointCircle();
	void NextPointPingPong();
	int32 PingPongIncrement = 1;
};
