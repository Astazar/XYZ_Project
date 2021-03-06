#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/Interactive/Interface/Interactable.h"
#include <Components/TimelineComponent.h>
#include "Door.generated.h"

UCLASS()
class XYZ_PROJECT_API ADoor : public AActor, public IInteractable
{
	GENERATED_BODY()

public: 
	ADoor();

	virtual void Tick(float DeltaSeconds) override;

	virtual void Interact(AXYZBaseCharacter* BaseCharacter) override;

	virtual  FName GetActionEventName() const override;

	virtual bool HasOnInteractionCallback() const override;

	virtual FDelegateHandle AddOnInteractionUFunction(UObject* Object, const FName & FunctionName) override;

	virtual void RemoveOnInteractionDelegate(FDelegateHandle DelegateHandle) override;

	IInteractable::FOnInteraction OnInteractionEvent;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interactive | Door")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interactive | Door")
	USceneComponent* DoorPivot; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	float AngleClosed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	float AngleOpened = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	UCurveFloat* DoorAnimationCurve;

private:
	UFUNCTION()
	void UpdateDoorAnimation(float Alpha);

	UFUNCTION()
	void OnDoorAnimationFinished();

	void InteractWithDoor();

	FTimeline DoorOpenAnimTimeline;

	bool bIsOpened = false;

};