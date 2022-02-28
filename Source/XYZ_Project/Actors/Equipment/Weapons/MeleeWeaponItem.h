#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "XYZ_ProjectTypes.h"
#include "MeleeWeaponItem.generated.h"


USTRUCT(BlueprintType)
struct FMeleeAttackDescription
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Attack")
	TSubclassOf<class UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Attack", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float DamageAmount = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Attack")
	class UAnimMontage* AttackMontage; 
};

UCLASS(Blueprintable)
class XYZ_PROJECT_API AMeleeWeaponItem : public AEquipableItem
{
	GENERATED_BODY()
	
public:
	AMeleeWeaponItem();

	void StartAttack(EMeleeAttackTypes AttackType);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Attack")
	TMap<EMeleeAttackTypes, FMeleeAttackDescription> Attacks;

private:
	void OnAttackTimerElapsed();
	
	FTimerHandle AttackTimer;
	FMeleeAttackDescription* CurrentAttack;
};
