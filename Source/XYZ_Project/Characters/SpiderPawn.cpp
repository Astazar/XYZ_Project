// Fill out your copyright notice in the Description page of Project Settings.


#include "SpiderPawn.h"
#include <Components/SkeletalMeshComponent.h>
#include <Kismet/KismetSystemLibrary.h>

ASpiderPawn::ASpiderPawn()
{
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SpiderMesh"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	IKScale = GetActorScale().Z;
	IKTraceDistance = CollisionSphereRadius*2,5 * IKScale;
}

void ASpiderPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	IKRightFrontFootOffset = FMath::FInterpTo(IKRightFrontFootOffset, GetIKOffsetForASocket(RightFrontFootSocketName), DeltaTime , IKInterpSpeed);
	IKRightRearFootOffset = FMath::FInterpTo(IKRightRearFootOffset, GetIKOffsetForASocket(RightRearFootSocketName), DeltaTime, IKInterpSpeed);
	IKLeftFrontFootOffset = FMath::FInterpTo(IKLeftFrontFootOffset, GetIKOffsetForASocket(LeftFrontFootSocketName), DeltaTime, IKInterpSpeed);
	IKLeftRearFootOffset = FMath::FInterpTo(IKLeftRearFootOffset, GetIKOffsetForASocket(LeftRearFootSocketName), DeltaTime, IKInterpSpeed);
}

float ASpiderPawn::GetIKOffsetForASocket(const FName& SocketName)
{
	float Result = 0.0f;
	FVector SocketLocation = SkeletalMeshComponent->GetSocketLocation(SocketName);
	FVector TraceStart(SocketLocation.X,SocketLocation.Y,GetActorLocation().Z);
	FVector TraceEnd = TraceStart - IKTraceDistance * FVector::UpVector;
	float StandartHigh = TraceStart.Z - CollisionSphereRadius;
	FHitResult HitResult;
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, TraceType, true, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult, true))
	{
		Result = (StandartHigh - HitResult.Location.Z) / IKScale;
	}
	
	return Result;
}
