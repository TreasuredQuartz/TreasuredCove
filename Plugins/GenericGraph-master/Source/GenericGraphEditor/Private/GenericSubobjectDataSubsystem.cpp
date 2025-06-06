// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "GenericSubobjectDataSubsystem.h"
#include "GenericGraphNode.h"
#include "GenericGraphNodeComponent.h"

#include "GenericSubobjectData.h"
#include "GenericSubobjectDataHandle.h"

#define LOCTEXT_NAMESPACE "GenericSubobjectDataSubsytem"

DEFINE_LOG_CATEGORY_STATIC(LogGenericSubobjectSubsystem, Log, All);

//////////////////////////////////////////////
// USubobjectDataSubsystem

void UGenericSubobjectDataSubsystem::GatherGenericSubobjectData(UObject* Context, TArray<FGenericSubobjectDataHandle>& OutArray) 
{
	OutArray.Reset();

	auto CreateSubobjectData = [](UObject* Context, const FGenericSubobjectDataHandle& ParentHandle = FGenericSubobjectDataHandle::InvalidHandle, bool bIsInheritedSCS = false)
	{
		TSharedPtr<FGenericSubobjectData> SharedPtr = TSharedPtr<FGenericSubobjectData>(new FGenericSubobjectData(Context, ParentHandle, bIsInheritedSCS));

		if (!SharedPtr.IsValid())
		{
			ensureMsgf(false, TEXT("The subobject data fNodeies failed to create subobject data!"));
			SharedPtr = TSharedPtr<FGenericSubobjectData>(new FGenericSubobjectData(Context, ParentHandle, bIsInheritedSCS));
		}

		check(SharedPtr.IsValid());

		// SharedPtr->Handle.DataPtr = SharedPtr;

		return SharedPtr->GetHandle();
	};

	FGenericSubobjectDataHandle RootHandle = CreateSubobjectData(Context);
	OutArray.Add(RootHandle);
	const FGenericSubobjectData* RootNodeDataPtr = RootHandle.GetData();
	check(RootNodeDataPtr);
	auto FNodeyCreateSubobjectDataWithParent = [CreateSubobjectData](UGenericGraphNodeComponent* Context, FGenericSubobjectDataHandle ParentHandle, bool bIsInheritedSCS = false)
	{
		FGenericSubobjectData* ParentData = ParentHandle.GetSharedDataPtr().Get();
		if (!ensureMsgf(ParentData, TEXT("Attempted to use an invalid parent subobject handle!")))
		{
			return FGenericSubobjectDataHandle::InvalidHandle;
		}

		// Does this parent subobject already have this object listed in the children?
		// if yes, then just return that handle
		const FGenericSubobjectDataHandle& ExistingChild = ParentData->FindChildByObject(Context);
		if (ExistingChild.IsValid())
		{
			return ExistingChild;
		}

		// Otherwise, we need to create a new handle
		FGenericSubobjectDataHandle OutHandle = CreateSubobjectData(Context, ParentHandle, bIsInheritedSCS);

		// Inform the parent that it has a new child
		// const bool bSuccess = ParentData->AddChildHandleOnly(OutHandle);
		// ensureMsgf(bSuccess, TEXT("Failed to add a child to parent subobject!"));

		return OutHandle;
	};

	const UGenericGraphNode* NodeContext = Cast<UGenericGraphNode>(Context);
	// Get the full set of instanced components
	TSet<UGenericGraphNodeComponent*> ComponentsToAdd(NodeContext->GetComponents());

	// Now add any remaining instanced owned components not already added above. This will first add any
	// unattached scene components followed by any instanced non-scene components owned by the Node instance.
	for (UGenericGraphNodeComponent* NodeComp : ComponentsToAdd)
	{
		// Create new subobject data with the original data as their parent.
		OutArray.Add(FNodeyCreateSubobjectDataWithParent(NodeComp, RootHandle));
	}
}

