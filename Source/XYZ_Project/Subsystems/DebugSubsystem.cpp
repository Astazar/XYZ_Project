// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugSubsystem.h"
#include <Kismet/GameplayStatics.h>
#include <DrawDebugHelpers.h>

bool UDebugSubsystem::IsCategoryEnabled(const FName& CategoryName) const
{
	const bool* bIsEnabled = EnabledDebugCategories.Find(CategoryName);

#if ENABLE_DRAW_DEBUG
	return bIsEnabled != nullptr && *bIsEnabled;
#else	
	return false;
#endif
}

UDebugSubsystem* UDebugSubsystem::GetDebugSubsystem()
{
	UWorld* World = GEngine->GameViewport->GetWorld();
	return UGameplayStatics::GetGameInstance(World)->GetSubsystem<UDebugSubsystem>();
}

void UDebugSubsystem::EnableDebugCategory(const FName& CategoryName, bool bIsEnabled)
{
	EnabledDebugCategories.FindOrAdd(CategoryName);
	EnabledDebugCategories[CategoryName] = bIsEnabled;
}