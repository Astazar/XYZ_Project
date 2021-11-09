// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZBasePawnAnimInstance.h"
#include "../XYZ_ProjectBasePawn.h"
#include "../../Components/MovementComponents/XYZBasePawnMovementComponent.h"

void UXYZBasePawnAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AXYZ_ProjectBasePawn>(), TEXT("UXYZBasePawnAnimInstance::NativeBeginPlay. AXYZ_ProjectBasePawn can work only with UXYZBasePawnAnimInstance"));
	CachedBasePawn = StaticCast<AXYZ_ProjectBasePawn*>(TryGetPawnOwner());
}

void UXYZBasePawnAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CachedBasePawn.IsValid())
	{
		return;
	}
	InputForward = CachedBasePawn->GetInputForward();
	InputRight = CachedBasePawn->GetInputRight();
	bIsInAir =  CachedBasePawn->GetMovementComponent()->IsFalling();


}
