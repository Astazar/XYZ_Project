#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAttributesComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeathEventSignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FOutOfStaminaEventSignature, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FCurrentHealthChanged, float, float);
DECLARE_MULTICAST_DELEGATE_TwoParams(FCurrentStaminaChanged, float, float);
DECLARE_MULTICAST_DELEGATE_TwoParams(FCurrentOxygenChanged, float, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XYZ_PROJECT_API UCharacterAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterAttributesComponent();

	FOnDeathEventSignature OnDeathEvent;
	FOutOfStaminaEventSignature OutOfStaminaEvent;

	FCurrentHealthChanged OnCurrentHealthChangedEvent;
	FCurrentStaminaChanged OnCurrentStaminaChangedEvent;
	FCurrentOxygenChanged OnCurrentOxygenChangedEvent;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool IsAlive() { return CurrentHealth > 0.0f; }

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void UpdateStamina(float DeltaSeconds);
	virtual void UpdateOxygenValue(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	virtual void SetCurrentHealthClamped(float NewHealth);
	UFUNCTION(BlueprintCallable)
	virtual void SetCurrentStaminaClamped(float NewStamina);
	UFUNCTION(BlueprintCallable)
	virtual void SetCurrentOxygenClamped(float NewOxygen);

	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentHealth() const { return CurrentHealth; };
	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentStamina() const { return CurrentStamina; };
	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentOxygen() const { return CurrentOxygen; };

	float GetCurrentHealthPercent() const;
	float GetCurrentStaminaPercent() const;
	float GetCurrentOxygenPercent() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxStamina = 1000.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float StaminaRestoreVelocity = 200.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SprintStaminaConsumptionVelocity = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Oxygen", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxOxygen = 100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Oxygen", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float OxygenRestoreVelocity = 15.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Oxygen", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float OxygenConsumptionVelocity = 10.0f;

private:
	UPROPERTY(ReplicatedUsing=OnRep_Health)
	float CurrentHealth = 0.0f;
	float CurrentStamina = 0.0f;
	float CurrentOxygen = 0.0f;

	UFUNCTION()
	void OnRep_Health();
	void OnHealthChanged();


#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT		
	void DebugDrawAttributes();
#endif

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	TWeakObjectPtr<class AXYZBaseCharacter> CachedBaseCharacter;
};
