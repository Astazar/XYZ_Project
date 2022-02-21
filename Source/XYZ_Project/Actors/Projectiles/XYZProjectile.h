#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XYZProjectile.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnProjectileHit, const FHitResult&, Hit, const FVector&, Direction, const float, ShotRange);

UCLASS()
class XYZ_PROJECT_API AXYZProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AXYZProjectile();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void LaunchProjectile(FVector Direction, FVector StartLocation);

	UPROPERTY(BlueprintAssignable)
	FOnProjectileHit OnProjectileHit;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	virtual void OnProjectileLaunched();

private:
	FVector LaunchStartLocation = FVector::ZeroVector;

	UFUNCTION()
	void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
};