FGenericSubobjectDataHandle UGenericSubobjectDataSubsystem::FindHandleForObject(const FGenericSubobjectDataHandle& Context, const UObject* ObjectToFind) const
{
	if (Context.IsValid())
	{
		if (const UGenericGraphNodeComponent* ComponentToFind = Cast<UGenericGraphNodeComponent>(ObjectToFind))
		{
			UClass* OwnerClass = ComponentToFind->GetOwner()->GetClass();

			// Get the class default object
			const UGenericGraphNode* CDO = Cast<UGenericGraphNode>(OwnerClass->GetDefaultObject());
			if (CDO)
			{
				// Iterate over the Components array and attempt to find a component with a matching name
				for (UGenericGraphNodeComponent* ComponentTemplate : CDO->GetComponents())
				{
					if (ComponentTemplate && ComponentTemplate->GetFName() == ComponentToFind->GetFName())
					{
						// We found a match; redirect to the component archetype instance that may be associated with a tree node
						ObjectToFind = ComponentTemplate;
						break;
					}
				}
			}
		}

		TSet<FGenericSubobjectData*> OutData;
		FindAllSubobjectData(Context.GetSharedDataPtr().Get(), OutData);

		for (FGenericSubobjectData* CurData : OutData)
		{
			if (CurData->GetObject() == ObjectToFind)
			{
				return CurData->GetHandle();
			}
		}
	}

	return FGenericSubobjectDataHandle::InvalidHandle;
}

void UGenericSubobjectDataSubsystem::FindAllSubobjectData(FGenericSubobjectData* InData, TSet<FGenericSubobjectData*>& OutVisited) const
{
	if (!InData || OutVisited.Contains(InData))
	{
		return;
	}

	OutVisited.Add(InData);

	for (FGenericSubobjectDataHandle ChildHandle : InData->GetChildrenHandles())
	{
		FindAllSubobjectData(ChildHandle.GetData(), OutVisited);
	}
}

