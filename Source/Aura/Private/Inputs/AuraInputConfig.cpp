// Copyright OneLuckyDad

#include "Inputs/AuraInputConfig.h"
#include "InputAction.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FAuraInputAction& Action : AbilityInputActions)
	{
		if (Action.InputTag == InputTag && Action.InputAction)
		{
			return Action.InputAction;
		}
	}
	
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find AbilityInputAction for Input Tag [%s], on InputConfig [%s]"), *InputTag.ToString(), *GetName());
	}
	
	return nullptr;
}
