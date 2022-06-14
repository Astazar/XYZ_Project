#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "XYZAttributeProgressBar.generated.h"


class UProgressBar;
UCLASS()
class XYZ_PROJECT_API UXYZAttributeProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetProgressPercentage(float Percentage);

	void OnProgressChanged(float CurrentValue, float MaxValue);

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* AttributeProgressBar;
	
};
