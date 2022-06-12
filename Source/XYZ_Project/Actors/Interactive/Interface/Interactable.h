#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"


class AXYZBaseCharacter;
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class XYZ_PROJECT_API IInteractable
{
	GENERATED_BODY()

public:
	virtual void Interact(AXYZBaseCharacter* BaseCharacter) PURE_VIRTUAL(IInteractable::Interact,);
	virtual FName GetActionEventName() const PURE_VIRTUAL(IInteractable::Interact, return FName(NAME_None););
};
