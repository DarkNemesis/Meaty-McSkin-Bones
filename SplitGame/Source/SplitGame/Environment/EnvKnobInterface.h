#pragma once
#include "CoreMinimal.h"
#include "EnvKnobInterface.generated.h"

UINTERFACE(BlueprintType)
class UEnvKnobInterface : public UInterface
{
	GENERATED_BODY()
};

class IEnvKnobInterface
{
	GENERATED_BODY()
public:
	/** React to a trigger volume activating this object. Return true if the reaction succeeds. */
	UFUNCTION(Category = "Knob Activation")
		virtual bool KnobActivated();

	UFUNCTION(Category = "Knob Deactivation")
		virtual bool KnobDeactivated();
};