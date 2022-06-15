#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/Pickables/PickableItem.h"
#include "PickableWeapon.generated.h"



UCLASS(Blueprintable)
class XYZ_PROJECT_API APickableWeapon : public APickableItem
{
	GENERATED_BODY()
	
public:
	APickableWeapon();

	virtual void Interact(AXYZBaseCharacter* BaseCharacter) override;

	virtual FName GetActionEventName() const override;

protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* WeaponMesh;
};
