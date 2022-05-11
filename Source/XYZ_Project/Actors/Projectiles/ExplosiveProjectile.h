#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/XYZProjectile.h"
#include "ExplosiveProjectile.generated.h"

UENUM(BlueprintType)
enum class EExplosionType : uint8
{
	ByTimer,
	ByHit
};

class UExplosionComponent;

UCLASS()
class XYZ_PROJECT_API AExplosiveProjectile : public AXYZProjectile
{
	GENERATED_BODY()
	
public:
	AExplosiveProjectile();

	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UExplosionComponent* ExplosionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float DetonationTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	EExplosionType ExplosionType = EExplosionType::ByTimer;

	virtual void OnProjectileLaunched() override;

private:
	void OnDetonationTimerElapsed();

	UFUNCTION()
	void OnExplosiveProjectileHit(AXYZProjectile* Projectile, const FHitResult& HitResult, const FVector& Direction, float ShotRange);

	AController* GetController();

	FTimerHandle DetonationTimer;
};