FGenericSubobjectDataHandle UGenericSubobjectDataSubsystem::AddNewSubobject(const FAddNewGenericSubobjectParams& Params, FText& FailReason)
{
	FGenericSubobjectDataHandle NewDataHandle = FGenericSubobjectDataHandle::InvalidHandle;

	/*
	// Check for valid parent
	if (!Params.ParentHandle.IsValid() || !Params.NewClass)
	{
		return NewDataHandle;
	}

	UClass* NewClass = Params.NewClass;
	UObject* Asset = Params.AssetOverride;
	const FGenericSubobjectDataHandle& ParentObjHandle = Params.ParentHandle;

	if (NewClass->ClassWithin && NewClass->ClassWithin != UObject::StaticClass())
	{
		FailReason = LOCTEXT("AddComponentFailed", "Cannot add components that have \"Within\" markup");
		return NewDataHandle;
	}

	// Ensure that the new class is actually a valid subobject type.
	// As of right now, that means the class has to be a child of an Actor Component. 
	if (!NewClass->IsChildOf(UGenericGraphNodeComponent::StaticClass()))
	{
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("ClassName"), NewClass->GetDisplayNameText());
		FailReason = FText::Format(LOCTEXT("AddComponentFailed_InvalidClassType", "Cannot add a subobject of class '{ClassName}'"), Arguments);
		return NewDataHandle;
	}

	FName TemplateVariableName;
	UGenericGraphNodeComponent* ComponentTemplate = Cast<UGenericGraphNodeComponent>(Asset);

	if (ComponentTemplate)
	{
		Asset = nullptr;
	}

	{
		FGenericSubobjectData* ParentObjData = ParentObjHandle.GetData();

		check(ParentObjData);

		// If this is a component template, then we can just duplicate it
		if (ComponentTemplate)
		{
			UGenericGraphNodeComponent* NewComponent = FComponentEditorUtils::DuplicateComponent(ComponentTemplate);
			// Create a new subobject data set with this component
			NewDataHandle = FactoryCreateSubobjectDataWithParent(NewComponent, ParentObjData->GetHandle());
		}
		else
		{
			AActor* ActorInstance = ParentObjData->GetMutableActorContext();

			// If we can't find an actor instance then search up the hierarchy for one that we can use instead
			if (!ActorInstance)
			{
				const FGenericSubobjectDataHandle& ActorRootHandle = GetActorRootHandle(ParentObjData->GetHandle());
				if (FGenericSubobjectData* ActorInstanceData = ActorRootHandle.GetData())
				{
					ActorInstance = ActorInstanceData->GetMutableActorContext();
				}
			}

			if (ActorInstance)
			{
				ActorInstance->Modify();

				// Create an appropriate name for the new component
				FName NewComponentName = NAME_None;
				if (Asset)
				{
					NewComponentName = *FComponentEditorUtils::GenerateValidVariableNameFromAsset(Asset, ActorInstance);
				}
				else
				{
					NewComponentName = *FComponentEditorUtils::GenerateValidVariableName(NewClass, ActorInstance);
				}

				// Get the set of owned components that exists prior to instancing the new component.
				TInlineComponentArray<UGenericGraphNodeComponent*> PreInstanceComponents;
				ActorInstance->GetComponents(PreInstanceComponents);

				// Construct the new component and attach as needed
				UGenericGraphNodeComponent* NewInstanceComponent = NewObject<UGenericGraphNodeComponent>(ActorInstance, NewClass, NewComponentName, RF_Transactional);

				// Do Scene Attachment if this new Component is a USceneComponent
				if (USceneComponent* NewSceneComponent = Cast<USceneComponent>(NewInstanceComponent))
				{
					// If this is an actor with no subobjects on it, then set the new subobject as scene root of the actor.
					// This can occur if the user has placed in a native C++ class to the world with no subobjects on it
					if ((ParentObjData->IsActor() && ParentObjData->GetChildrenHandles().IsEmpty()) ||
						ActorInstance->GetRootComponent() == nullptr)
					{
						ActorInstance->SetRootComponent(NewSceneComponent);
					}
					else
					{
						USceneComponent* AttachTo = Cast<USceneComponent>(ParentObjData->GetMutableComponentTemplate());
						if (AttachTo && AttachTo->IsTemplate())
						{
							UGenericGraphNodeComponent** ArchetypeAttachment = PreInstanceComponents.FindByPredicate([AttachTo](const UGenericGraphNodeComponent* A)
							{
								return A && A->GetArchetype() == AttachTo;
							});

							AttachTo = ArchetypeAttachment ? CastChecked<USceneComponent>(*ArchetypeAttachment) : nullptr;
						}

						if (AttachTo == nullptr)
						{
							AttachTo = ActorInstance->GetRootComponent();
						}
						check(AttachTo != nullptr);

						// Make sure that the mobility of the new scene component is such that we can attach it
						if (AttachTo->Mobility == EComponentMobility::Movable)
						{
							NewSceneComponent->Mobility = EComponentMobility::Movable;
						}
						else if (AttachTo->Mobility == EComponentMobility::Stationary && NewSceneComponent->Mobility == EComponentMobility::Static)
						{
							NewSceneComponent->Mobility = EComponentMobility::Stationary;
						}

						NewSceneComponent->AttachToComponent(AttachTo, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
					}
				}

				// If the component was created from/for a particular asset, assign it now
				if (Asset)
				{
					FComponentAssetBrokerage::AssignAssetToComponent(NewInstanceComponent, Asset);
				}

				// Add to SerializedComponents array so it gets saved
				ActorInstance->AddInstanceComponent(NewInstanceComponent);
				NewInstanceComponent->OnComponentCreated();
				NewInstanceComponent->RegisterComponent();

				// Register any new components that may have been created during construction of the instanced component, but were not explicitly registered.
				TInlineComponentArray<UGenericGraphNodeComponent*> PostInstanceComponents;
				ActorInstance->GetComponents(PostInstanceComponents);
				for (UGenericGraphNodeComponent* ActorComponent : PostInstanceComponents)
				{
					if (!ActorComponent->IsRegistered() && ActorComponent->bAutoRegister && IsValidChecked(ActorComponent) && !PreInstanceComponents.Contains(ActorComponent))
					{
						ActorComponent->RegisterComponent();
					}
				}

				// Rerun construction scripts
				ActorInstance->RerunConstructionScripts();

				// If the running the construction script destroyed the new node, don't create an entry for it
				if (IsValidChecked(NewInstanceComponent))
				{
					// Create a new subobject data set with this component 
					NewDataHandle = FactoryCreateSubobjectDataWithParent(NewInstanceComponent, ParentObjData->GetHandle());
				}

				BroadcastInstanceChanges();
			}
			else
			{
				FailReason = LOCTEXT("AddComponentFailed_Inherited", "Cannot add components within an Inherited hierarchy");
			}
		}
	}

	// If the new handle is valid, broadcast to any listeners that a new subobject was added.
	if (const FGenericSubobjectData* NewData = NewDataHandle.GetData())
	{
		OnNewSubobjectAdded_Delegate.Broadcast(*NewData);
	} */

	return NewDataHandle;
}

