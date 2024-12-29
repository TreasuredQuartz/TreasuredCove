// Copywrite notice


#include "Global/Config/GAInputConfigData.h"
#include "InputAction.h"
#include "GameplayTagContainer.h"

const UInputAction* UAbilityInputConfigData::FindInputActionForTag(const FGameplayTag& InputTag) const
{
	if (TaggedInputActions.IsEmpty()) return nullptr;

	for (const FTaggedInputAction& TaggedInputAction : TaggedInputActions)
	{
		if (TaggedInputAction.InputAction && TaggedInputAction.InputTag == InputTag)
		{
			return TaggedInputAction.InputAction;
		}
	}

	return nullptr;
}
