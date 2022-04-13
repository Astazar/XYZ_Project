// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISense.h"
#include <Perception/AISense_Damage.h>
#include "AISense_DamageSight.generated.h"


class UAISenseConfig_DamageSight;

USTRUCT(BlueprintType)
struct XYZ_PROJECT_API FAIDamageSightEvent : public FAIDamageEvent
{
	GENERATED_USTRUCT_BODY()

	typedef class UAISense_DamageSight FSenseClass;

	FAIDamageSightEvent() : FAIDamageEvent() {}

	FAIDamageSightEvent(AActor* InDamagedActor, AActor* InInstigator, float DamageAmount, const FVector& EventLocation, const FVector& InHitLocation = FAISystem::InvalidLocation)
		:FAIDamageEvent(InDamagedActor, InInstigator, DamageAmount, EventLocation, InHitLocation) {}
};

/**
 * 
 */
UCLASS(ClassGroup = AI)
class XYZ_PROJECT_API UAISense_DamageSight : public UAISense
{
	GENERATED_BODY()
	
public:
	UAISense_DamageSight(const FObjectInitializer& ObjectInitializer);

	struct FDigestedDamageSightProperties
	{
		float SightAngleCos;
		float SightRadiusSq;

		FDigestedDamageSightProperties();
		FDigestedDamageSightProperties(const UAISenseConfig_DamageSight& SenseConfig);
	};

	FDigestedDamageSightProperties DigestedDamageSightProp;

	virtual void RegisterEvent(const FAIDamageEvent& Event);

	virtual void RegisterEventStimulus(int32 EventIndex);

	static void ReportDamageSightEvent(UObject* WorldContextObject, AActor* DamagedActor, AActor* Instigator, float DamageAmount, FVector EventLocation, FVector HitLocation);

protected:
	UPROPERTY()
	TArray<FAIDamageEvent> RegisteredEvents;

	virtual float Update() override;

	void OnNewListenerImpl(const FPerceptionListener& NewListener);

	bool IsTargetOutOfSightRadius(FVector ListenerLocation, FVector TargetActorLocation);
};