int32 UGenericSubobjectDataSubsystem::DeleteSubobjects(const FGenericSubobjectDataHandle& ContextHandle, const TArray<FGenericSubobjectDataHandle>& SubobjectsToDelete, FGenericSubobjectDataHandle& OutComponentToSelect, bool bForce /* = false */)
{
	int32 NumDeletedSubobjects = 0;

	if (!ContextHandle.IsValid() || SubobjectsToDelete.Num() == 0)
	{
		return NumDeletedSubobjects;
	}
	const FGenericSubobjectData* ContextData = ContextHandle.GetData();
	UObject* ContextObj = ContextData->GetMutableObject();
	check(ContextObj);

	ContextObj->Modify();

	TArray<UGenericGraphNodeComponent*> ComponentsToDelete;
	for (const FGenericSubobjectDataHandle& Handle : SubobjectsToDelete)
	{
		if (Handle.IsValid())
		{
			if (const FGenericSubobjectData* Data = Handle.GetData())
			{
				if (!Data->CanDelete())
				{
					UE_LOG(LogGenericSubobjectSubsystem, Warning, TEXT("Cannot delete subobject '%s' from '%s'!"), *Data->GetDisplayString(), *ContextObj->GetFullName());
					continue;
				}

				if (UGenericGraphNodeComponent* Template = Data->GetMutableComponentTemplate())
				{
					ComponentsToDelete.Add(Template);
				}
			}
		}
	}

	// Actually delete the components if we have any that can be
	if (!ComponentsToDelete.IsEmpty())
	{
		UGenericGraphNodeComponent* NodeComponentToSelect = nullptr;

		auto DeleteComponents = [](const TArray<UGenericGraphNodeComponent*>& ComponentsToDelete, UGenericGraphNodeComponent*& OutComponentToSelect)
		{
			int32 NumDeletedComponents = 0;

			TArray<UGenericGraphNode*> NodesToReconstruct;

			for (UGenericGraphNodeComponent* ComponentToDelete : ComponentsToDelete)
			{
				if (ComponentToDelete->CreationMethod != EComponentCreationMethod::Instance)
				{
					// We can only delete instance components, so retain selection on the un-deletable component
					OutComponentToSelect = ComponentToDelete;
					continue;
				}

				UGenericGraphNode* Owner = ComponentToDelete->GetOwner();
				check(Owner != nullptr);

				// If necessary, determine the component that should be selected following the deletion of the indicated component
				if (!OutComponentToSelect || ComponentToDelete == OutComponentToSelect)
				{
					// For a non-scene component, try to select the preceding non-scene component
					for (UGenericGraphNodeComponent* Component : Owner->GetComponents())
					{
						if (Component != nullptr)
						{
							if (Component == ComponentToDelete)
							{
								break;
							}
						}
					}
				}

				// Defer reconstruction
				NodesToReconstruct.AddUnique(Owner);

				// Actually delete the component
				ComponentToDelete->Modify();
				ComponentToDelete->DestroyComponent();
				NumDeletedComponents++;
			}

			// Non-native components will be reinstanced, so we have to update the ptr after reconstruction
			// in order to avoid pointing at an invalid (trash) instance after re-running construction scripts.
			FName ComponentToSelectName;
			const UGenericGraphNode* ComponentToSelectOwner = nullptr;
			if (OutComponentToSelect && OutComponentToSelect->CreationMethod != EComponentCreationMethod::Native)
			{
				// Keep track of the pending selection's name and owner
				ComponentToSelectName = OutComponentToSelect->GetFName();
				ComponentToSelectOwner = OutComponentToSelect->GetOwner();

				// Reset the ptr value - we'll reassign it after reconstruction
				OutComponentToSelect = nullptr;
			}

			// Reconstruct owner instance(s) after deletion
			for (UGenericGraphNode* NodeToReconstruct : NodesToReconstruct)
			{
				check(NodeToReconstruct != nullptr);

				// If this Node matches the owner of the component to be selected, find the new instance of the component in the Node
				if (ComponentToSelectName != NAME_None && OutComponentToSelect == nullptr && NodeToReconstruct == ComponentToSelectOwner)
				{
					TInlineComponentArray<UGenericGraphNodeComponent*> NodeComponents;
					NodeToReconstruct->GetComponents(NodeComponents);
					for (UGenericGraphNodeComponent* NodeComponent : NodeComponents)
					{
						if (NodeComponent->GetFName() == ComponentToSelectName)
						{
							OutComponentToSelect = NodeComponent;
							break;
						}
					}
				}
			}

			return NumDeletedComponents;
		};

		// FComponentEditorUtils::DeleteComponents
		auto UtilityDeleteComponents = [](const TArray<UGenericGraphNodeComponent*>& ComponentsToDelete, UGenericGraphNodeComponent*& OutComponentToSelect)
		{
			int32 NumDeletedComponents = 0;

			TArray<UGenericGraphNode*> ActorsToReconstruct;

			for (UGenericGraphNodeComponent* ComponentToDelete : ComponentsToDelete)
			{
				if (ComponentToDelete->CreationMethod != EComponentCreationMethod::Instance)
				{
					// We can only delete instance components, so retain selection on the un-deletable component
					OutComponentToSelect = ComponentToDelete;
					continue;
				}

				UGenericGraphNode* Owner = ComponentToDelete->GetOwner();
				check(Owner != nullptr);

				// If necessary, determine the component that should be selected following the deletion of the indicated component
				if (!OutComponentToSelect || ComponentToDelete == OutComponentToSelect)
				{
					// For a non-scene component, try to select the preceding non-scene component
					for (UGenericGraphNodeComponent* Component : Owner->GetComponents())
					{
						if (Component != nullptr)
						{
							if (Component == ComponentToDelete)
							{
								break;
							}
						}
					}
				}

				// Defer reconstruction
				ActorsToReconstruct.AddUnique(Owner);

				// Actually delete the component
				ComponentToDelete->Modify();
				ComponentToDelete->DestroyComponent();
				NumDeletedComponents++;
			}

			// Non-native components will be reinstanced, so we have to update the ptr after reconstruction
			// in order to avoid pointing at an invalid (trash) instance after re-running construction scripts.
			FName ComponentToSelectName;
			const UGenericGraphNode* ComponentToSelectOwner = nullptr;
			if (OutComponentToSelect && OutComponentToSelect->CreationMethod != EComponentCreationMethod::Native)
			{
				// Keep track of the pending selection's name and owner
				ComponentToSelectName = OutComponentToSelect->GetFName();
				ComponentToSelectOwner = OutComponentToSelect->GetOwner();

				// Reset the ptr value - we'll reassign it after reconstruction
				OutComponentToSelect = nullptr;
			}

			// Reconstruct owner instance(s) after deletion
			for (UGenericGraphNode* ActorToReconstruct : ActorsToReconstruct)
			{
				check(ActorToReconstruct != nullptr);

				// If this actor matches the owner of the component to be selected, find the new instance of the component in the actor
				if (ComponentToSelectName != NAME_None && OutComponentToSelect == nullptr && ActorToReconstruct == ComponentToSelectOwner)
				{
					TInlineComponentArray<UGenericGraphNodeComponent*> ActorComponents;
					ActorToReconstruct->GetComponents(ActorComponents);
					for (UGenericGraphNodeComponent* ActorComponent : ActorComponents)
					{
						if (ActorComponent->GetFName() == ComponentToSelectName)
						{
							OutComponentToSelect = ActorComponent;
							break;
						}
					}
				}
			}

			return NumDeletedComponents;
		};

		NumDeletedSubobjects = UtilityDeleteComponents(ComponentsToDelete, NodeComponentToSelect);
		if (NodeComponentToSelect)
		{
			OutComponentToSelect = FindHandleForObject(ContextHandle, NodeComponentToSelect);
		}

		// BroadcastInstanceChanges();
	}

	return NumDeletedSubobjects;
}

