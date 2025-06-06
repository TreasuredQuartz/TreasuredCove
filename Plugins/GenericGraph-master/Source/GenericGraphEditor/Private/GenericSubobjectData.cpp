// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "GenericSubobjectData.h"
#include "GenericSubobjectDataHandle.h"
#include "GenericGraphNode.h"
#include "GenericGraphNodeComponent.h"

#include "Kismet2/KismetEditorUtilities.h"

#define LOCTEXT_NAMESPACE "GenericSubobjectData"

FGenericSubobjectData::FGenericSubobjectData()
	: WeakObjectPtr(nullptr)
	, ParentObjectHandle(FGenericSubobjectDataHandle::InvalidHandle)
{
	// By Default the handle will be invalid, it will only
	// be generated if we are given an object.
}

FGenericSubobjectData::FGenericSubobjectData(UObject* ContextObject, const FGenericSubobjectDataHandle& InParentHandle, const bool bInIsInheritedSCS)
	: WeakObjectPtr(ContextObject)
	, ParentObjectHandle(InParentHandle)
{
	if (UGenericGraphNodeComponent* Component = Cast<UGenericGraphNodeComponent>(ContextObject))
	{
		// Create an inherited subobject data
		if (bInIsInheritedSCS || Component->CreationMethod != EComponentCreationMethod::Instance)
		{
			bIsInheritedSubobject = true;
		}
	}
}

