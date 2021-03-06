#include "BTService_Fire.h"
#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>
#include "Characters/XYZBaseCharacter.h"
#include <Components/CharacterComponents/CharacterEquipmentComponent.h>
#include <Actors/Equipment/Weapons/RangeWeaponItem.h>

UBTService_Fire::UBTService_Fire()
{
	NodeName = "Fire";
}

void UBTService_Fire::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	AAIController* AIC = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!IsValid(AIC) || !IsValid(Blackboard))
	{
		return;
	}

	AXYZBaseCharacter* Character = Cast<AXYZBaseCharacter>(AIC->GetPawn());
	if (!IsValid(Character))
	{
		return;
	}

	const UCharacterEquipmentComponent* EquipmentComponent = Character->GetCharacterEquipmentComponent();
	ARangeWeaponItem* RangeWeapon = EquipmentComponent->GetCurrentRangeWeapon();

	if (!IsValid(RangeWeapon))
	{
		return;
	}

	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!IsValid(CurrentTarget))
	{
		Character->StopFire();
		return;
	}

	float DistanceSquared = FVector::DistSquared(CurrentTarget->GetActorLocation(), Character->GetActorLocation());
	if (DistanceSquared > FMath::Square(MaxFireDistance))
	{
		Character->StopFire();
		return;
	}

	if (!(RangeWeapon->IsReloading() || RangeWeapon->IsFiring()))
	{
		Character->StartFire();
	}
}
