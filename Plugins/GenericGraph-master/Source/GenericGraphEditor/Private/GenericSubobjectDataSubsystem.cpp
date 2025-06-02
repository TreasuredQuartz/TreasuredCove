// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "GenericSubobjectDataSubsystem.h"
#include "GenericGraphNode.h"
#include "GenericGraphNodeComponent.h"

#define LOCTEXT_NAMESPACE "GenericSubobjectDataSubsytem"

DEFINE_LOG_CATEGORY_STATIC(LogGenericSubobjectSubsystem, Log, All);

//////////////////////////////////////////////
// USubobjectDataSubsystem

void UGenericSubobjectDataSubsystem::GatherGenericSubobjectData(UObject* Context, TArray<FSubobjectDataHandle>& OutArray) 
{
	OutArray.Reset();

	auto CreateSubobjectData = [](UObject* Context, const FSubobjectDataHandle& ParentHandle = FSubobjectDataHandle::InvalidHandle, bool bIsInheritedSCS = false)
	{
		TSharedPtr<FSubobjectData> SharedPtr = TSharedPtr<FSubobjectData>(new FSubobjectData(Context, ParentHandle, bIsInheritedSCS));

		if (!SharedPtr.IsValid())
		{
			ensureMsgf(false, TEXT("The subobject data factories failed to create subobject data!"));
			SharedPtr = TSharedPtr<FSubobjectData>(new FSubobjectData(Context, ParentHandle, bIsInheritedSCS));
		}

		check(SharedPtr.IsValid());

		// SharedPtr->Handle.DataPtr = SharedPtr;

		return SharedPtr->GetHandle();
	};

	FSubobjectDataHandle RootHandle = CreateSubobjectData(Context);
	OutArray.Add(RootHandle);
	const FSubobjectData* RootActorDataPtr = RootHandle.GetData();
	check(RootActorDataPtr);
	auto FactoryCreateSubobjectDataWithParent = [CreateSubobjectData](UGenericGraphNodeComponent* Context, FSubobjectDataHandle ParentHandle, bool bIsInheritedSCS = false)
	{
		FSubobjectData* ParentData = ParentHandle.GetSharedDataPtr().Get();
		if (!ensureMsgf(ParentData, TEXT("Attempted to use an invalid parent subobject handle!")))
		{
			return FSubobjectDataHandle::InvalidHandle;
		}

		// Does this parent subobject already have this object listed in the children?
		// if yes, then just return that handle
		const FSubobjectDataHandle& ExistingChild = ParentData->FindChildByObject(Context);
		if (ExistingChild.IsValid())
		{
			return ExistingChild;
		}

		// Otherwise, we need to create a new handle
		FSubobjectDataHandle OutHandle = CreateSubobjectData(Context, ParentHandle, bIsInheritedSCS);

		// Inform the parent that it has a new child
		// const bool bSuccess = ParentData->AddChildHandleOnly(OutHandle);
		// ensureMsgf(bSuccess, TEXT("Failed to add a child to parent subobject!"));

		return OutHandle;
	};

	const UGenericGraphNode* NodeContext = Cast<UGenericGraphNode>(Context);
	// Get the full set of instanced components
	TSet<UGenericGraphNodeComponent*> ComponentsToAdd(NodeContext->GetComponents());

	// Now add any remaining instanced owned components not already added above. This will first add any
	// unattached scene components followed by any instanced non-scene components owned by the Actor instance.
	for (UGenericGraphNodeComponent* ActorComp : ComponentsToAdd)
	{
		// Create new subobject data with the original data as their parent.
		OutArray.Add(FactoryCreateSubobjectDataWithParent(ActorComp, RootHandle));
	}
}

#undef LOCTEXT_NAMESPACE
