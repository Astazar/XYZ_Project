#include "AICharacterSpawner.h"
#include <AI/Characters/XYZAICharacter.h>
#include <UObject/ScriptInterface.h>
#include "Actors/Interactive/Interface/Interactable.h"



void AAICharacterSpawner::SpawnAI()
{
	if (!bCanSpawn || !IsValid(AICharacterClass))
	{
		return;
	}

	AXYZAICharacter* AICharacter = GetWorld()->SpawnActor<AXYZAICharacter>(AICharacterClass, GetTransform());
	if (!IsValid(AICharacter->Controller))
	{
		AICharacter->SpawnDefaultController();
	}

	if (bDoOnce)
	{
		UnSubscribeFromTrigger();
		bCanSpawn = false;
	}
}

void AAICharacterSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_STRING_CHECKED(AAICharacterSpawner, SpawnTriggerActor))
	{
		SpawnTrigger = SpawnTriggerActor;
		if (SpawnTrigger.GetInterface())
		{
			if (!SpawnTrigger->HasOnInteractionCallback())
			{
				SpawnTriggerActor = nullptr;
				SpawnTrigger = nullptr;
			}
		}
		else
		{
			SpawnTriggerActor = nullptr;
			SpawnTrigger = nullptr;
		}
	}
}	

void AAICharacterSpawner::BeginPlay()
{
	Super::BeginPlay();
	if (SpawnTrigger.GetInterface())
	{
		TriggerHandle = SpawnTrigger->AddOnInteractionUFunction(this, FName("SpawnAI"));
	}

	if (bIsSpawnOnStart)
	{
		SpawnAI();
	}
}

void AAICharacterSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnSubscribeFromTrigger();
	Super::EndPlay(EndPlayReason);
}

void AAICharacterSpawner::UnSubscribeFromTrigger()
{
	if (TriggerHandle.IsValid() && SpawnTrigger.GetInterface())
	{
		SpawnTrigger->RemoveOnInteractionDelegate(TriggerHandle);
	}
}
