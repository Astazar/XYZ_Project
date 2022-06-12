#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HighlightInteractable.generated.h"


class UTextBlock;
UCLASS()
class XYZ_PROJECT_API UHighlightInteractable : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetActionText(FName KeyName);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ActionText;
};