FProperty* GetPropertyForEditableNativeComponent(const UGenericGraphNodeComponent* NativeComponent)
{
	// A native component can be edited if it is bound to a member variable and that variable is marked as visible in the editor
	// Note: We aren't concerned with whether the component is marked editable - the component itself is responsible for determining which of its properties are editable	
	UObject* ComponentOuter = (NativeComponent ? NativeComponent->GetOuter() : nullptr);
	UClass* OwnerClass = (ComponentOuter ? ComponentOuter->GetClass() : nullptr);

	if (OwnerClass != nullptr)
	{
		for (TFieldIterator<FObjectProperty> It(OwnerClass); It; ++It)
		{
			FObjectProperty* ObjectProp = *It;

			// Must be visible - note CPF_Edit is set for all properties that should be visible, not just those that are editable
			if ((ObjectProp->PropertyFlags & (CPF_Edit)) == 0)
			{
				continue;
			}

			UObject* Object = ObjectProp->GetObjectPropertyValue(ObjectProp->ContainerPtrToValuePtr<void>(ComponentOuter));
			if (Object != nullptr && Object->GetFName() == NativeComponent->GetFName())
			{
				return ObjectProp;
			}
		}

		// We have to check for array properties as well because they are not FObjectProperties and we want to be able to
		// edit the inside of it
		if (ComponentOuter != nullptr)
		{
			for (TFieldIterator<FArrayProperty> PropIt(OwnerClass, EFieldIteratorFlags::IncludeSuper); PropIt; ++PropIt)
			{
				FArrayProperty* TestProperty = *PropIt;
				void* ArrayPropInstAddress = TestProperty->ContainerPtrToValuePtr<void>(ComponentOuter);

				// Ensure that this property is valid
				FObjectProperty* ArrayEntryProp = CastField<FObjectProperty>(TestProperty->Inner);
				if ((ArrayEntryProp == nullptr) || !ArrayEntryProp->PropertyClass->IsChildOf<UGenericGraphNodeComponent>() || ((TestProperty->PropertyFlags & CPF_Edit) == 0))
				{
					continue;
				}

				// For each object in this array
				FScriptArrayHelper ArrayHelper(TestProperty, ArrayPropInstAddress);
				for (int32 ComponentIndex = 0; ComponentIndex < ArrayHelper.Num(); ++ComponentIndex)
				{
					// If this object is the native component we are looking for, then we should be allowed to edit it
					const uint8* ArrayValRawPtr = ArrayHelper.GetRawPtr(ComponentIndex);
					UObject* ArrayElement = ArrayEntryProp->GetObjectPropertyValue(ArrayValRawPtr);

					if (ArrayElement != nullptr && ArrayElement->GetFName() == NativeComponent->GetFName())
					{
						return ArrayEntryProp;
					}
				}
			}

			// Check for map properties as well
			for (TFieldIterator<FMapProperty> PropIt(OwnerClass, EFieldIteratorFlags::IncludeSuper); PropIt; ++PropIt)
			{
				FMapProperty* TestProperty = *PropIt;
				void* MapPropInstAddress = TestProperty->ContainerPtrToValuePtr<void>(ComponentOuter);

				// Ensure that this property is valid and that it is marked as visible in the editor
				FObjectProperty* MapValProp = CastField<FObjectProperty>(TestProperty->ValueProp);
				if ((MapValProp == nullptr) || !MapValProp->PropertyClass->IsChildOf<UGenericGraphNodeComponent>() || ((TestProperty->PropertyFlags & CPF_Edit) == 0))
				{
					continue;
				}

				FScriptMapHelper MapHelper(TestProperty, MapPropInstAddress);
				for (FScriptMapHelper::FIterator It(MapHelper); It; ++It)
				{
					// For each value in the map (don't bother checking the keys, they won't be what the user can edit in this case)
					const uint8* MapValueData = MapHelper.GetValuePtr(It);
					UObject* ValueElement = MapValProp->GetObjectPropertyValue(MapValueData);
					if (ValueElement != nullptr && ValueElement->GetFName() == NativeComponent->GetFName())
					{
						return MapValProp;
					}
				}
			}

			// Finally we should check for set properties
			for (TFieldIterator<FSetProperty> PropIt(OwnerClass, EFieldIteratorFlags::IncludeSuper); PropIt; ++PropIt)
			{
				FSetProperty* TestProperty = *PropIt;
				void* SetPropInstAddress = TestProperty->ContainerPtrToValuePtr<void>(ComponentOuter);

				// Ensure that this property is valid and that it is marked visible
				FObjectProperty* SetValProp = CastField<FObjectProperty>(TestProperty->ElementProp);
				if ((SetValProp == nullptr) || !SetValProp->PropertyClass->IsChildOf<UGenericGraphNodeComponent>() || ((TestProperty->PropertyFlags & CPF_Edit) == 0))
				{
					continue;
				}

				// For each item in the set
				FScriptSetHelper SetHelper(TestProperty, SetPropInstAddress);
				for (FScriptSetHelper::FIterator It(SetHelper); It; ++It)
				{
					const uint8* SetValData = SetHelper.GetElementPtr(It);
					UObject* SetValueElem = SetValProp->GetObjectPropertyValue(SetValData);

					if (SetValueElem != nullptr && SetValueElem->GetFName() == NativeComponent->GetFName())
					{
						return SetValProp;
					}
				}
			}
		}
	}

	return nullptr;
}

bool FGenericSubobjectData::CanEdit() const
{
	if (bIsInheritedSubobject)
	{
		if (IsComponent())
		{
			if (IsInstancedInheritedComponent())
			{
				const UGenericGraphNodeComponent* Template = GetComponentTemplate();
				return (Template ? Template->IsEditableWhenInherited() : false);
			}
			else if (const UGenericGraphNodeComponent* ComponentTemplate = GetComponentTemplate())
			{
				return GetPropertyForEditableNativeComponent(ComponentTemplate) != nullptr;
			}
		}
	}

	// Nodes are editable by default
	if (const UGenericGraphNode* NodeContext = GetObject<UGenericGraphNode>())
	{
		return true;
	}
	// If this is an instance-added component, then we can edit it. We know this isn't inherted because it would
	// be a FInheritedSubobjectData
	else if (const UGenericGraphNodeComponent* Component = GetComponentTemplate())
	{
		if (Component->CreationMethod == EComponentCreationMethod::Instance)
		{
			return true;
		}
	}
	return false;
}

