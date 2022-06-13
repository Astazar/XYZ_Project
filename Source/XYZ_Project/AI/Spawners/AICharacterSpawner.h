#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AICharacterSpawner.generated.h"

class IInteractable;
class AXYZAICharacter;
UCLASS()
class XYZ_PROJECT_API AAICharacterSpawner : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SpawnAI();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Spawner")
	TSubclassOf<AXYZAICharacter> AICharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Spawner")
	bool bIsSpawnOnStart = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Spawner")
	bool bDoOnce = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Spawner")
	AActor* SpawnTriggerActor;

protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* SpawnRoot;

private:
	void UnSubscribeFromTrigger();

	UPROPERTY()
	TScriptInterface<IInteractable> SpawnTrigger;

	FDelegateHandle TriggerHandle;

	bool bCanSpawn = true;
};