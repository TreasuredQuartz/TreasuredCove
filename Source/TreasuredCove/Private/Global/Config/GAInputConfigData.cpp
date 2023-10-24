#include "GAInputConfigData.h"
#include "InputAction.h"
#include "GameplayTagContainer.h"

const UInputAction* UAbilityInputConfigData::FindInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FTaggedInputAction& TaggedInputAction : TaggedInputActions)
	{
		if (TaggedInputAction.InputAction && TaggedInputAction.InputTag == InputTag)
		{
			return TaggedInputAction.InputAction;
		}
	}

	return nullptr;
}
