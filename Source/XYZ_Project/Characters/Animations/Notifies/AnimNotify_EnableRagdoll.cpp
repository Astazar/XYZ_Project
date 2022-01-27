#include "AnimNotify_EnableRagdoll.h"
#include "XYZ_Project/XYZ_ProjectTypes.h"



void UAnimNotify_EnableRagdoll::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	MeshComp->SetCollisionProfileName(CollisionProfileRagdoll);
	MeshComp->SetSimulatePhysics(true);
}

