#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "ThrowableItem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnThrowAmmoChanged, int32);

USTRUCT(BlueprintType)
struct FThrowInfo
{
	GENERATED_BODY()

	FThrowInfo() : Location_Mul_10(FVector_NetQuantize100::ZeroVector), Direction(FVector_NetQuantizeNormal::ZeroVector) {};
	FThrowInfo(FVector Location, FVector Direction) : Location_Mul_10(Location * 10.0f), Direction(Direction) {};

	UPROPERTY()
	FVector_NetQuantize100 Location_Mul_10;

	UPROPERTY()
	FVector_NetQuantizeNormal Direction;

	FVector GetLocation() const { return Location_Mul_10 * 0.1f; }
	FVector GetDirection() const { return Direction; }
};


UCLASS(Blueprintable)
class XYZ_PROJECT_API AThrowableItem : public AEquipableItem
{
	GENERATED_BODY()
	
public:
	AThrowableItem();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void CreatePools() override;

	void Throw();

	void LaunchItemProjectile();

	bool CanThrow() const;

	bool GetIsThrowing() const;

	void ThrowAnimationFinished();

	EAmunitionType GetThrowAmmoType() const;

	int32 GetThrowAmmo() const;
	void SetThrowAmmo(int32 NewAmmo);

	int32 GetMaxThrowAmmo() const;

	FOnThrowAmmoChanged OnThrowAmmoChanged;

protected:
	void LaunchItemProjectileInternal(const FThrowInfo& ThrowInfo);

	void PlayThrowMontage();

	UFUNCTION(Server, Reliable)
	void Server_LaunchItemProjectile(const FThrowInfo& ThrowInfo);

	UFUNCTION(Server, Reliable)
	void Server_Throw();

	UFUNCTION()
	void OnRep_ThrowInfo();

	UFUNCTION()
	void OnRep_IsThrowing();

	UFUNCTION()
	void BackToPool(AXYZProjectile* Projectile);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables | Ammo", meta = (ClampMin = 0, UIMin = 0))
	int32 ThrowAmmo = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables | Ammo", meta = (ClampMin = 0, UIMin = 0))
	int32 MaxThrowAmmo = 5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables | Ammo")
	EAmunitionType ThrowAmmoType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
	UAnimMontage* ThrowMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
	TSubclassOf<class AXYZProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throwables", meta = (ClampMin = -90.0f, UIMin = -90.0f, ClampMax = 90.0f, UIMax = 90.0f))
	float ThrowAngle = 0.0f;

	UPROPERTY(ReplicatedUsing=OnRep_ThrowInfo)
	FThrowInfo CurrentThrowInfo;

	UPROPERTY(Replicated)
	TArray<AXYZProjectile*> ProjectilePool;

	int32 CurrentProjectileIndex = 0;

	int32 ProjectilePoolSize = 1;

	const FVector ProjectilePoolLocation = FVector(0.0f, 0.0f, -100.0f);

private:
	UPROPERTY(ReplicatedUsing=OnRep_IsThrowing)
	bool bIsThrowing = false;

	FTimerHandle ThrowTimer;
};
