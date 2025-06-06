// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "GenericGraphNodeComponent.h"
#include "GenericGraphNode.h"


#define LOCTEXT_NAMESPACE "GenericGraphNodeComponent"

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

void UGenericGraphNodeComponent::ExecuteUnregisterEvents()
{
	if (bRegistered)
	{
		OnUnregister();
	}
}

void UGenericGraphNodeComponent::OnComponentCreated()
{
	ensure(!bHasBeenCreated);
	bHasBeenCreated = true;
}

void UGenericGraphNodeComponent::RegisterComponent()
{
}

void UGenericGraphNodeComponent::UnregisterComponent()
{
	// Do nothing if not registered
	if (!IsRegistered())
	{
		return;
	}

	ExecuteUnregisterEvents();
}

void UGenericGraphNodeComponent::EndPlay(EEndPlayReason::Type Reason)
{
	bIsReadyForReplication = false;
	bHasBegunPlay = false;
}

void UGenericGraphNodeComponent::DestroyComponent()
{
	// Avoid re-entrancy
	if (bIsBeingDestroyed)
	{
		return;
	}

	bIsBeingDestroyed = true;

	if (bHasBegunPlay)
	{
		EndPlay(EEndPlayReason::Destroyed);
	}

	// Ensure that we call UninitializeComponent before we destroy this component
	if (bHasBeenInitialized)
	{
		UninitializeComponent();
	}

	bIsReadyForReplication = false;

	// Unregister if registered
	if (IsRegistered())
	{
		UnregisterComponent();
	}

	// Then remove from Components array, if we have an Actor
	if (UGenericGraphNode* MyOwner = GetOwner())
	{
		MyOwner->RemoveOwnedComponent(this);
	}

	// Tell the component it is being destroyed
	OnComponentDestroyed();

	// Finally mark pending kill, to NULL out any other refs
	MarkAsGarbage();
}

void UGenericGraphNodeComponent::OnComponentDestroyed()
{
	bHasBeenCreated = false;
}

#undef LOCTEXT_NAMESPACE
