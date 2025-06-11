// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "GenericSubobjectEditor/GenericSubobjectDataSubsystem.h"
#include "GenericGraphNode.h"
#include "GenericGraphNodeComponent.h"

#include "GenericSubobjectEditor/GenericSubobjectData.h"
// #include "GenericSubobjectEditor/GenericSubobjectDataHandle.h" // Included in header file.
#include "kismet2/KismetEditorUtilities.h"
#include "Exporters/Exporter.h"
#include "UnrealExporter.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Factories.h"

#define LOCTEXT_NAMESPACE "GenericSubobjectDataSubsytem"

DEFINE_LOG_CATEGORY_STATIC(LogGenericSubobjectSubsystem, Log, All);

//////////////////////////////////////////////
// USubobjectDataSubsystem

void UGenericSubobjectDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
}

void UGenericSubobjectDataSubsystem::Deinitialize()
{
}

UGenericSubobjectDataSubsystem* UGenericSubobjectDataSubsystem::Get()
{
	return GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>();
}

void UGenericSubobjectDataSubsystem::GatherGenericSubobjectData(UObject* Context, TArray<FGenericSubobjectDataHandle>& OutArray) 
{
	OutArray.Reset();

	/*FGenericSubobjectDataHandle RootHandle = CreateSubobjectData(Context);
	OutArray.Add(RootHandle);
	const FGenericSubobjectData* RootNodeDataPtr = RootHandle.GetData();
	check(RootNodeDataPtr); // */

	const UGenericGraphNode* NodeContext = Cast<UGenericGraphNode>(Context);
	// Get the full set of instanced components
	TSet<UGenericGraphNodeComponent*> ComponentsToAdd(NodeContext->GetComponents());

	// Now add any remaining instanced owned components not already added above. This will first add any
	// unattached scene components followed by any instanced non-scene components owned by the Node instance.
	for (UGenericGraphNodeComponent* NodeComp : ComponentsToAdd)
	{
		// Create new subobject data with the original data as their parent.
		OutArray.Add(CreateSubobjectData(NodeComp));
	}
}

