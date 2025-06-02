// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "GenericGraphNodeComponent.h"

bool UGenericGraphNodeComponent::IsEditableWhenInherited() const
{
	bool bCanEdit = bEditableWhenInherited;
	if (bCanEdit)
	{
#if WITH_EDITOR
		if (CreationMethod == EComponentCreationMethod::Native && !IsTemplate())
		{
			// bCanEdit = FComponentEditorUtils::GetPropertyForEditableNativeComponent(this) != nullptr;
		}
		else
#endif
			if (CreationMethod == EComponentCreationMethod::UserConstructionScript)
			{
				bCanEdit = false;
			}
	}
	return bCanEdit;
}