int32 UGenericSubobjectDataSubsystem::DeleteSubobjects(const FGenericSubobjectDataHandle& ContextHandle, const TArray<FGenericSubobjectDataHandle>& SubobjectsToDelete)
{
	FGenericSubobjectDataHandle Dummy;
	return DeleteSubobjects(ContextHandle, SubobjectsToDelete, Dummy);
}

bool UGenericSubobjectDataSubsystem::CanCopySubobjects(const TArray<FGenericSubobjectDataHandle>& Handles) const
{
	return false;
}

void UGenericSubobjectDataSubsystem::CopySubobjects(const TArray<FGenericSubobjectDataHandle>& Handles)
{
}

bool UGenericSubobjectDataSubsystem::CanPasteSubobjects(const FGenericSubobjectDataHandle& RootHandle) const
{
	return false;
}

void UGenericSubobjectDataSubsystem::PasteSubobjects(const FGenericSubobjectDataHandle& PasteToContext, const TArray<FGenericSubobjectDataHandle>& NewParentHandles, TArray<FGenericSubobjectDataHandle>& OutPastedHandles)
{
}

void UGenericSubobjectDataSubsystem::DuplicateSubobjects(const FGenericSubobjectDataHandle& Context, const TArray<FGenericSubobjectDataHandle>& SubobjectsToDup, TArray<FGenericSubobjectDataHandle>& OutNewSubobjects)
{
}

#undef LOCTEXT_NAMESPACE