bool FGenericSubobjectData::CanDelete() const
{
	if (bIsInheritedSubobject)
	{
		if (IsInheritedComponent() || (IsDefaultSceneRoot() && SceneRootHasDefaultName()) || IsInstancedInheritedComponent())
		{
			return false;
		}

		return true;
	}

	// Components can be deleted if they are not inherited
	if (const UGenericGraphNodeComponent* ComponentTemplate = GetComponentTemplate())
	{
		// Inherited components cannot be deleted
		if (IsInheritedComponent())
		{
			return false;
		}
		// You can delete the default scene root on instances of Native C++ actors in the level
		// but not inside of a blueprint or on BP created actors
		else if (IsDefaultSceneRoot())
		{
			if (const UGenericGraphNode* Node = ComponentTemplate->GetOwner())
			{
				return Node->GetClass()->IsNative();
			}

			// Otherwise you can't delete the default scene root
			return false;
		}
		else
		{
			// You can delete any instance-added component
			return IsInstancedComponent();
		}
	}

	// Otherwise, it can't be deleted
	return false;
}

bool FGenericSubobjectData::CanDuplicate() const
{
	return CanCopy();
}

bool IsClassABlueprintSpawnableComponent(const UClass* Class)
{
	// @fixme: Cooked packages don't have any metadata (yet; they might become available via the sidecar editor data)
	// However, all uncooked BPs that derive from GenericGraphNodeComponent have the BlueprintSpawnableComponent metadata set on them
	// (see FBlueprintEditorUtils::RecreateClassMetaData), so include any GenericGraphNodeComponent BP that comes from a cooked package
	return (!Class->HasAnyClassFlags(CLASS_Abstract) &&
		Class->IsChildOf<UGenericGraphNodeComponent>()); // &&
		// (Class->HasMetaData(FBlueprintMetadata::MD_BlueprintSpawnableComponent) || Class->GetPackage()->bIsCookedForEditor));
}

bool CanCopyComponent(const UGenericGraphNodeComponent* ComponentToCopy)
{
	if (ComponentToCopy != nullptr)
	{
		UClass* ComponentClass = ComponentToCopy->GetClass();
		check(ComponentClass != nullptr);

		// Component class cannot be abstract and must also be tagged as BlueprintSpawnable
		return IsClassABlueprintSpawnableComponent(ComponentClass);
	}

	return false;
}

bool FGenericSubobjectData::CanCopy() const
{
	if (IsInstancedInheritedComponent())
	{
		return CanCopyComponent(GetComponentTemplate()) && !SceneRootHasDefaultName();
	}

	return CanCopyComponent(GetComponentTemplate());
}

bool FGenericSubobjectData::CanReparent() const
{
	if (IsComponent())
	{
		return !IsInheritedComponent();
	}

	return false;
}

bool FGenericSubobjectData::CanRename() const
{
	if (!IsValid())
	{
		return false;
	}

	// You can rename instance actors in the level editor, but you cannot rename
	// the default actors in blueprints. 
	if (IsNode())
	{
		return true;
	}

	return !IsInheritedComponent() && !IsDefaultSceneRoot();
}

bool FGenericSubobjectData::IsInstancedInheritedComponent() const
{
	if (!IsComponent())
	{
		return false;
	}
	FGenericSubobjectDataHandle CurrentHandle = ParentObjectHandle;
	FGenericSubobjectData* CurrentData = CurrentHandle.GetData();

	while (CurrentHandle.IsValid() && CurrentData && !CurrentData->IsNode())
	{
		CurrentHandle = CurrentData->GetParentHandle();
		CurrentData = CurrentHandle.GetData();
	}

	return CurrentData->IsValid();
}

bool FGenericSubobjectData::IsAttachedTo(const FGenericSubobjectDataHandle& InHandle) const
{
	return false;
}

const UObject* FGenericSubobjectData::GetObjectForBlueprint(UBlueprint* Blueprint) const
{
	const bool bCanEdit = CanEdit();

	// If this not a component, then we can simply return the current object as long as it's editable
	return bCanEdit ? WeakObjectPtr.Get() : nullptr;
}

