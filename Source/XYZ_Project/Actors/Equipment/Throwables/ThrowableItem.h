#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "ThrowableItem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class XYZ_PROJECT_API AThrowableItem : public AEquipableItem
{
	GENERATED_BODY()
	
public:
	void Throw();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
	TSubclassOf<class AXYZProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throwables", meta = (Clamp = -90.0f, UIMin = -90.0f, ClampMax = 90.0f, UIMax = 90.0f))
	float ThrowAngle = 0.0f;

};
