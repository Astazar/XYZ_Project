// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformInvocator.h"

// Sets default values
APlatformInvocator::APlatformInvocator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	USceneComponent* DefaultInvocatorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("InvocatorRoot"));
	RootComponent = DefaultInvocatorRoot;
}

void APlatformInvocator::Invoke()
{
	if (OnInvocatorActivated.IsBound())
	{
		OnInvocatorActivated.Broadcast();
	}
	else GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue,TEXT("Noone is bound"));
}