bool FGenericSubobjectData::IsInstancedComponent() const
{
	// Check the flags on the component (RF_ClassDefaultObject | RF_ArchetypeObject)
	const UGenericGraphNodeComponent* Component = GetComponentTemplate();
	return Component && !Component->IsTemplate();
}

bool FGenericSubobjectData::IsNativeComponent() const
{
	return false;
}

bool FGenericSubobjectData::IsBlueprintInheritedComponent() const
{
	return false;
}

bool FGenericSubobjectData::IsInheritedComponent() const
{
	return false;
}

bool FGenericSubobjectData::IsRootComponent() const
{
	return false;
}

bool FGenericSubobjectData::IsDefaultSceneRoot() const
{
	return false;
}

bool FGenericSubobjectData::SceneRootHasDefaultName() const
{
	return false;
}

bool FGenericSubobjectData::IsComponent() const
{
	return false;
}

bool FGenericSubobjectData::IsNode() const
{
	return false;
}

UGenericGraphNodeComponent* FindMatchingComponent(const UGenericGraphNodeComponent* ComponentInstance, const TInlineComponentArray<UGenericGraphNodeComponent*>& ComponentList)
{
	if (ComponentInstance == nullptr)
	{
		return nullptr;
	}

	TInlineComponentArray<UGenericGraphNodeComponent*> FoundComponents;
	UGenericGraphNodeComponent* LastFoundComponent = nullptr;
	for (UGenericGraphNodeComponent* Component : ComponentList)
	{
		// Early out on pointer match
		if (ComponentInstance == Component)
		{
			return Component;
		}

		if (ComponentInstance->GetFName() == Component->GetFName())
		{
			FoundComponents.Add(Component);
			LastFoundComponent = Component;
		}
	}

	// No match or 1 match avoid sorting
	if (FoundComponents.Num() <= 1)
	{
		return LastFoundComponent;
	}

	return FoundComponents[0];
}

UGenericGraphNodeComponent* FGenericSubobjectData::GetMutableNodeContext()
{
	return nullptr;
}

UGenericGraphNodeComponent* FGenericSubobjectData::FindMutableComponentInstanceInNode(const UGenericGraphNode* InNode) const
{
	const UGenericGraphNodeComponent* ComponentTemplate = GetComponentTemplate();

	UGenericGraphNodeComponent* ComponentInstance = nullptr;
	if (InNode)
	{
		if (ComponentTemplate)
		{
			TInlineComponentArray<UGenericGraphNodeComponent*> Components;
			InNode->GetComponents(Components);
			ComponentInstance = FindMatchingComponent(ComponentTemplate, Components);
		}
	}

	return ComponentInstance;
}

FString FGenericSubobjectData::GetDisplayString(bool bShowNativeComponentNames /* = true */) const
{
	FName VariableName = GetVariableName();

	const UGenericGraphNodeComponent* ComponentTemplate = GetComponentTemplate();

	// Only display SCS node variable names in the tree if they have not been autogenerated
	if (ComponentTemplate != nullptr)
	{
		return ComponentTemplate->GetFName().ToString();
	}
	// If the context is a simple UObject, then we can get it's name
	else if (const UObject* Context = GetObject())
	{
		FString Name;
		Context->GetName(Name);

		return Name;
	}
	// Anything else will be unknown!
	else
	{
		FString UnnamedString = LOCTEXT("UnnamedToolTip", "Unnamed").ToString();
		FString NativeString = IsNativeComponent() ? LOCTEXT("NativeToolTip", "Native ").ToString() : TEXT("");

		if (ComponentTemplate != nullptr)
		{
			return FString::Printf(TEXT("[%s %s%s]"), *UnnamedString, *NativeString, *ComponentTemplate->GetClass()->GetName());
		}
		else
		{
			return FString::Printf(TEXT("[%s %s]"), *UnnamedString, *NativeString);
		}
	}
}

