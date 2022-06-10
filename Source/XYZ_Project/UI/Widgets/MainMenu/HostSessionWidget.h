#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/MainMenu/NetworkWidget.h"
#include "HostSessionWidget.generated.h"


UCLASS()
class XYZ_PROJECT_API UHostSessionWidget : public UNetworkWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network session")
	FName ServerName;

	UFUNCTION(BlueprintCallable)
	void CreateSession();
};
