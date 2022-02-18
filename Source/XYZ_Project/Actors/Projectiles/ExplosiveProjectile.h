#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/XYZProjectile.h"
#include "ExplosiveProjectile.generated.h"


class UExplosionComponent;

UCLASS()
class XYZ_PROJECT_API AExplosiveProjectile : public AXYZProjectile
{
	GENERATED_BODY()
	
public:
	AExplosiveProjectile();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UExplosionComponent* ExplosionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float DetonationTime = 2.0f;

	virtual void OnProjectileLaunched() override;

private:
	void OnDetonationTimerElapsed();

	AController* GetController();

	FTimerHandle DetonationTimer;
};
