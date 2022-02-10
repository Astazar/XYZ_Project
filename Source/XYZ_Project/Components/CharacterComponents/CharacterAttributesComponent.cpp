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

UCharacterAttributesComponent::UCharacterAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UCharacterAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<AXYZBaseCharacter>(), TEXT("UCharacterAttributesComponent::BeginPlay UCharacterAttributesComponent can be used only with AXYZBaseCharacter"));
	CachedBaseCharacter = StaticCast<AXYZBaseCharacter*>(GetOwner());
	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
	CurrentOxygen = MaxOxygen;
	CachedBaseCharacter->OnTakeAnyDamage.AddDynamic(this, &UCharacterAttributesComponent::OnTakeAnyDamage);
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
		CurrentStamina += StaminaRestoreVelocity * DeltaSeconds;
		CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
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
		CurrentStamina -= SprintStaminaConsumptionVelocity * DeltaSeconds;
		CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
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
	if (CurrentSwimState == ESwimState::None)
	{
		return;
	}

	float OxygenDelta = CurrentSwimState == ESwimState::OnWaterSurface ? OxygenRestoreVelocity : -OxygenConsumptionVelocity;
	CurrentOxygen = FMath::Clamp(CurrentOxygen + OxygenDelta * DeltaTime, 0.0f, MaxOxygen);
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

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributesComponent::OnTakeAnyDamage %s received %.2f, amount of damage from %s"), *CachedBaseCharacter->GetName(), Damage, DamageCauser!=nullptr ? *DamageCauser->GetName() : TEXT("nullptr."));

	if (CurrentHealth <= 0.0f)
	{	
		UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributesComponent::OnTakeAnyDamage character %s is killed by %s"), *CachedBaseCharacter->GetName(), DamageCauser != nullptr ? *DamageCauser->GetName() : TEXT("nullptr."));
		if (OnDeathEvent.IsBound())
		{
			OnDeathEvent.Broadcast();
		}
	}
}

