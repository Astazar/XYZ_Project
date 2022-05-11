#include "AITurretController.h"
#include <Perception/AISense_Sight.h>
#include <Perception/AIPerceptionComponent.h>
#include "AI/Characters/Turret.h"
#include "AI/Perception/Senses/AISense_DamageSight.h"


void AAITurretController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (IsValid(InPawn))
	{
		checkf(InPawn->IsA<ATurret>(), TEXT("AAITurretController::SetPawn AAITurretController can posses only turrets"));
		CachedTurret = StaticCast<ATurret*>(InPawn);
	}
	else
	{
		CachedTurret = nullptr;
	}
}

void AAITurretController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);
	if (!CachedTurret.IsValid())
	{
		return;
	}

	TArray<AActor*> DamageInstigators;
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_DamageSight::StaticClass(), DamageInstigators);
	for (AActor* DamageInstigator : DamageInstigators)
	{
		if (IsValid(DamageInstigator))
		{
			CachedTurret->CurrentTarget = DamageInstigator;
			CachedTurret->OnCurrentTargetSet();
			return;
		}
	}

	AActor* ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	CachedTurret->CurrentTarget = ClosestActor;
	CachedTurret->OnCurrentTargetSet();
}

