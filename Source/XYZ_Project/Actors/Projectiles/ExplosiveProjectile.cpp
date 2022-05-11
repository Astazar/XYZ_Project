#include "ExplosiveProjectile.h"
#include "Components/ExplosionComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AExplosiveProjectile::AExplosiveProjectile()
{
	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>(TEXT("ExplosionComponent"));
	ExplosionComponent->SetupAttachment(RootComponent);
}

void AExplosiveProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (ExplosionType == EExplosionType::ByHit)
	{
		ProjectileMovementComponent->bShouldBounce = false;
		OnProjectileHit.AddDynamic(this, &AExplosiveProjectile::OnExplosiveProjectileHit);
	}
}

void AExplosiveProjectile::OnProjectileLaunched()
{
	Super::OnProjectileLaunched();
	if (ExplosionType == EExplosionType::ByTimer)
	{
		GetWorldTimerManager().SetTimer(DetonationTimer, this, &AExplosiveProjectile::OnDetonationTimerElapsed, DetonationTime, false);
	}
}

void AExplosiveProjectile::OnDetonationTimerElapsed()
{
	ExplosionComponent->Explode(GetController());
}

void AExplosiveProjectile::OnExplosiveProjectileHit(AXYZProjectile* Projectile, const FHitResult& HitResult, const FVector& Direction, float ShotRange)
{
	ExplosionComponent->Explode(GetController());
}

AController* AExplosiveProjectile::GetController()
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;
}
