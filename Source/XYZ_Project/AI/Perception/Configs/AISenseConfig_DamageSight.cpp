// Fill out your copyright notice in the Description page of Project Settings.


#include "AISenseConfig_DamageSight.h"

UAISenseConfig_DamageSight::UAISenseConfig_DamageSight(const FObjectInitializer& ObjectInitializer)
{
	DebugColor = FColor::Red;
}

TSubclassOf<UAISense> UAISenseConfig_DamageSight::GetSenseImplementation() const
{
	return Implementation;
}	

