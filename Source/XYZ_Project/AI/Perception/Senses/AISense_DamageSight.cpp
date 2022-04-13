// Fill out your copyright notice in the Description page of Project Settings.


#include "AISense_DamageSight.h"
#include <Perception/AIPerceptionSystem.h>
#include <Perception/AIPerceptionComponent.h>
#include <Perception/AIPerceptionListenerInterface.h>
#include "AI/Perception/Configs/AISenseConfig_DamageSight.h"



UAISense_DamageSight::UAISense_DamageSight(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OnNewListenerDelegate.BindUObject(this, &UAISense_DamageSight::OnNewListenerImpl);
}

void UAISense_DamageSight::RegisterEvent(const FAIDamageEvent& Event)
{
	if (Event.IsValid())
	{
		int32 EventIndex = RegisteredEvents.Add(Event);
		RegisterEventStimulus(EventIndex);
	}
}

void UAISense_DamageSight::RegisterEventStimulus(int32 EventIndex)
{
	AIPerception::FListenerMap& ListenersMap = *GetListeners();
	IAIPerceptionListenerInterface* PerceptionListener = RegisteredEvents[EventIndex].GetDamagedActorAsPerceptionListener();
	if (PerceptionListener != nullptr)
	{
		UAIPerceptionComponent* PerceptionComponent = PerceptionListener->GetPerceptionComponent();
		if (PerceptionComponent != nullptr && PerceptionComponent->GetListenerId().IsValid())
		{
			FPerceptionListener& Listener = ListenersMap[PerceptionComponent->GetListenerId()];
			if (!IsTargetOutOfSightRadius(Listener.CachedLocation, RegisteredEvents[EventIndex].Instigator->GetActorLocation()) && Listener.HasSense(GetSenseID()))
			{
				Listener.RegisterStimulus(RegisteredEvents[EventIndex].Instigator, FAIStimulus(*this, RegisteredEvents[EventIndex].Amount, RegisteredEvents[EventIndex].Location, RegisteredEvents[EventIndex].HitLocation));
				RequestImmediateUpdate();
				return;
			}
		}
	}
	RegisteredEvents.RemoveAt(EventIndex);
}

void UAISense_DamageSight::ReportDamageSightEvent(UObject* WorldContextObject, AActor* DamagedActor, AActor* Instigator, float DamageAmount, FVector EventLocation, FVector HitLocation)
{
	UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(WorldContextObject);
	if (PerceptionSystem)
	{
		FAIDamageSightEvent Event(DamagedActor, Instigator, DamageAmount, EventLocation, HitLocation);
		FAISenseID senseID = GetSenseID<FAIDamageEvent::FSenseClass>();
		PerceptionSystem->OnEvent(Event);
	}
}


float UAISense_DamageSight::Update()
{
	AIPerception::FListenerMap& ListenersMap = *GetListeners();
	for (int i = 0; i < RegisteredEvents.Num(); i++)
	{
		IAIPerceptionListenerInterface* PerceptionListener = RegisteredEvents[i].GetDamagedActorAsPerceptionListener();
		if (PerceptionListener != nullptr)
		{
			UAIPerceptionComponent* PerceptionComponent = PerceptionListener->GetPerceptionComponent();
			const FActorPerceptionInfo* Inf = PerceptionComponent->GetActorInfo(*(RegisteredEvents[i].Instigator));
			if (Inf != nullptr)
			{
				if (Inf->LastSensedStimuli[GetSenseID()].IsExpired())
				{
					RegisteredEvents.RemoveAt(i);
					return 0;
				}
				bool bIsActiveStimulus = PerceptionComponent->HasActiveStimulus(*(RegisteredEvents[i].Instigator), GetSenseID());
				FPerceptionListener& Listener = ListenersMap[PerceptionComponent->GetListenerId()];
				FVector TargetActorLocation = RegisteredEvents[i].Instigator->GetActorLocation();
				if (bIsActiveStimulus)
				{
					if (!IsTargetOutOfSightRadius(Listener.CachedLocation, TargetActorLocation))
					{
						const FVector DirectionToTarget = (TargetActorLocation - Listener.CachedLocation).GetUnsafeNormal();
						bool bRes = FVector::DotProduct(DirectionToTarget, Listener.CachedDirection) > DigestedDamageSightProp.SightAngleCos;
						if (bRes)
						{
							FHitResult Hit;
							const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Listener.CachedLocation, TargetActorLocation, ECC_Visibility);
							if (Hit.Actor == RegisteredEvents[i].Instigator)
							{
								Listener.RegisterStimulus(RegisteredEvents[i].Instigator, FAIStimulus(*this, RegisteredEvents[i].Amount, RegisteredEvents[i].Instigator->GetActorLocation(), RegisteredEvents[i].HitLocation));
							}
						}
					}
				}
			}
		}
	}
	return 0;
}


void UAISense_DamageSight::OnNewListenerImpl(const FPerceptionListener& NewListener)
{
	UAIPerceptionComponent* NewListenerPtr = NewListener.Listener.Get();
	check(NewListenerPtr);
	const UAISenseConfig_DamageSight* SenseConfig = Cast<const UAISenseConfig_DamageSight>(NewListenerPtr->GetSenseConfig(GetSenseID()));
	check(SenseConfig);
	DigestedDamageSightProp = FDigestedDamageSightProperties(*SenseConfig);
}

bool UAISense_DamageSight::IsTargetOutOfSightRadius(FVector ListenerLocation, FVector TargetActorLocation)
{
	return (FVector::DistSquared(ListenerLocation, TargetActorLocation) > DigestedDamageSightProp.SightRadiusSq);
}

UAISense_DamageSight::FDigestedDamageSightProperties::FDigestedDamageSightProperties()
	: SightAngleCos(0.0f), SightRadiusSq(-1.0f)
{}

UAISense_DamageSight::FDigestedDamageSightProperties::FDigestedDamageSightProperties(const UAISenseConfig_DamageSight& SenseConfig)
{
	SightAngleCos = FMath::Cos(FMath::Clamp(FMath::DegreesToRadians(SenseConfig.SightAngle), 0.0f, PI));
	SightRadiusSq = FMath::Square(SenseConfig.SightRadius);
}
