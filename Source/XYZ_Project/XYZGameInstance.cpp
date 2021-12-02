// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZGameInstance.h"


bool UXYZGameInstance::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	bool bResult = Super::ProcessConsoleExec(Cmd, Ar, Executor);
	if (!bResult)
	{
		TArray<UGameInstanceSubsystem*> Subsystems = GetSubsystemArray<UGameInstanceSubsystem>();
		for (UGameInstanceSubsystem* Sybsystem : Subsystems)
		{
			bResult |= Sybsystem->ProcessConsoleExec(Cmd,Ar, Executor);
		}
	}
	return bResult;
}
