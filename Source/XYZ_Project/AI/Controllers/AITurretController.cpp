#include "AITurretController.h"
#include <Perception/AISense_Sight.h>
#include <Perception/AIPerceptionComponent.h>
#include "AI/Characters/Turret.h"
#include "AI/Perception/Senses/AISense_DamageSight.h"

AAITurretController::AAITurretController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("TurretPerceptionComponent"));
}

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
	if (!CachedTurret.IsValid())
	{
		return;
	}

	TArray<AActor*> SeenActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), SeenActors);

	AActor* ClosestActor = nullptr;
	float MinSquaredDistance = FLT_MAX;
	FVector TurretLocation = CachedTurret->GetActorLocation();

	for (AActor* SeenActor : SeenActors)
	{
		float CurrentSquaredDistance = (TurretLocation - SeenActor->GetActorLocation()).SizeSquared();
		if (CurrentSquaredDistance < MinSquaredDistance)
		{
			MinSquaredDistance = CurrentSquaredDistance;
			ClosestActor = SeenActor;
		}
	}

	TArray<AActor*> DamageInstigators;
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_DamageSight::StaticClass(), DamageInstigators);
	for (AActor* DamageInstigator : DamageInstigators)
	{
		if (IsValid(DamageInstigator))
		{
			CachedTurret->SetCurrentTarget(DamageInstigator);
			return;
		}
	}

	CachedTurret->SetCurrentTarget(ClosestActor);
}