FText FGenericSubobjectData::GetDragDropDisplayText() const
{
	return FText::FromString(GetDisplayString());
}

FText FGenericSubobjectData::GetDisplayNameContextModifiers(bool bShowNativeComponentNames) const
{
	if (IsNode())
	{
		if (const AActor* DefaultActor = GetObject<AActor>())
		{
			if (const UBlueprint* Blueprint = UBlueprint::GetBlueprintFromClass(DefaultActor->GetClass()))
			{
				return LOCTEXT("ActorContext_self", "(Self)");
			}
			else
			{
				return LOCTEXT("ActorContext_Instance", "(Instance)");
			}
		}
	}
	else if (const UGenericGraphNodeComponent* Template = GetComponentTemplate())
	{
		FName VariableName = GetVariableName();
		const bool bIsBlueprintInstanceInherited = IsInstancedInheritedComponent();

		if (bIsBlueprintInstanceInherited)
		{
			FStringFormatNamedArguments Args;
			Args.Add(TEXT("InheritedText"), TEXT("(Inherited)"));
			return FText::FromString(FString::Format(TEXT("{InheritedText}"), Args));
		}
	}

	return FText::GetEmpty();
}

FText FGenericSubobjectData::GetDisplayName() const
{
	if (const AActor* DefaultActor = GetObject<AActor>())
	{
		FString Name;
		UBlueprint* Blueprint = UBlueprint::GetBlueprintFromClass(DefaultActor->GetClass());
		if (Blueprint != nullptr)
		{
			Blueprint->GetName(Name);
		}
		else
		{
			Name = DefaultActor->GetActorLabel();
		}
		return FText::FromString(Name);
	}
	else if (const UObject* Context = GetObject())
	{
		FString Name;
		Context->GetName(Name);

		return FText::FromString(Name);
	}

	return LOCTEXT("GetDisplayNameNotOverridden", "Unknown Subobject");
}

FName FindVariableNameGivenComponentInstance(const UGenericGraphNodeComponent* ComponentInstance)
{
	check(ComponentInstance != nullptr);

	// When names mismatch, try finding a differently named variable pointing to the the component (the mismatch should only be possible for native components)
	auto FindPropertyReferencingComponent = [](const UGenericGraphNodeComponent* Component) -> FProperty*
	{
		if (UGenericGraphNode* OwnerActor = Component->GetOwner())
		{
			UClass* OwnerClass = OwnerActor->GetClass();
			UGenericGraphNode* OwnerCDO = CastChecked<UGenericGraphNode>(OwnerClass->GetDefaultObject());
			check(OwnerCDO->HasAnyFlags(RF_ClassDefaultObject));

			for (TFieldIterator<FObjectProperty> PropIt(OwnerClass, EFieldIteratorFlags::IncludeSuper); PropIt; ++PropIt)
			{
				FObjectProperty* TestProperty = *PropIt;
				if (Component->GetClass()->IsChildOf(TestProperty->PropertyClass))
				{
					void* TestPropertyInstanceAddress = TestProperty->ContainerPtrToValuePtr<void>(OwnerCDO);
					UObject* ObjectPointedToByProperty = TestProperty->GetObjectPropertyValue(TestPropertyInstanceAddress);
					if (ObjectPointedToByProperty == Component)
					{
						// This property points to the component archetype, so it's an anchor even if it was named wrong
						return TestProperty;
					}
				}
			}

			for (TFieldIterator<FArrayProperty> PropIt(OwnerClass, EFieldIteratorFlags::IncludeSuper); PropIt; ++PropIt)
			{
				FArrayProperty* TestProperty = *PropIt;
				void* ArrayPropInstAddress = TestProperty->ContainerPtrToValuePtr<void>(OwnerCDO);

				FObjectProperty* ArrayEntryProp = CastField<FObjectProperty>(TestProperty->Inner);
				if ((ArrayEntryProp == nullptr) || !ArrayEntryProp->PropertyClass->IsChildOf<UActorComponent>())
				{
					continue;
				}

				FScriptArrayHelper ArrayHelper(TestProperty, ArrayPropInstAddress);
				for (int32 ComponentIndex = 0; ComponentIndex < ArrayHelper.Num(); ++ComponentIndex)
				{
					UObject* ArrayElement = ArrayEntryProp->GetObjectPropertyValue(ArrayHelper.GetRawPtr(ComponentIndex));
					if (ArrayElement == Component)
					{
						return TestProperty;
					}
				}
			}
		}

		return nullptr;
	};

	// First see if the name just works
	if (UGenericGraphNode* OwnerActor = ComponentInstance->GetOwner())
	{
		UClass* OwnerActorClass = OwnerActor->GetClass();
		if (FObjectProperty* TestProperty = FindFProperty<FObjectProperty>(OwnerActorClass, ComponentInstance->GetFName()))
		{
			if (ComponentInstance->GetClass()->IsChildOf(TestProperty->PropertyClass))
			{
				return TestProperty->GetFName();
			}
		}

		if (FProperty* ReferencingProp = FindPropertyReferencingComponent(ComponentInstance))
		{
			return ReferencingProp->GetFName();
		}
	}

	if (UGenericGraphNodeComponent* Archetype = Cast<UGenericGraphNodeComponent>(ComponentInstance->GetArchetype()))
	{
		if (FProperty* ReferencingProp = FindPropertyReferencingComponent(Archetype))
		{
			return ReferencingProp->GetFName();
		}
	}

	return NAME_None;
}