FGenericSubobjectDataHandle UGenericSubobjectDataSubsystem::FindHandleForObject(const FGenericSubobjectDataHandle& Context, const UObject* ObjectToFind) const
{
	if (Context.IsValid())
	{
		if (const UGenericGraphNodeComponent* ComponentToFind = Cast<UGenericGraphNodeComponent>(ObjectToFind))
		{
			// Get the component owner's class object
			check(ComponentToFind->GetOwner() != nullptr);
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

bool UGenericSubobjectDataSubsystem::IsValidRename(const FGenericSubobjectDataHandle& Handle, const FText& InNewName, FText& OutErrorMessage) const
{
	const FGenericSubobjectData* Data = Handle.GetData();
	if (!Data)
	{
		return false;
	}

	const UBlueprint* Blueprint = nullptr; // Data->GetBlueprint();
	const FString& NewTextStr = InNewName.ToString();

	/* if (!NewTextStr.IsEmpty())
	{
		if (Data->GetVariableName().ToString() == NewTextStr)
		{
			return true;
		}

		if (const UGenericGraphNodeComponent* ComponentInstance = Data->GetComponentTemplate())
		{
			UGenericGraphNode* ExistingNameSearchScope = ComponentInstance->GetOwner();
			if ((ExistingNameSearchScope == nullptr) && (Blueprint != nullptr))
			{
				ExistingNameSearchScope = Cast<UGenericGraphNode>(Blueprint->GeneratedClass->GetDefaultObject());
			}

			if (!FComponentEditorUtils::IsValidVariableNameString(ComponentInstance, NewTextStr))
			{
				OutErrorMessage = LOCTEXT("RenameFailed_EngineReservedName", "This name is reserved for engine use.");
				return false;
			}
			else if (NewTextStr.Len() >= NAME_SIZE)
			{
				FFormatNamedArguments Arguments;
				Arguments.Add(TEXT("CharCount"), NAME_SIZE - 1);
				OutErrorMessage = FText::Format(LOCTEXT("ComponentRenameFailed_TooLong", "Component name must be less than {CharCount} characters long."), Arguments);
				return false;
			}
			else if (!FComponentEditorUtils::IsComponentNameAvailable(NewTextStr, ExistingNameSearchScope, ComponentInstance)
				|| !FComponentEditorUtils::IsComponentNameAvailable(NewTextStr, ComponentInstance->GetOuter(), ComponentInstance))
			{
				OutErrorMessage = LOCTEXT("RenameFailed_ExistingName", "Another component already has the same name.");
				return false;
			}
		}
		else if (const UGenericGraphNode* ActorInstance = Data->GetObject<UGenericGraphNode>())
		{
			// #TODO_BH Validation of actor instance
		}
		else
		{
			OutErrorMessage = LOCTEXT("RenameFailed_InvalidComponentInstance", "This node is referencing an invalid component instance and cannot be renamed. Perhaps it was destroyed?");
			return false;
		}
	} */

	TSharedPtr<INameValidatorInterface> NameValidator;
	if (Blueprint != nullptr)
	{
		NameValidator = MakeShareable(new FKismetNameValidator(Blueprint, Data->GetVariableName()));
	}
	else if (const UGenericGraphNodeComponent* CompTemplate = Data->GetComponentTemplate())
	{
		NameValidator = MakeShareable(new FStringSetNameValidator(CompTemplate->GetName()));
	}

	if (NameValidator)
	{
		EValidatorResult ValidatorResult = NameValidator->IsValid(NewTextStr);
		if (ValidatorResult == EValidatorResult::AlreadyInUse)
		{
			OutErrorMessage = FText::Format(LOCTEXT("RenameFailed_InUse", "{0} is in use by another variable or function!"), InNewName);
		}
		else if (ValidatorResult == EValidatorResult::EmptyName)
		{
			OutErrorMessage = LOCTEXT("RenameFailed_LeftBlank", "Names cannot be left blank!");
		}
		else if (ValidatorResult == EValidatorResult::TooLong)
		{
			OutErrorMessage = LOCTEXT("RenameFailed_NameTooLong", "Names must have fewer than 100 characters!");
		}
	}

	if (OutErrorMessage.IsEmpty())
	{
		return true;
	}

	return false;
}

bool UGenericSubobjectDataSubsystem::RenameSubobject(const FGenericSubobjectDataHandle& Handle, const FText& InNewName)
{
	FText OutErrorMessage;
	if (!IsValidRename(Handle, InNewName, OutErrorMessage))
	{
		return false;
	}

	const FGenericSubobjectData* Data = Handle.GetData();
	if (!Data)
	{
		return false;
	}

	// For components: either instanced or simple construction script ("Add Component" in Blueprint editor)
	if (UGenericGraphNodeComponent* ComponentInstance = Data->GetMutableComponentTemplate())
	{
		auto GetBlueprint = [](const FGenericSubobjectData* InData)
		{
			UBlueprint* BP = nullptr;

			// If this object is a BP, we can just return that
			if (UBlueprint* NewBP = Cast<UBlueprint>(InData->WeakObjectPtr.Get()))
			{
				BP = NewBP;
			}
			// If it is an actor, then we can get the BP from the UClass
			else if (const AActor* DefaultActor = InData->GetObject<AActor>())
			{
				BP = UBlueprint::GetBlueprintFromClass(DefaultActor->GetClass());
			}
			// For components, we need to get the blueprint from their owning actor or the SCS
			else if (InData->IsComponent())
			{
				/*if (const USCS_Node* SCS_Node = InData->GetSCSNode())
				{
					if (const USimpleConstructionScript* SCS = SCS_Node->GetSCS())
					{
						return SCS->GetBlueprint();
					}
				}
				else*/ if (const UGenericGraphNodeComponent* ActorComponent = InData->GetComponentTemplate())
				{
					if (const UGenericGraphNode* Actor = ActorComponent->GetOwner())
					{
						BP = UBlueprint::GetBlueprintFromClass(Actor->GetClass());
					}
				}
			}

			return BP;
		};

		UBlueprint* BP = GetBlueprint(Data);
		const FString DesiredName = InNewName.ToString();
		auto ValidateNameInBP = [BP](const FString& DesiredName)
		{
			return FKismetNameValidator(BP).IsValid(DesiredName) == EValidatorResult::Ok
				? FName(DesiredName)
				: FName(DesiredName); // FBlueprintEditorUtils::FindUniqueKismetName(BP, DesiredName);
		};

		// When placed in level, just the component needs to be renamed.
		if (Data->IsInstancedComponent())
		{
			// UBlueprint instance required may not always be available, e.g. if we added a non-Blueprint C++ class into the level like AStaticMeshActor.
			// If they are available, use FKismetNameValidator and otherwise we'll just do our best here and replace invalid characters.
			const FString NewNameAsString = BP
				? ValidateNameInBP(DesiredName).ToString()
				: DesiredName; // FBlueprintEditorUtils::ReplaceInvalidBlueprintNameCharactersInline(DesiredName);

			// Name collision could occur due to e.g. our archetype being updated and causing a conflict with our ComponentInstance:
			if (StaticFindObject(UObject::StaticClass(), ComponentInstance->GetOuter(), *NewNameAsString) == nullptr)
			{
				const ERenameFlags RenameFlags = REN_DontCreateRedirectors;
				ComponentInstance->Rename(*NewNameAsString, nullptr, RenameFlags);
			}

			return true;
		}

		// In Blueprint editor, the component variable name must be updated.
		if (BP)
		{
			// Is this desired name the same as what is already there? If so then don't bother.
			/*USCS_Node* SCSNode = Data->GetSCSNode();
			if (SCSNode && SCSNode->GetVariableName().ToString().Equals(DesiredName))
			{
				return true;
			}

			const FName ValidatedNewName = ValidateNameInBP(DesiredName);
			FBlueprintEditorUtils::RenameComponentMemberVariable(BP, SCSNode, ValidatedNewName);
			return true;*/
		}
	}

	return false;
}

bool UGenericSubobjectDataSubsystem::CanCopySubobjects(const TArray<FGenericSubobjectDataHandle>& Handles) const
{
	TArray<UGenericGraphNodeComponent*> ComponentsToCopy;

	for (const FGenericSubobjectDataHandle& Handle : Handles)
	{
		if (FGenericSubobjectData* Data = Handle.GetData())
		{
			if (!Data->CanCopy())
			{
				return false;
			}

			// Get the component template associated with the selected node
			if (UGenericGraphNodeComponent* ComponentTemplate = Data->GetMutableComponentTemplate())
			{
				ComponentsToCopy.Add(ComponentTemplate);
			}
		}
	}

	// Verify that the components can be copied
	// return FComponentEditorUtils::CanCopyComponents(ComponentsToCopy);
	bool bCanCopy = ComponentsToCopy.Num() > 0;
	if (bCanCopy)
	{
		for (int32 i = 0; i < ComponentsToCopy.Num() && bCanCopy; ++i)
		{
			// Check for the default scene root; that cannot be copied/duplicated
			UGenericGraphNodeComponent* Component = ComponentsToCopy[i];
			UClass* ComponentClass = Component->GetClass();
			bCanCopy = FKismetEditorUtilities::IsClassABlueprintSpawnableComponent(ComponentClass);
		}
	}

	return bCanCopy;
}

void UGenericSubobjectDataSubsystem::CopySubobjects(const TArray<FGenericSubobjectDataHandle>& Handles)
{
	if (!CanCopySubobjects(Handles))
	{
		return;
	}

	TArray<UGenericGraphNodeComponent*> ComponentsToCopy;

	for (const FGenericSubobjectDataHandle& Handle : Handles)
	{
		if (FGenericSubobjectData* Data = Handle.GetData())
		{
			ensureMsgf(Data->CanCopy(), TEXT("A non-copiable subobject has been allowed to copy!"));

			// Get the component template associated with the selected node
			if (UGenericGraphNodeComponent* ComponentTemplate = Data->GetMutableComponentTemplate())
			{
				ComponentsToCopy.Add(ComponentTemplate);
			}
		}
	}

	// Copy the components to the clipboard
	// FComponentEditorUtils::CopyComponents(ComponentsToCopy);

	FStringOutputDevice Archive;

	// Clear the mark state for saving.
	UnMarkAllObjects(EObjectMark(OBJECTMARK_TagExp | OBJECTMARK_TagImp));

	// Duplicate the selected component templates into temporary objects that we can modify
	TMap<FName, FName> ParentMap;
	TMap<FName, UGenericGraphNodeComponent*> ObjectMap;
	for (UGenericGraphNodeComponent* Component : ComponentsToCopy)
	{
		// Duplicate the component into a temporary object
		UObject* DuplicatedComponent = StaticDuplicateObject(Component, GetTransientPackage());
		if (DuplicatedComponent)
		{
			// Record the temporary object into the name->object map
			ObjectMap.Add(Component->GetFName(), CastChecked<UGenericGraphNodeComponent>(DuplicatedComponent));
		}
	}

	const FExportObjectInnerContext Context;

	// Export the component object(s) to text for copying
	for (const TPair<FName, UGenericGraphNodeComponent*>& ObjectPair : ObjectMap)
	{
		// Get the component object to be copied
		UGenericGraphNodeComponent* ComponentToCopy = ObjectPair.Value;
		check(ComponentToCopy);

		// Export the component object to the given string
		UExporter::ExportToOutputDevice(&Context, ComponentToCopy, NULL, Archive, TEXT("copy"), 0, PPF_ExportsNotFullyQualified | PPF_Copy | PPF_Delimited, false, ComponentToCopy->GetOuter());
	}

	// Copy text to clipboard
	{
		FPlatformApplicationMisc::ClipboardCopy(*Archive);
	}
}

// Text object factory for pasting components
struct FGenericComponentObjectTextFactory : public FCustomizableTextObjectFactory
{
	// Child->Parent name map
	TMap<FName, FName> ParentMap;

	// Name->Instance object mapping
	TMap<FName, UGenericGraphNodeComponent*> NewObjectMap;

	// Constructs a new object factory from the given text buffer
	static TSharedRef<FGenericComponentObjectTextFactory> Get(const FString& InTextBuffer, bool bPasteAsArchetypes = false)
	{
		// Construct a new instance
		TSharedPtr<FGenericComponentObjectTextFactory> FactoryPtr = MakeShareable(new FGenericComponentObjectTextFactory());
		check(FactoryPtr.IsValid());

		// Create new objects if we're allowed to
		if (FactoryPtr->CanCreateObjectsFromText(InTextBuffer))
		{
			EObjectFlags ObjectFlags = RF_Transactional;
			if (bPasteAsArchetypes)
			{
				ObjectFlags |= RF_ArchetypeObject | RF_Public;
			}

			// Use the transient package initially for creating the objects, since the variable name is used when copying
			FactoryPtr->ProcessBuffer(GetTransientPackage(), ObjectFlags, InTextBuffer);
		}

		return FactoryPtr.ToSharedRef();
	}

	virtual ~FGenericComponentObjectTextFactory() {}

protected:
	// Constructor; protected to only allow this class to instance itself
	FGenericComponentObjectTextFactory()
		:FCustomizableTextObjectFactory(GWarn)
	{
	}

	// FCustomizableTextObjectFactory implementation

	virtual bool CanCreateClass(UClass* ObjectClass, bool& bOmitSubObjs) const override
	{
		// Allow actor component types to be created
		bool bCanCreate = ObjectClass && ObjectClass->IsChildOf(UActorComponent::StaticClass());

		if (!bCanCreate)
		{
			// Also allow Blueprint-able actor types to pass, in order to enable proper creation of actor component types as subobjects. The actor instance will be discarded after processing.
			bCanCreate = ObjectClass && ObjectClass->IsChildOf(AActor::StaticClass()) && FKismetEditorUtilities::CanCreateBlueprintOfClass(ObjectClass);
		}
		else
		{
			// Actor component classes should not be abstract and must also be tagged as BlueprintSpawnable
			bCanCreate = FKismetEditorUtilities::IsClassABlueprintSpawnableComponent(ObjectClass);
		}

		return bCanCreate;
	}

	virtual void ProcessConstructedObject(UObject* NewObject) override
	{
		check(NewObject);

		TInlineComponentArray<UGenericGraphNodeComponent*> ActorComponents;
		if (UGenericGraphNodeComponent* NewActorComponent = Cast<UGenericGraphNodeComponent>(NewObject))
		{
			ActorComponents.Add(NewActorComponent);
		}
		else if (UGenericGraphNode* NewActor = Cast<UGenericGraphNode>(NewObject))
		{
			NewActor->GetComponents(ActorComponents);
		}

		for (UGenericGraphNodeComponent* ActorComponent : ActorComponents)
		{
			// Add it to the new object map
			NewObjectMap.Add(ActorComponent->GetFName(), ActorComponent);
		}
	}

	// FCustomizableTextObjectFactory (end)
};

bool UGenericSubobjectDataSubsystem::CanPasteSubobjects(const FGenericSubobjectDataHandle& RootHandle) const
{
	const FGenericSubobjectData* RootData = RootHandle.GetData();
	const UGenericGraphNodeComponent* SceneComponent = RootData->GetComponentTemplate();
	const bool bPasteAsArchetypes = true;

	FString ClipboardContent;
	{
		FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);
	}

	// Obtain the component object text factory for the clipboard content and return whether or not we can use it
	TSharedRef<FGenericComponentObjectTextFactory> Factory = FGenericComponentObjectTextFactory::Get(ClipboardContent, bPasteAsArchetypes);

	return (Factory->NewObjectMap.Num() > 0);
}

void UGenericSubobjectDataSubsystem::PasteSubobjects(const FGenericSubobjectDataHandle& PasteToContext, const TArray<FGenericSubobjectDataHandle>& NewParentHandles, TArray<FGenericSubobjectDataHandle>& OutPastedHandles)
{
	if (!PasteToContext.IsValid() || NewParentHandles.IsEmpty())
	{
		return;
	}

	FGenericSubobjectData* PasteToContextData = PasteToContext.GetSharedDataPtr().Get();

	const FScopedTransaction Transaction(LOCTEXT("PasteComponents", "Paste Component(s)"));
	
	if (UGenericGraphNode* ActorContext = PasteToContextData->GetMutableObject<UGenericGraphNode>())
	{
		// Paste the components. This uses the current data from the clipboard
		// FComponentEditorUtils::PasteComponents(PastedComponents, ActorContext, TargetComponent);
		TArray<UGenericGraphNodeComponent*> PastedComponents;
		auto PasteComponents = [](TArray<UGenericGraphNodeComponent*>& OutPastedComponents, UGenericGraphNode* TargetActor)
		{
			check(TargetActor);

			// Get the text from the clipboard
			FString TextToImport;
			FPlatformApplicationMisc::ClipboardPaste(TextToImport);

			// Get a new component object factory for the clipboard content
			TSharedRef<FGenericComponentObjectTextFactory> Factory = FGenericComponentObjectTextFactory::Get(TextToImport);

			TargetActor->Modify();

			// FComponentEditorUtils::GenerateValidVariableNameFromAsset
			auto GenerateName = [](UObject* Asset, UObject* ComponentOwner)
			{
				int32 Counter = 1;
				FString AssetName = Asset->GetName();

				if (UClass* Class = Cast<UClass>(Asset))
				{
					if (!Class->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
					{
						AssetName.RemoveFromEnd(TEXT("Component"));
					}
					else
					{
						AssetName.RemoveFromEnd("_C");
					}
				}
				else if (UGenericGraphNodeComponent* Comp = Cast <UGenericGraphNodeComponent>(Asset))
				{
					// AssetName.RemoveFromEnd(UGenericGraphNodeComponent::ComponentTemplateNameSuffix);
				}

				// Try to create a name without any numerical suffix first
				FString NewName = AssetName;

				auto BuildNewName = [&Counter, &AssetName]()
				{
					return FString::Printf(TEXT("%s%d"), *AssetName, Counter++);
				};

				if (ComponentOwner)
				{
					// If a desired name is supplied then walk back and find any numeric suffix so we can increment it nicely
					int32 Index = AssetName.Len();
					while (Index > 0 && AssetName[Index - 1] >= '0' && AssetName[Index - 1] <= '9')
					{
						--Index;
					}

					if (Index < AssetName.Len())
					{
						FString NumericSuffix = AssetName.RightChop(Index);
						Counter = FCString::Atoi(*NumericSuffix);
						NumericSuffix = FString::Printf(TEXT("%d"), Counter); // Restringify the counter to account for leading 0s that we don't want to remove
						AssetName.RemoveAt(AssetName.Len() - NumericSuffix.Len(), NumericSuffix.Len(), EAllowShrinking::No);
						++Counter;
						NewName = BuildNewName();
					}

					auto IsComponentNameAvailable = [](FString InString, UObject* ComponentOwner, UObject* ComponentToIgnore = nullptr)
					{
						UObject* Object = FindObjectFast<UObject>(ComponentOwner, *InString);

						bool bNameIsAvailable = Object == nullptr || Object == ComponentToIgnore;

						return bNameIsAvailable;
					};

					while (!IsComponentNameAvailable(NewName, ComponentOwner))
					{
						NewName = BuildNewName();
					}
				}

				return NewName;
			};


			for (const TPair<FName, UGenericGraphNodeComponent*>& NewObjectPair : Factory->NewObjectMap)
			{
				// Get the component object instance
				UGenericGraphNodeComponent* NewActorComponent = NewObjectPair.Value;
				check(NewActorComponent);

				// Relocate the instance from the transient package to the Actor and assign it a unique object name
				FString NewComponentName = GenerateName(NewActorComponent, TargetActor);
				NewActorComponent->Rename(*NewComponentName, TargetActor, REN_DontCreateRedirectors | REN_DoNotDirty);

				TargetActor->AddInstanceComponent(NewActorComponent);
				NewActorComponent->RegisterComponent();

				OutPastedComponents.Add(NewActorComponent);
			}

			// Rerun construction scripts
			// TargetActor->RerunConstructionScripts();
		};

		PasteComponents(PastedComponents, ActorContext);

		// Create handles for each pasted subobject, attaching it to it's parent as necessary
		for (UGenericGraphNodeComponent* PastedComponent : PastedComponents)
		{
			FGenericSubobjectDataHandle PastedHandle = CreateSubobjectData(PastedComponent);
			if (PastedHandle.IsValid())
			{
				OutPastedHandles.AddUnique(PastedHandle);
			}
		}
	}
}

void UGenericSubobjectDataSubsystem::DuplicateSubobjects(const FGenericSubobjectDataHandle& Context, const TArray<FGenericSubobjectDataHandle>& SubobjectsToDup, TArray<FGenericSubobjectDataHandle>& OutNewSubobjects)
{
}

FGenericSubobjectDataHandle UGenericSubobjectDataSubsystem::CreateSubobjectData(UObject* Context, const FGenericSubobjectDataHandle& ParentHandle /*= FGenericSubobjectDataHandle::InvalidHandle*/)
{
	TSharedPtr<FGenericSubobjectData> SharedPtr = TSharedPtr<FGenericSubobjectData>(new FGenericSubobjectData(Context, ParentHandle));

	if (!SharedPtr.IsValid())
	{
		ensureMsgf(false, TEXT("The subobject data fNodeies failed to create subobject data!"));
		SharedPtr = TSharedPtr<FGenericSubobjectData>(new FGenericSubobjectData(Context, ParentHandle));
	}

	check(SharedPtr.IsValid());

	SharedPtr->Handle.DataPtr = SharedPtr;

	return SharedPtr->GetHandle();
}

FGenericSubobjectDataHandle UGenericSubobjectDataSubsystem::FactoryCreateSubobjectDataWithParent(UObject* Context, const FGenericSubobjectDataHandle& ParentHandle)
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
	FGenericSubobjectDataHandle OutHandle = CreateSubobjectData(Context);

	return OutHandle;
}

void UGenericSubobjectDataSubsystem::SelectSubobjects(const TArray<FGenericSubobjectDataHandle>& SelectedSubobjects) const
{
	OnSubobjectSelectedDelegate.ExecuteIfBound(SelectedSubobjects);
}

#undef LOCTEXT_NAMESPACE
