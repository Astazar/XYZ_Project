// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributesComponent.h"
#include "XYZ_Project/Characters/XYZBaseCharacter.h"
#include "XYZ_Project/Subsystems/DebugSubsystem.h"
#include "XYZ_Project/XYZ_ProjectTypes.h"
#include <DrawDebugHelpers.h>
#include <Components/CapsuleComponent.h>
#include "XYZ_Project/Components/MovementComponents/XYZBaseMovementComponent.h"
#include <GameFramework/Pawn.h>
#include <TimerManager.h>
#include <Net/UnrealNetwork.h>

UCharacterAttributesComponent::UCharacterAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}


void UCharacterAttributesComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCharacterAttributesComponent, CurrentHealth);
}

void UCharacterAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<AXYZBaseCharacter>(), TEXT("UCharacterAttributesComponent::BeginPlay UCharacterAttributesComponent can be used only with AXYZBaseCharacter"));
	checkf(MaxHealth > 0.0f,TEXT("UCharacterAttributesComponent::BeginPlay Max health can not be equal to zero"));
	checkf(MaxStamina > 0.0f, TEXT("UCharacterAttributesComponent::BeginPlay Max stamina can not be equal to zero"));
	checkf(MaxOxygen > 0.0f, TEXT("UCharacterAttributesComponent::BeginPlay Max oxygen can not be equal to zero"));
	CachedBaseCharacter = StaticCast<AXYZBaseCharacter*>(GetOwner());
	SetCurrentHealthClamped(MaxHealth);
	SetCurrentStaminaClamped(MaxStamina);
	SetCurrentOxygenClamped(MaxOxygen);

	if (GetOwner()->HasAuthority())
	{
		CachedBaseCharacter->OnTakeAnyDamage.AddDynamic(this, &UCharacterAttributesComponent::OnTakeAnyDamage);
	}
}

void UCharacterAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateStamina(DeltaTime);
	UpdateOxygenValue(DeltaTime);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttributes();
#endif
}


void UCharacterAttributesComponent::UpdateStamina(float DeltaSeconds)
{
	UXYZBaseMovementComponent* XYZBaseMovementComponent = CachedBaseCharacter->GetCharacterMovementComponent();
	if (!XYZBaseMovementComponent->IsSprinting() && CurrentStamina < MaxStamina)
	{
		SetCurrentStaminaClamped(CurrentStamina + StaminaRestoreVelocity * DeltaSeconds);
		if (CurrentStamina == MaxStamina && XYZBaseMovementComponent->GetIsOutOfStamina())
		{
			if (OutOfStaminaEvent.IsBound())
			{
				OutOfStaminaEvent.Broadcast(false);
			}
		}
	}
	if (XYZBaseMovementComponent->IsSprinting())
	{
		SetCurrentStaminaClamped(CurrentStamina - SprintStaminaConsumptionVelocity * DeltaSeconds);
		if (CurrentStamina <= 0.0f)
		{ 
			if (OutOfStaminaEvent.IsBound())
			{
				OutOfStaminaEvent.Broadcast(true);
			}
		}
	}
}


void UCharacterAttributesComponent::UpdateOxygenValue(float DeltaTime)
{
	ESwimState CurrentSwimState = CachedBaseCharacter->GetCharacterMovementComponent()->GetCurrentSwimState();

	if (CurrentSwimState != ESwimState::UnderWater && CurrentOxygen == MaxOxygen)
	{
		return;
	}

	float OxygenDelta = ((CurrentSwimState == ESwimState::None || CurrentSwimState == ESwimState::OnWaterSurface) && CurrentOxygen < MaxOxygen) ? OxygenRestoreVelocity : -OxygenConsumptionVelocity;
	SetCurrentOxygenClamped(CurrentOxygen + OxygenDelta * DeltaTime);
}


void UCharacterAttributesComponent::SetCurrentHealthClamped(float NewHealth)
{
	CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
}

void UCharacterAttributesComponent::SetCurrentStaminaClamped(float NewStamina)
{
	CurrentStamina = FMath::Clamp(NewStamina, 0.0f, MaxStamina);
	if (OnCurrentStaminaChangedEvent.IsBound())
	{
		OnCurrentStaminaChangedEvent.Broadcast(CurrentStamina, MaxStamina);
	}
}

void UCharacterAttributesComponent::SetCurrentOxygenClamped(float NewOxygen)
{
	CurrentOxygen = FMath::Clamp(NewOxygen, 0.0f, MaxOxygen);
	if (OnCurrentOxygenChangedEvent.IsBound())
	{
		OnCurrentOxygenChangedEvent.Broadcast(CurrentOxygen, MaxOxygen);
	}
}

float UCharacterAttributesComponent::GetCurrentHealthPercent() const
{
	return CurrentHealth/MaxHealth;
}

float UCharacterAttributesComponent::GetCurrentStaminaPercent() const
{
	return CurrentStamina/MaxStamina;
}

float UCharacterAttributesComponent::GetCurrentOxygenPercent() const
{
	return CurrentOxygen/MaxOxygen;
}

void UCharacterAttributesComponent::OnRep_Health()
{
	OnHealthChanged();
}

void UCharacterAttributesComponent::OnHealthChanged()
{
	if (OnCurrentHealthChangedEvent.IsBound())
	{
		OnCurrentHealthChangedEvent.Broadcast(CurrentHealth, MaxHealth);
	}
	if (CurrentHealth <= 0.0f)
	{
		if (OnDeathEvent.IsBound())
		{
			OnDeathEvent.Broadcast();
		}
	}
}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void UCharacterAttributesComponent::DebugDrawAttributes()
{
	if(UDebugSubsystem::GetDebugSubsystem()->IsCategoryEnabled(DebugCategoryCharacterAttributes))
	{
		FVector HealthTextLocation = CachedBaseCharacter->GetActorLocation() + (CachedBaseCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 10.0f) * FVector::UpVector;
		DrawDebugString(GetWorld(), HealthTextLocation, FString::Printf(TEXT("Health: %.2f"), CurrentHealth), nullptr, FColor::Green, 0.0f, true);
		FVector StaminaTextLocation = HealthTextLocation - 10.0f * FVector::UpVector;
		DrawDebugString(GetWorld(), StaminaTextLocation, FString::Printf(TEXT("Stamina: %.2f"), CurrentStamina), nullptr, FColor::Blue, 0.0f, true);
		if (CachedBaseCharacter->GetCharacterMovementComponent()->IsSwimming())
		{
			FVector OxygenTextLocation = StaminaTextLocation - 10.0f * FVector::UpVector;
			DrawDebugString(GetWorld(), OxygenTextLocation, FString::Printf(TEXT("Oxygen: %.2f"), CurrentOxygen), nullptr, FColor::Blue, 0.0f, true);
		}
	}
}
#endif

void UCharacterAttributesComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return;
	}

	SetCurrentHealthClamped(CurrentHealth - Damage);
	UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributesComponent::OnTakeAnyDamage %s received %.2f, amount of damage from %s"), *CachedBaseCharacter->GetName(), Damage, DamageCauser!=nullptr ? *DamageCauser->GetName() : TEXT("nullptr."));
	OnHealthChanged();
}