FName FGenericSubobjectData::GetVariableName() const
{
	FName VariableName = NAME_None;

	const UGenericGraphNodeComponent* ComponentTemplate = GetComponentTemplate();

	if (ComponentTemplate != nullptr)
	{
		// Try to find the component anchor variable name (first looks for an exact match then scans for any matching variable that points to the archetype in the CDO)
		VariableName = FindVariableNameGivenComponentInstance(ComponentTemplate);
	}

	return VariableName;
}

FText FGenericSubobjectData::GetSocketName() const
{
	return FText::GetEmpty();
}

FName FGenericSubobjectData::GetSocketFName() const
{
	return NAME_None;
}

bool FGenericSubobjectData::HasValidSocket() const
{
	return false;
}

void FGenericSubobjectData::SetSocketName(FName InNewName)
{
}

void FGenericSubobjectData::SetupAttachment(FName SocketName, const FGenericSubobjectDataHandle& AttachParentHandle)
{
}

FGenericSubobjectDataHandle FGenericSubobjectData::GetRootSubobject() const
{
	return FGenericSubobjectDataHandle();
}

bool FGenericSubobjectData::HasChild(const FGenericSubobjectDataHandle& ChildHandle) const
{
	return false;
}

FGenericSubobjectDataHandle FGenericSubobjectData::FindChild(const FGenericSubobjectDataHandle& ChildHandle) const
{
	return FGenericSubobjectDataHandle();
}

FGenericSubobjectDataHandle FGenericSubobjectData::FindChildByObject(UObject* ContextObject) const
{
	return FGenericSubobjectDataHandle();
}

FText FGenericSubobjectData::GetAssetName() const
{
	return FText();
}

FText FGenericSubobjectData::GetAssetPath() const
{
	return FText();
}

bool FGenericSubobjectData::IsAssetVisible() const
{
	return false;
}

FText FGenericSubobjectData::GetMobilityToolTipText() const
{
	return FText();
}

FText FGenericSubobjectData::GetComponentEditorOnlyTooltipText() const
{
	return FText();
}

FText FGenericSubobjectData::GetIntroducedInToolTipText() const
{
	return FText();
}

FText FGenericSubobjectData::GetActorDisplayText() const
{
	return FText();
}

bool FGenericSubobjectData::AddChildHandleOnly(const FGenericSubobjectDataHandle& InHandle)
{
	return false;
}

bool FGenericSubobjectData::RemoveChildHandleOnly(const FGenericSubobjectDataHandle& InHandle)
{
	return false;
}
