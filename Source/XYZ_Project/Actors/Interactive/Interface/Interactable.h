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
	DECLARE_MULTICAST_DELEGATE(FOnInteraction);

	virtual void Interact(AXYZBaseCharacter* BaseCharacter) PURE_VIRTUAL(IInteractable::Interact,);
	virtual FName GetActionEventName() const PURE_VIRTUAL(IInteractable::GetActionEventName, return FName(NAME_None););
	virtual bool HasOnInteractionCallback() const PURE_VIRTUAL(IInteractable::HasOnInteractionCallback, return false;);
	virtual FDelegateHandle AddOnInteractionUFunction(UObject* Object, const FName & FunctionName) PURE_VIRTUAL(IInteractable::AddOnInteractionUFunction, return FDelegateHandle(););
	virtual void RemoveOnInteractionDelegate(FDelegateHandle DelegateHandle) PURE_VIRTUAL(IInteractable::RemoveOnInteractionDelegate,);
};
