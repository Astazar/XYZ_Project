// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributesComponent.h"
#include "XYZ_Project/Characters/XYZBaseCharacter.h"
#include "XYZ_Project/Subsystems/DebugSubsystem.h"
#include "XYZ_Project/XYZ_ProjectTypes.h"
#include <DrawDebugHelpers.h>
#include <Components/CapsuleComponent.h>

UCharacterAttributesComponent::UCharacterAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UCharacterAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<AXYZBaseCharacter>(), TEXT("UCharacterAttributesComponent::BeginPlay UCharacterAttributesComponent can be used only with AXYZBaseCharacter"));
	CachedBaseCharacter = StaticCast<AXYZBaseCharacter*>(GetOwner());
	Health = MaxHealth;
	CachedBaseCharacter->OnTakeAnyDamage.AddDynamic(this, &UCharacterAttributesComponent::OnTakeAnyDamage);
}

void UCharacterAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttribute();
#endif
}


#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void UCharacterAttributesComponent::DebugDrawAttribute()
{
	if(UDebugSubsystem::GetDebugSubsystem()->IsCategoryEnabled(DebugCategoryCharacterAttributes))
	{
		FVector TextLocation = CachedBaseCharacter->GetActorLocation() + (CachedBaseCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 5.0f) * FVector::UpVector;
		DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("Health: %.2f"), Health), nullptr, FColor::Green, 0.0f, true);
	}
}
#endif

void UCharacterAttributesComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributesComponent::OnTakeAnyDamage %s received %.2f, amount of damage from %s"), *CachedBaseCharacter->GetName(), Damage, *DamageCauser->GetName());

	if (Health <= 0.0f)
	{	
		UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributesComponent::OnTakeAnyDamage character %s is killed by %s"), *CachedBaseCharacter->GetName(), *DamageCauser->GetName());
		if (OnDeathEvent.IsBound())
		{
			OnDeathEvent.Broadcast();
		}
	}
}

