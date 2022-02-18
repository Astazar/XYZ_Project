#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XYZProjectile.generated.h"

UCLASS()
class XYZ_PROJECT_API AXYZProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AXYZProjectile();

	UFUNCTION(BlueprintCallable)
	void LaunchProjectile(FVector Direction);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	virtual void OnProjectileLaunched();
};
