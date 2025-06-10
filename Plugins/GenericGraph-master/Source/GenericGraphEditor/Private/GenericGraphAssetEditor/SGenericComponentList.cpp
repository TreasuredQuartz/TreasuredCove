// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "GenericGraphAssetEditor/SGenericComponentList.h"
#include "GenericGraphAssetEditor/SGenericNodeComponentClassCombo.h"

// Generic Node Component Include list

#include "GenericGraphEditorPCH.h"
#include "GenericSubobjectEditor/GenericSubobjectData.h"
#include "GenericSubobjectEditor/GenericSubobjectDataHandle.h"
#include "GenericSubobjectEditor/GenericComponentAssetBroker.h"
#include "GenericSubobjectEditor/GenericSubobjectDataSubsystem.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Widgets/Input/SSearchBox.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "ClassIconFinder.h"
#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "TutorialMetaData.h"
#include "IDocumentation.h"
#include "Selection.h"

#define LOCTEXT_NAMESPACE "EdNode_GenericGraph"

static const FName ComponentList_ColumnName_ComponentClass("ComponentClass");
static const FName ComponentList_ColumnName_Asset("Asset");
static const FName ComponentList_ColumnName_Mobility("Mobility");
static const FName ComponentList_ContextMenuName("Kismet.GenericComponentListContextMenu");

/////////////////////////////////////////////////////
// Slate Widget SGenericComponentList

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SGenericComponentList::Construct(const FArguments& InArgs, UGenericGraphNode* InNode)
{
	GenericGraphNode = InNode;
	OnSelectionUpdated = InArgs._OnSelectionUpdated;
	HideComponentClassCombo = InArgs._HideComponentClassCombo;

	CreateCommandList();

	ConstructTreeWidget();

	ConstructHeaderBoxWidget();

	TSharedPtr<SWidget> Contents;
	
	Contents = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		.Padding(4.f, 0, 4.f, 4.f)
		[
			HeaderBox.ToSharedRef()
		]

		+ SVerticalBox::Slot()
		[
			SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("SCSEditor.Background"))
				.Padding(4.f)
				.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("ComponentsPanel")))
				.Visibility(this, &SGenericComponentList::GetComponentsTreeVisibility)
				[
					ListWidget.ToSharedRef()
				]
		];

	this->ChildSlot
		[
			Contents.ToSharedRef()
		];

	UpdateTree();
}

void SGenericComponentList::ConstructTreeWidget()
{
	TSharedPtr<SHeaderRow> HeaderRow = SNew(SHeaderRow)
		+ SHeaderRow::Column(FName("ComponentClass"))
		.DefaultLabel(LOCTEXT("Class", "Class"))
		.FillWidth(4);

	ListWidget = SNew(SListView<FGenericComponentListNodePtrType>)
		.ListItemsSource(&NodeList)
		.OnGenerateRow(this, &SGenericComponentList::GenerateListRow)
		.OnSelectionChanged(this, &SGenericComponentList::OnTreeSelectionChanged)
		.ToolTipText(LOCTEXT("DropAssetToAddComponent", "Drop asset here to add a component."))
		.SelectionMode(ESelectionMode::Multi)
		.ItemHeight(24)
		.HeaderRow
		(
			HeaderRow
		);

	HeaderRow->SetVisibility(EVisibility::Collapsed);
}

void SGenericComponentList::ConstructSearchBoxWidget()
{
	FilterBox = SNew(SSearchBox)
		.HintText(LOCTEXT("SearchHint", "Search"))
		.OnTextChanged(this, &SGenericComponentList::OnFilterTextChanged)
		.Visibility(this, &SGenericComponentList::GetComponentsFilterBoxVisibility);
}

void SGenericComponentList::ConstructHeaderBoxWidget()
{
	ConstructSearchBoxWidget();

	HeaderBox = SNew(SVerticalBox);

	TSharedPtr<SHorizontalBox> ButtonBox = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(0.0f, 0.0f, 4.0f, 0.0f)
		.AutoWidth()
		[
			SNew(SGenericNodeComponentClassCombo)
				.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("GenericGraphNode.AddComponent")))
				.Visibility(this, &SGenericComponentList::GetComponentClassComboButtonVisibility)
				.OnSubobjectClassSelected(this, &SGenericComponentList::PerformComboAddClass)
				.ToolTipText(LOCTEXT("AddComponent_Tooltip", "Adds a new component to this generic graph node"))
				.IsEnabled(true)
				// .CustomClassFilters(InArgs._SubobjectClassListFilters)
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(0.0f, 0.0f, 4.0f, 0.0f)
		.AutoWidth()
		[
			// Search Panel Context Extension
			// SAssignNew(ExtensionPanel, SExtensionPanel)
				// .ExtensionPanelID("SCSEditor.NextToAddComponentButton")
				// .ExtensionContext(ExtensionContext)

			SNew(SBox)
		]

		// horizontal slot index #2 => reserved for BP-editor search bar (see 'ButtonBox' and 'SearchBarHorizontalSlotIndex' usage below)

		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(0.0f, 0.0f, 4.0f, 0.0f)
		.AutoWidth()
		[
			// SNew(SPositiveActionButton)
				// .AddMetaData<FTagMetaData>(FTagMetaData(TEXT("Actor.ConvertToBlueprint")))
				// .Visibility(this, &SGenericSubobjectEditor::GetPromoteToBlueprintButtonVisibility)
				// .OnClicked(this, &SGenericSubobjectEditor::OnPromoteToBlueprintClicked)
				// .Icon(FAppStyle::Get().GetBrush("Icons.Blueprints"))
				/* .ToolTip(IDocumentation::Get()->CreateToolTip(
					TAttribute<FText>(LOCTEXT("PromoteToBluerprintTooltip", "Converts this actor into a reusable Blueprint Class that can have script behavior")),
					nullptr,
					TEXT("Shared/LevelEditor"),
					TEXT("ConvertToBlueprint"))) */

			SNew(SBox)
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			// SNew(SPositiveActionButton)
				// .AddMetaData<FTagMetaData>(FTagMetaData(TEXT("Actor.EditBlueprint")))
				// .Visibility(this, &SGenericSubobjectEditor::GetEditBlueprintButtonVisibility)
				// .ToolTipText(LOCTEXT("EditActorBlueprint_Tooltip", "Edit the Blueprint for this Actor"))
				// .Icon(FAppStyle::Get().GetBrush("Icons.Blueprints"))
				// .MenuContent() [ EditBlueprintMenuBuilder.MakeWidget() ]

			SNew(SBox)
		];

	ButtonBox->AddSlot()
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.Padding(3.0f, 3.0f)
		[
			FilterBox.ToSharedRef()
		];

	HeaderBox->AddSlot()
		.VAlign(VAlign_Center)
		.AutoHeight()
		[
			ButtonBox.ToSharedRef()
		];
}

void SGenericComponentList::UpdateTree(bool bRegenerateNodes)
{
	check(ListWidget.IsValid());

	if (bRegenerateNodes)
	{

	}
	// Obtain the list of selected items
	TArray<FGenericComponentListNodePtrType> SelectedTreeNodes = GetSelectedNodes();
	
	// Clear the current tree
	if (SelectedTreeNodes.Num() != 0)
	{
		ListWidget->ClearSelection();
	}
	NodeList.Empty();

	if (GenericGraphNode)
	{
		UGenericSubobjectDataSubsystem* DataSubsystem = UGenericSubobjectDataSubsystem::Get();

		TArray<FGenericSubobjectDataHandle> SubobjectData;
		DataSubsystem->GatherGenericSubobjectData(GenericGraphNode, SubobjectData);

		FGenericComponentListNodePtrType SeperatorNode;
		TMap<FGenericSubobjectDataHandle, FGenericComponentListNodePtrType> AddedNodes;

		// Create slate nodes for each subobject
		for (FGenericSubobjectDataHandle& Handle : SubobjectData)
		{
			// Do we have a slate node for this handle already? If not, then we need to make one
			FGenericComponentListNodePtrType NewNode = SGenericComponentList::FindOrCreateSlateNodeForHandle(Handle, AddedNodes);

			NodeList.Add(NewNode);

			/* There are no parents or children nodes.
			// const FSubobjectDataHandle& ParentHandle = Data->GetParentHandle();
			
			// Have parent? 
			if (ParentHandle.IsValid())
			{
				// Get the parent node for this subobject
				FSubobjectEditorTreeNodePtrType ParentNode = SSubobjectEditor::FindOrCreateSlateNodeForHandle(ParentHandle, AddedNodes);

				check(ParentNode);
				ParentNode->AddChild(NewNode);
				TreeWidget->SetItemExpansion(ParentNode, true);

				const bool bFilteredOut = RefreshFilteredState(NewNode, false);

				// Add a separator after the default scene root, but only it is not filtered out and if there are more items below it
				if (!bFilteredOut &&
					Data->IsDefaultSceneRoot() &&
					Data->IsInheritedComponent() &&
					SubobjectData.Find(Handle) < SubobjectData.Num() - 1)
				{
					SeperatorNode = MakeShareable<FSubobjectEditorTreeNode>(
						new FSubobjectEditorTreeNode(FSubobjectDataHandle::InvalidHandle, /** bIsSeperator true));
					AddedNodes.Add(SeperatorNode->GetDataHandle(), SeperatorNode);
					ParentNode->AddChild(SeperatorNode);
				}

			} */
			
			// Only for tree views
			// TreeWidget->SetItemExpansion(NewNode, true);
		}

		RestoreSelectionState(SelectedTreeNodes);

		// If we have a pending deferred rename request, redirect it to the new tree node
		if (DeferredRenameRequest.IsValid())
		{
			FGenericComponentListNodePtrType NodeToRenamePtr = FindSlateNodeForHandle(DeferredRenameRequest);
			if (NodeToRenamePtr.IsValid())
			{
				ListWidget->RequestScrollIntoView(NodeToRenamePtr);
			}
		}
	}

	/*
	TArray<UGenericGraphNodeComponent*> Components = GenericGraphNode->GetComponents();

	if (Components.Num() > 0)
	{
		for (UGenericGraphNodeComponent* Component : Components)
		{
			FGenericComponentListNodePtrType Node = MakeShareable<FGenericComponentListNode>(
				new FGenericComponentListNode(Component));

			NodeList.Add(Node);

			RefreshFilteredState(Node);
		}
	}*/

	ListWidget->RequestListRefresh();
}

void SGenericComponentList::DumpTree()
{
}

void SGenericComponentList::RefreshSelectionDetails()
{

}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FGenericSubobjectDataHandle SGenericComponentList::AddNewSubobject(UClass* NewClass, UObject* Asset, FText& FailReason)
{
	FGenericSubobjectDataHandle NewComponentHandle = FGenericSubobjectDataHandle::InvalidHandle;
	UGenericSubobjectDataSubsystem* Subsystem = UGenericSubobjectDataSubsystem::Get();

	// Ensure that the new class is actually a valid subobject type.
	// As of right now, that means the class has to be a child of an Generic Graph Node Component. 
	if (!NewClass->IsChildOf(UGenericGraphNodeComponent::StaticClass()) || !Subsystem)
	{
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("ClassName"), NewClass->GetDisplayNameText());
		FailReason = FText::Format(LOCTEXT("AddComponentFailed_InvalidClassType", "Cannot add a subobject of class '{ClassName}'"), Arguments);

		return NewComponentHandle; // Invalid
	}

	FName TemplateVariableName;
	UGenericGraphNodeComponent* ComponentTemplate = Cast<UGenericGraphNodeComponent>(Asset);
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

	if (ComponentTemplate)
	{
		Asset = nullptr;

		// Duplicate Component //

		UGenericGraphNodeComponent* NewCloneComponent = nullptr;
		UGenericGraphNode* Node = ComponentTemplate->GetOwner();

		Node->Modify();

		FName NewComponentName = *GenerateName(ComponentTemplate, Node);

		const bool bTemplateTransactional = ComponentTemplate->HasAllFlags(RF_Transactional);
		ComponentTemplate->SetFlags(RF_Transactional);

		NewCloneComponent = DuplicateObject<UGenericGraphNodeComponent>(ComponentTemplate, Node, NewComponentName);
		NewCloneComponent->ClearFlags(RF_DefaultSubObject);

		if (!bTemplateTransactional)
		{
			ComponentTemplate->ClearFlags(RF_Transactional);
		}

		NewCloneComponent->OnComponentCreated();

		// Add to SerializedComponents array so it gets saved
		Node->AddInstanceComponent(NewCloneComponent);

		// Register the new component
		NewCloneComponent->RegisterComponent();

		// Rerun construction scripts
		// Node->RerunConstructionScripts();

		NewComponentHandle = Subsystem->CreateSubobjectData(NewCloneComponent);
	}
	else if (GenericGraphNode)
	{
		GenericGraphNode->Modify();

		// Create an appropriate name for the new component
		FName NewComponentName = NAME_None;
		if (Asset)
		{
			NewComponentName = *GenerateName(Asset, GenericGraphNode);
		}
		else
		{
			NewComponentName = *GenerateName(NewClass, GenericGraphNode);
		}

		// Get the set of owned components that exists prior to instancing the new component.
		TInlineComponentArray<UGenericGraphNodeComponent*> PreInstanceComponents;
		GenericGraphNode->GetComponents(PreInstanceComponents);

		// Construct the new component and attach as needed
		UGenericGraphNodeComponent* NewInstanceComponent = NewObject<UGenericGraphNodeComponent>(GenericGraphNode, NewClass, NewComponentName, RF_Transactional);

		NewInstanceComponent->OnComponentCreated();

		// Add to SerializedComponents array so it gets saved
		GenericGraphNode->AddInstanceComponent(NewInstanceComponent);

		NewInstanceComponent->RegisterComponent();

		NewComponentHandle = Subsystem->CreateSubobjectData(NewInstanceComponent);
	}

	return NewComponentHandle;
}

FGenericSubobjectDataHandle SGenericComponentList::PerformComboAddClass(TSubclassOf<UGenericGraphNodeComponent> ComponentClass, EComponentCreateAction::Type ComponentCreateAction, UObject* AssetOverride)
{
	FGenericSubobjectDataHandle NewComponentHandle = FGenericSubobjectDataHandle::InvalidHandle;
	UClass* NewClass = ComponentClass;

	// if ()
	{

	}
	// else if ()
	{

	}

	if (NewClass)
	{
		TUniquePtr<FScopedTransaction> AddTransaction = MakeUnique<FScopedTransaction>(LOCTEXT("AddComponent", "Add Component"));

		FEditorDelegates::LoadSelectedAssetsIfNeeded.Broadcast();
		USelection* Selection = GEditor->GetSelectedObjects();

		bool bAddedComponent = false;
		FText OutFailReason;

		// This adds components according to the type selected in the drop down. If the user
		// has the appropriate objects selected in the content browser then those are added,
		// else we go down the previous route of adding components by type.

		// Furthermore don't try to match up assets for USceneComponent it will match lots of things and doesn't have any nice behavior for asset adds 
		if (Selection->Num() > 0 && !AssetOverride)
		{
			for (FSelectionIterator ObjectIter(*Selection); ObjectIter; ++ObjectIter)
			{
				UObject* Object = *ObjectIter;
				UClass* Class = Object->GetClass();

				TArray<TSubclassOf<UGenericGraphNodeComponent>> ComponentClasses = FGenericComponentAssetBrokerage::GetComponentsForAsset(Object);

				// if the selected asset supports the selected component type then go ahead and add it
				for (int32 ComponentIndex = 0; ComponentIndex < ComponentClasses.Num(); ComponentIndex++)
				{
					if (ComponentClasses[ComponentIndex]->IsChildOf(NewClass))
					{
						NewComponentHandle = AddNewSubobject(NewClass, Object, OutFailReason/*, MoveTemp(AddTransaction)*/);
						bAddedComponent = true;
						break;
					}
				}
			}
		}

		if (!bAddedComponent)
		{
			if (GenericGraphNode)
			{
				NewComponentHandle = AddNewSubobject(NewClass, AssetOverride, OutFailReason/*, MoveTemp(AddTransaction)*/);
			}
		}

		// We failed adding a new component, display why with a slate notif
		if (!NewComponentHandle.IsValid())
		{
			if (OutFailReason.IsEmpty())
			{
				OutFailReason = LOCTEXT("AddComponentFailed_Generic", "Failed to add component!");
			}

			FNotificationInfo Info(OutFailReason);
			Info.Image = FAppStyle::GetBrush(TEXT("Icons.Error"));
			Info.bFireAndForget = true;
			Info.bUseSuccessFailIcons = false;
			Info.ExpireDuration = 5.0f;

			FSlateNotificationManager::Get().AddNotification(Info);
		}
		else
		{
			UpdateTree();

			// Set focus to the newly created subobject
			/* FGenericComponentListNodePtrType NewNode = FindSlateNodeForHandle(NewComponentHandle);
			if (NewNode != nullptr)
			{
				ListWidget->SetSelection(NewNode);
				OnRenameComponent(MoveTemp(AddTransaction));
			} // */
		}
	}

	return NewComponentHandle;
}

TSharedRef<ITableRow> SGenericComponentList::GenerateListRow(FGenericComponentListNodePtrType InNodePtr, const TSharedRef<STableViewBase>& OwnerTable)
{
	FGraphNodeMetaData TagMeta(TEXT("TableRow"));
	const UGenericGraphNodeComponent* Template = InNodePtr && !InNodePtr->IsSeperator()
		? Cast<UGenericGraphNodeComponent>(InNodePtr->GetDataSource()->GetObject())
		: nullptr;
	if (Template)
	{
		TagMeta.FriendlyName = FString::Printf(TEXT("TableRow,%s,0"), *Template->GetReadableName());
	}

	return SNew(SGenericComponent_RowWidget, SharedThis(this), InNodePtr, OwnerTable)
		.AddMetaData<FTutorialMetaData>(TagMeta);
}

void SGenericComponentList::OnTreeSelectionChanged(FGenericComponentListNodePtrType InNodePtr, ESelectInfo::Type SelectInfo)
{
	LOG_INFO(TEXT("Selected Component on Node!"));

	UpdateSelectionFromNodes(ListWidget->GetSelectedItems());
}

void SGenericComponentList::UpdateSelectionFromNodes(const TArray<FGenericComponentListNodePtrType>& SelectedNodes)
{
	bUpdatingSelection = true;

	TArray<FGenericSubobjectDataHandle> SelectedHandles;
	for (const FGenericComponentListNodePtrType SelectedNode : SelectedNodes)
	{
		if (SelectedNode) SelectedHandles.Add(SelectedNode->GetDataHandle());
	}

	UGenericSubobjectDataSubsystem::Get()->SelectSubobjects(SelectedHandles);

	bUpdatingSelection = false;
}

FGenericComponentListNodePtrType SGenericComponentList::FindOrCreateSlateNodeForHandle(const FGenericSubobjectDataHandle& InHandle, TMap<FGenericSubobjectDataHandle, FGenericComponentListNodePtrType>& ExistingNodes)
{
	// If we have already created a tree node for this handle, great! Return that.
	if (const FGenericComponentListNodePtrType* Found = ExistingNodes.Find(InHandle))
	{
		return *Found;
	}

	// Otherwise we haven't created this yet and we need to make a new one!
	FGenericComponentListNodePtrType NewNode = MakeShareable<FGenericComponentListNode>(new FGenericComponentListNode(InHandle));
	ExistingNodes.Add(InHandle, NewNode);

	return NewNode;
}

FGenericComponentListNodePtrType SGenericComponentList::FindSlateNodeForHandle(const FGenericSubobjectDataHandle& InHandleToFind, FGenericComponentListNodePtrType InStartNodePtr) const
{
	FGenericComponentListNodePtrType OutNodePtr;
	if (InHandleToFind.IsValid() && NodeList.Num() > 0)
	{
		FGenericSubobjectData* DataToFind = InHandleToFind.GetSharedDataPtr().Get();

		TSet<FGenericComponentListNodePtrType> VisitedNodes;
		for (FGenericComponentListNodePtrType CurNodePtr : NodeList)
		{
			if (CurNodePtr->GetDataHandle().IsValid())
			{
				if (CurNodePtr->GetDataHandle().GetSharedDataPtr()->GetObject() == DataToFind->GetObject())
				{
					OutNodePtr = CurNodePtr;
				}
			}
		}
	}

	return OutNodePtr;
}

FGenericComponentListNodePtrType SGenericComponentList::FindSlateNodeForVariableName(FName InVariableName) const
{
	FGenericComponentListNodePtrType OutNodePtr;
	if (NodeList.Num() > 0)
	{
		TSet<FGenericComponentListNodePtrType> VisitedNodes;
		for (FGenericComponentListNodePtrType CurNodePtr : NodeList)
		{
			if (CurNodePtr->GetDataHandle().IsValid())
			{
				if (CurNodePtr->GetDataHandle().GetSharedDataPtr()->GetVariableName() == InVariableName)
				{
					OutNodePtr = CurNodePtr;
				}
			}
		}
	}

	return OutNodePtr;
}

TArray<FGenericComponentListNodePtrType> SGenericComponentList::GetSelectedNodes() const
{
	TArray<FGenericComponentListNodePtrType> SelectedTreeNodes = ListWidget->GetSelectedItems();

	return SelectedTreeNodes;
}

TArray<FGenericSubobjectDataHandle> SGenericComponentList::GetSelectedHandles() const
{
	TArray<FGenericComponentListNodePtrType> SelectedNodes = GetSelectedNodes();
	TArray<FGenericSubobjectDataHandle> OutHandles;
	for (FGenericComponentListNodePtrType& Ptr : SelectedNodes)
	{
		OutHandles.Add(Ptr->GetDataHandle());
	}
	return OutHandles;
}

void SGenericComponentList::RestoreSelectionState(const TArray<FGenericComponentListNodePtrType>& SelectedListNodes, bool bFallBackToVariableName)
{
	if (SelectedListNodes.Num() > 0)
	{
		// If there is only one item selected, imitate user selection to preserve navigation
		ESelectInfo::Type SelectInfo = SelectedListNodes.Num() == 1 ? ESelectInfo::OnMouseClick : ESelectInfo::Direct;

		// Restore the previous selection state on the new tree nodes
		for (int i = 0; i < SelectedListNodes.Num(); ++i)
		{
			FGenericSubobjectDataHandle CurrentNodeDataHandle = SelectedListNodes[i]->GetDataHandle();
			FGenericComponentListNodePtrType NodeToSelectPtr = FindSlateNodeForHandle(CurrentNodeDataHandle);

			// If we didn't find something for this exact handle, fall back to just search for something
			// with the same variable name. This helps to still preserve selection across re-compiles of a class.
			if (!NodeToSelectPtr.IsValid() && CurrentNodeDataHandle.IsValid() && bFallBackToVariableName)
			{
				NodeToSelectPtr = FindSlateNodeForVariableName(CurrentNodeDataHandle.GetSharedDataPtr()->GetVariableName());
			}

			if (NodeToSelectPtr.IsValid())
			{
				ListWidget->SetItemSelection(NodeToSelectPtr, true, SelectInfo);
			}
		}
	}
}

void SGenericComponentList::SelectNode(FGenericComponentListNodePtrType InNodeToSelect, bool bIsCntrlDown)
{
	if (ListWidget.IsValid() && InNodeToSelect.IsValid())
	{
		if (!bIsCntrlDown)
		{
			ListWidget->SetSelection(InNodeToSelect);
		}
		else
		{
			ListWidget->SetItemSelection(InNodeToSelect, !ListWidget->IsItemSelected(InNodeToSelect));
		}
	}
}

void SGenericComponentList::ClearSelection()
{
	if (bUpdatingSelection == false)
	{
		check(ListWidget.IsValid());
		ListWidget->ClearSelection();
	}
}

void SGenericComponentList::CreateCommandList()
{
	CommandList = MakeShareable(new FUICommandList);

	CommandList->MapAction(FGenericCommands::Get().Cut,
		FUIAction(FExecuteAction::CreateSP(this, &SGenericComponentList::CutSelectedNodes),
			FCanExecuteAction::CreateSP(this, &SGenericComponentList::CanCutNodes))
	);

	CommandList->MapAction(FGenericCommands::Get().Copy,
		FUIAction(FExecuteAction::CreateSP(this, &SGenericComponentList::CopySelectedNodes),
			FCanExecuteAction::CreateSP(this, &SGenericComponentList::CanCopyNodes))
	);

	CommandList->MapAction(FGenericCommands::Get().Paste,
		FUIAction(FExecuteAction::CreateSP(this, &SGenericComponentList::PasteNodes),
			FCanExecuteAction::CreateSP(this, &SGenericComponentList::CanPasteNodes))
	);

	CommandList->MapAction(FGenericCommands::Get().Duplicate,
		FUIAction(FExecuteAction::CreateSP(this, &SGenericComponentList::OnDuplicateComponent),
			FCanExecuteAction::CreateSP(this, &SGenericComponentList::CanDuplicateComponent))
	);

	CommandList->MapAction(FGenericCommands::Get().Delete,
		FUIAction(FExecuteAction::CreateSP(this, &SGenericComponentList::OnDeleteNodes),
			FCanExecuteAction::CreateSP(this, &SGenericComponentList::CanDeleteNodes))
	);

	CommandList->MapAction(FGenericCommands::Get().Rename,
		FUIAction(FExecuteAction::CreateSP(this, &SGenericComponentList::OnRenameComponent),
			FCanExecuteAction::CreateSP(this, &SGenericComponentList::CanRenameComponent))
	);

	CommandList->MapAction(FGraphEditorCommands::Get().GetFindReferences(),
		FUIAction(FExecuteAction::CreateSP(this, &SGenericComponentList::OnFindReferences, false, EGetFindReferenceSearchStringFlags::Legacy))
	);

	CommandList->MapAction(FGraphEditorCommands::Get().FindReferencesByNameLocal,
		FUIAction(FExecuteAction::CreateSP(this, &SGenericComponentList::OnFindReferences, false, EGetFindReferenceSearchStringFlags::None))
	);

	CommandList->MapAction(FGraphEditorCommands::Get().FindReferencesByNameGlobal,
		FUIAction(FExecuteAction::CreateSP(this, &SGenericComponentList::OnFindReferences, true, EGetFindReferenceSearchStringFlags::None))
	);

	CommandList->MapAction(FGraphEditorCommands::Get().FindReferencesByClassMemberLocal,
		FUIAction(FExecuteAction::CreateSP(this, &SGenericComponentList::OnFindReferences, false, EGetFindReferenceSearchStringFlags::UseSearchSyntax))
	);

	CommandList->MapAction(FGraphEditorCommands::Get().FindReferencesByClassMemberGlobal,
		FUIAction(FExecuteAction::CreateSP(this, &SGenericComponentList::OnFindReferences, true, EGetFindReferenceSearchStringFlags::UseSearchSyntax))
	);
}

void SGenericComponentList::OnFindReferences(bool bSearchAllBlueprints, const EGetFindReferenceSearchStringFlags Flags)
{
	TArray<FGenericComponentListNodePtrType> SelectedNodes = ListWidget->GetSelectedItems();
	if (SelectedNodes.Num() == 1)
	{
		/* TSharedPtr<IToolkit> FoundAssetEditor = FToolkitManager::Get().FindEditorForAsset(GetBlueprint());
		FGenericSubobjectData* Data = SelectedNodes[0]->GetDataSource();
		if (FoundAssetEditor.IsValid() && Data)
		{
			const FString VariableName = Data->GetVariableName().ToString();

			FMemberReference MemberReference;
			MemberReference.SetSelfMember(*VariableName);
			const FString SearchTerm = EnumHasAnyFlags(Flags, EGetFindReferenceSearchStringFlags::UseSearchSyntax) ? MemberReference.GetReferenceSearchString(GetBlueprint()->SkeletonGeneratedClass) : FString::Printf(TEXT("\"%s\""), *VariableName);

			TSharedRef<IBlueprintEditor> BlueprintEditor = StaticCastSharedRef<IBlueprintEditor>(
				FoundAssetEditor.ToSharedRef());

			const bool bSetFindWithinBlueprint = !bSearchAllBlueprints;
			BlueprintEditor->SummonSearchUI(bSetFindWithinBlueprint, SearchTerm);
		} // */
	}
}

bool SGenericComponentList::IsEditingAllowed() const
{
	return AllowEditing.Get() && nullptr == GEditor->PlayWorld;
}

bool SGenericComponentList::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void SGenericComponentList::CutSelectedNodes()
{
	TArray<FGenericComponentListNodePtrType> SelectedNodes = GetSelectedNodes();
	const FScopedTransaction Transaction(SelectedNodes.Num() > 1
		? LOCTEXT("CutComponents", "Cut Components")
		: LOCTEXT("CutComponent", "Cut Component"));

	FGenericComponentListNodePtrType PostCutSelection = NodeList.Num() > 0 ? NodeList[0] : nullptr;
	/* for (FGenericComponentListNodePtrType& SelectedNode : SelectedNodes)
	{
		if (FGenericComponentListNodePtrType ParentNode = SelectedNode->GetParent())
		{
			if (!SelectedNodes.Contains(ParentNode))
			{
				PostCutSelection = ParentNode;
				break;
			}
		}
	} // */

	CopySelectedNodes();
	OnDeleteNodes();

	// Select the parent node of the thing that was just cut
	if (PostCutSelection.IsValid())
	{
		ListWidget->SetSelection(PostCutSelection);
	}
}

bool SGenericComponentList::CanCopyNodes() const
{
	TArray<FGenericSubobjectDataHandle> SelectedHandles = GetSelectedHandles();
	if (UGenericSubobjectDataSubsystem* System = UGenericSubobjectDataSubsystem::Get())
	{
		return System->CanCopySubobjects(SelectedHandles);
	}
	return false;
}

void SGenericComponentList::CopySelectedNodes()
{
	TArray<FGenericSubobjectDataHandle> SelectedHandles = GetSelectedHandles();
	if (UGenericSubobjectDataSubsystem* System = UGenericSubobjectDataSubsystem::Get())
	{
		return System->CopySubobjects(SelectedHandles);
	}
}

bool SGenericComponentList::CanPasteNodes() const
{
	if (!IsEditingAllowed())
	{
		return false;
	}

	FGenericSubobjectDataHandle SceneRoot = FGenericSubobjectDataHandle::InvalidHandle;

	if (UGenericSubobjectDataSubsystem* System = UGenericSubobjectDataSubsystem::Get())
	{
		return System->CanPasteSubobjects(SceneRoot);
	}

	return false;
}

void SGenericComponentList::PasteNodes()
{

}

bool SGenericComponentList::CanDeleteNodes() const
{
	if (!IsEditingAllowed())
	{
		return false;
	}

	TArray<FGenericComponentListNodePtrType> SelectedNodes = GetSelectedNodes();
	for (const FGenericComponentListNodePtrType& SelectedNode : SelectedNodes)
	{
		if (!SelectedNode->IsValid() || !SelectedNode->CanDelete())
		{
			return false;
		}
	}

	return true;
}

void SGenericComponentList::OnDeleteNodes()
{
	// Gather the handles of the components that we want to delete
	TArray<FGenericSubobjectDataHandle> HandlesToDelete;
	TArray<FGenericComponentListNodePtrType> SelectedNodes = GetSelectedNodes();
	for (const FGenericComponentListNodePtrType& Node : SelectedNodes)
	{
		check(Node->IsValid());

		if (const FGenericSubobjectData* Data = Node->GetDataSource())
		{
			HandlesToDelete.Add(Data->GetHandle());
		}
	}

	if (UGenericSubobjectDataSubsystem* System = UGenericSubobjectDataSubsystem::Get())
	{
		FGenericSubobjectDataHandle HandleToSelect;
		FGenericSubobjectDataHandle ContextHandle = System->CreateSubobjectData(GenericGraphNode);

		int32 NumDeleted = System->DeleteSubobjects(ContextHandle, HandlesToDelete, HandleToSelect);

		if (NumDeleted > 0)
		{
			if (HandleToSelect.IsValid())
			{
				FGenericComponentListNodePtrType NodeToSelect = FindSlateNodeForHandle(HandleToSelect);
				if (NodeToSelect.IsValid())
				{
					ListWidget->SetSelection(NodeToSelect);
				}
			}

			UpdateTree();

			// Do this AFTER marking the Blueprint as modified
			UpdateSelectionFromNodes(ListWidget->GetSelectedItems());
		}
	}
}

bool SGenericComponentList::CanDuplicateComponent() const
{
	if (!IsEditingAllowed())
	{
		return false;
	}

	return CanCopyNodes();
}

void SGenericComponentList::OnDuplicateComponent()
{

}

bool SGenericComponentList::CanRenameComponent() const
{
	if (!IsEditingAllowed())
	{
		return false;
	}
	// In addition to certain node types, don't allow nodes within a child actor template's hierarchy to be renamed
	TArray<FGenericComponentListNodePtrType> SelectedItems = ListWidget->GetSelectedItems();

	return SelectedItems.Num() == 1 && SelectedItems[0]->CanRename();
}

void SGenericComponentList::OnRenameComponent()
{
	OnRenameComponent(nullptr);
}

void SGenericComponentList::OnRenameComponent(TUniquePtr<FScopedTransaction> InComponentCreateTransaction)
{
	TArray<FGenericComponentListNodePtrType> SelectedNodes = GetSelectedNodes();

	// Should already be prevented from making it here.
	check(SelectedNodes.Num() == 1);

	DeferredRenameRequest = SelectedNodes[0]->GetDataHandle();

	// If this fails, something in the chain of responsibility failed to end the previous transaction.
	check(!DeferredOngoingCreateTransaction.IsValid());
	// If a 'create + give initial name' transaction is ongoing, take responsibility of ending it until the selected item is scrolled into view.
	DeferredOngoingCreateTransaction = MoveTemp(InComponentCreateTransaction);

	ListWidget->RequestScrollIntoView(SelectedNodes[0]);

	if (DeferredOngoingCreateTransaction.IsValid() && !PostTickHandle.IsValid())
	{
		// Ensure the item will be scrolled into view during the frame (See explanation in OnPostTick()).
		PostTickHandle = FSlateApplication::Get().OnPostTick().AddSP(this, &SGenericComponentList::OnPostTick);
	}
}

void SGenericComponentList::OnPostTick(float)
{
	// If a 'create + give initial name' is ongoing and the transaction ownership was not transferred during the frame it was requested, it is most likely because the newly
	// created item could not be scrolled into view (should say 'teleported', the scrolling is not animated). The tree view will not put the item in view if the there is
	// no space left to display the item. (ex a splitter where all the display space is used by the other component). End the transaction before starting a new frame. The user
	// will not be able to rename on creation, the widget is likely not in view and cannot be edited anyway.
	DeferredOngoingCreateTransaction.Reset();

	// The post tick event handler is not required anymore.
	FSlateApplication::Get().OnPostTick().Remove(PostTickHandle);
	PostTickHandle.Reset();
}

void SGenericComponentList::OnFilterTextChanged(const FText& InFilterText)
{
	FGenericComponentListNodePtrType NewSelection;
	// iterate backwards so we select from the top down
	for (int32 ComponentIndex = NodeList.Num() - 1; ComponentIndex >= 0; --ComponentIndex)
	{
		FGenericComponentListNodePtrType Node = NodeList[ComponentIndex];

		/*bool bIsRootVisible = !RefreshFilteredState(Node, true);
		ListWidget->SetItemExpansion(Node, bIsRootVisible);
		if (bIsRootVisible)
		{
			if (!GetFilterText().IsEmpty())
			{
				NewSelection = OnFilterTextChanged_Inner::ExpandToFilteredChildren(this, Node);
			}
		}*/
	}

	if (!NewSelection.IsValid() && NodeList.Num() > 0)
	{
		NewSelection = NodeList[0];
	}

	if (NewSelection.IsValid() && !ListWidget->IsItemSelected(NewSelection))
	{
		SelectNode(NewSelection, /*IsCntrlDown =*/false);
	}

	UpdateTree(/*bRegenerateTreeNodes =*/false);
}

TSubclassOf<UObject> SGenericComponentList::GetComponentTypeFilterToApply() const
{
	TSubclassOf<UObject> ComponentType = UICustomization.IsValid()
										? UICustomization->GetTypeFilter()
										: nullptr;
	if (!ComponentType)
	{
		// ComponentType = ComponentTypeFilter.Get();
	}
	return ComponentType;
}

bool SGenericComponentList::RefreshFilteredState(FGenericComponentListNodePtrType ListNode)
{
	const UClass* FilterType = GetComponentTypeFilterToApply();

	FString FilterText = FText::TrimPrecedingAndTrailing(GetFilterText()).ToString();
	TArray<FString> FilterTerms;
	FilterText.ParseIntoArray(FilterTerms, TEXT(" "), /*CullEmpty =*/true);

	ListNode->RefreshFilteredState(FilterType, FilterTerms);
	return ListNode->IsFlaggedForFiltration();
}

FText SGenericComponentList::GetFilterText() const
{
	return FilterBox->GetText();
}

EVisibility SGenericComponentList::GetComponentClassComboButtonVisibility() const
{
	return (HideComponentClassCombo.Get() || (UICustomization.IsValid() && UICustomization->HideAddComponentButton()))
		? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility SGenericComponentList::GetComponentsFilterBoxVisibility() const
{
	return (UICustomization.IsValid() && UICustomization->HideComponentsFilterBox())
		? EVisibility::Collapsed
		: EVisibility::Visible;
}

EVisibility SGenericComponentList::GetComponentsTreeVisibility() const
{
	return (UICustomization.IsValid() && UICustomization->HideComponentsTree())
		? EVisibility::Collapsed
		: EVisibility::Visible;
}

/////////////////////////////////////////////////////
// Slate Widget SGenericComponent_RowWidget

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SGenericComponent_RowWidget::Construct(const FArguments& InArgs, TWeakPtr<SGenericComponentList> InEditor, FGenericComponentListNodePtrType InNodePtr, TSharedPtr<STableViewBase> InOwnerTableView)
{
	check(InNodePtr.IsValid());
	ComponentEditor = InEditor;
	ComponentPtr = InNodePtr;

	bool bIsSeparator = InNodePtr->IsSeperator();

	FSuperRowType::FArguments Args = FSuperRowType::FArguments()
		.Style(bIsSeparator ?
			&FAppStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.NoHoverTableRow") :
			&FAppStyle::Get().GetWidgetStyle<FTableRowStyle>("SceneOutliner.TableViewRow")) //@todo create editor style for the SCS tree
		.Padding(FMargin(0.f, 4.f, 0.f, 4.f))
		.ShowSelection(!bIsSeparator)
		.OnDragDetected(this, &SGenericComponent_RowWidget::HandleOnDragDetected)
		.OnDragEnter(this, &SGenericComponent_RowWidget::HandleOnDragEnter)
		.OnDragLeave(this, &SGenericComponent_RowWidget::HandleOnDragLeave)
		.OnCanAcceptDrop(this, &SGenericComponent_RowWidget::HandleOnCanAcceptDrop)
		.OnAcceptDrop(this, &SGenericComponent_RowWidget::HandleOnAcceptDrop);

	SMultiColumnTableRow<FGenericComponentListNodePtrType>::Construct(Args, InOwnerTableView.ToSharedRef());
}

TSharedRef<SWidget> SGenericComponent_RowWidget::GenerateWidgetForColumn(const FName& ColumnName)
{
	FGenericComponentListNodePtrType Node = GetComponentPtr();
	check(Node);

	if (Node->IsSeperator())
	{
		return SNew(SBox)
			.Padding(1.f)
			[
				SNew(SSeparator)
					.SeparatorImage(FAppStyle::Get().GetBrush("Menu.Separator"))
					.Thickness(1.f)
			];
	}

	// FGenericSubobjectData* Data = Node->GetDataSource();

	if (ColumnName == ComponentList_ColumnName_ComponentClass)
	{
		InlineWidget =
			SNew(SInlineEditableTextBlock)
			.Text(this, &SGenericComponent_RowWidget::GetNameLabel)
			.OnVerifyTextChanged(this, &SGenericComponent_RowWidget::OnNameTextVerifyChanged)
			.OnTextCommitted(this, &SGenericComponent_RowWidget::OnNameTextCommit)
			.IsSelected(this, &SGenericComponent_RowWidget::IsSelectedExclusively)
			.IsReadOnly(this, &SGenericComponent_RowWidget::IsReadOnly);

		Node->SetRenameRequestedDelegate(FGenericComponentListNode::FOnRenameRequested::CreateSP(InlineWidget.Get(), &SInlineEditableTextBlock::EnterEditingMode));

		const bool IsRootActorNode = false; // Data->IsRootActor();

		TSharedRef<SHorizontalBox> Contents =
			SNew(SHorizontalBox)
			.ToolTip(CreateToolTipWidget())
			+ SHorizontalBox::Slot()
			.Padding(FMargin(IsRootActorNode ? 0.f : 4.f, 0.f, 0.f, 0.f))
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SExpanderArrow, SharedThis(this))
					.Visibility(IsRootActorNode ? EVisibility::Collapsed : EVisibility::Visible)
			]
			+ SHorizontalBox::Slot()
			.Padding(FMargin(IsRootActorNode ? 4.f : 0.f, 0.f, 0.f, 0.f))
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
					.Image(GetIconBrush())
					.ColorAndOpacity(this, &SGenericComponent_RowWidget::GetColorTintForIcon)
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.AutoWidth()
			.Padding(6.f, 0.f, 0.f, 0.f)
			[
				InlineWidget.ToSharedRef()
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.FillWidth(1.0f)
			.Padding(4.f, 0.f, 0.f, 0.f)
			[
				SNew(STextBlock)
					.Text(this, &SGenericComponent_RowWidget::GetObjectContextText)
					.ColorAndOpacity(FSlateColor::UseForeground())
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			.AutoWidth()
			.Padding(4.f, 0.f, 4.f, 0.f)
			[
				// GetInheritedLinkWidget()
				SNew(SBox)
			];

		return Contents;
	}
	else if (ColumnName == ComponentList_ColumnName_Asset)
	{
		return
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
					.Visibility(this, &SGenericComponent_RowWidget::GetAssetVisibility)
					.Text(this, &SGenericComponent_RowWidget::GetAssetName)
					.ToolTipText(this, &SGenericComponent_RowWidget::GetAssetPath)
			];
	}
	else if (ColumnName == ComponentList_ColumnName_Mobility)
	{
		return SNew(SSpacer);
	}

	return SNew(STextBlock)
		.Text(LOCTEXT("UnknownColumn", "Unknown Column")); // */
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

/////////////////////////// Begin Drag And Drop ///////////////////////////

void SGenericComponent_RowWidget::HandleOnDragEnter(const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();
	if (!Operation.IsValid())
	{
		return;
	}
	
	/* TSharedPtr<FSubobjectRowDragDropOp> DragRowOp = DragDropEvent.GetOperationAs<FSubobjectRowDragDropOp>();

	if (DragRowOp.IsValid())
	{
		check(SubobjectEditor.IsValid());

		FText Message;
		FSlateColor IconColor = FLinearColor::White;

		for (const FGenericSubobjectEditorTreeNodePtrType& SelectedNodePtr : DragRowOp->SourceNodes)
		{
			const FGenericSubobjectData* Data = SelectedNodePtr->GetDataSource();
			if (!Data->CanReparent())
			{
				// We set the tooltip text here because it won't change across entry/leave events
				if (DragRowOp->SourceNodes.Num() == 1)
				{
					if (SelectedNodePtr->IsChildSubtreeNode())
					{
						Message = LOCTEXT("DropActionToolTip_Error_CannotReparent_ChildActorSubTreeNodes", "The selected component is part of a child actor template and cannot be reordered here.");
					}
					else if (!Data->IsSceneComponent())
					{
						Message = LOCTEXT("DropActionToolTip_Error_CannotReparent_NotSceneComponent", "The selected component is not a scene component and cannot be attached to other components.");
					}
					else if (Data->IsInheritedComponent())
					{
						Message = LOCTEXT("DropActionToolTip_Error_CannotReparent_Inherited", "The selected component is inherited and cannot be reordered here.");
					}
					else
					{
						Message = LOCTEXT("DropActionToolTip_Error_CannotReparent", "The selected component cannot be moved.");
					}
				}
				else
				{
					Message = LOCTEXT("DropActionToolTip_Error_CannotReparentMultiple", "One or more of the selected components cannot be attached.");
				}
				break;
			}
		}

		if (Message.IsEmpty())
		{
			// FGenericSubobjectEditorTreeNodePtrType SceneRootNodePtr = SubobjectEditor.Pin()->GetSceneRootNode();

			FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
			const FGenericSubobjectData* NodeData = NodePtr->GetDataSource();

			if (!NodePtr->IsComponentNode())
			{
				// Don't show a feedback message if over a node that makes no sense, such as a separator or the instance node
				Message = LOCTEXT("DropActionToolTip_FriendlyError_DragToAComponent", "Drag to another component in order to attach to that component or become the root component.\nDrag to a Blueprint graph in order to drop a reference.");
			}
			else if (NodePtr->IsChildSubtreeNode())
			{
				// Can't drag onto components within a child actor node's subtree
				Message = LOCTEXT("DropActionToolTip_Error_ChildActorSubTree", "Cannot attach to this component as it is part of a child actor template.");
			}

			// Validate each selected node being dragged against the node that belongs to this row. Exit the loop if we have a valid tooltip OR a valid pending drop action once all nodes in the selection have been validated.
			for (auto SourceNodeIter = DragRowOp->SourceNodes.CreateConstIterator(); SourceNodeIter && (Message.IsEmpty() || DragRowOp->PendingDropAction != FSubobjectRowDragDropOp::DropAction_None); ++SourceNodeIter)
			{
				FGenericSubobjectEditorTreeNodePtrType DraggedNodePtr = *SourceNodeIter;
				const FGenericSubobjectData* const DraggedNodeData = DraggedNodePtr->GetDataSource();
				check(DraggedNodePtr.IsValid());

				// Reset the pending drop action each time through the loop
				DragRowOp->PendingDropAction = FSubobjectRowDragDropOp::DropAction_None;

				// Get the component template objects associated with each node
				const USceneComponent* HoveredTemplate = Cast<USceneComponent>(NodePtr->GetComponentTemplate());
				const USceneComponent* DraggedTemplate = Cast<USceneComponent>(DraggedNodeData->GetComponentTemplate());

				if (DraggedNodePtr == NodePtr)
				{
					// Attempted to drag and drop onto self
					if (DragRowOp->SourceNodes.Num() > 1)
					{
						Message = FText::Format(LOCTEXT("DropActionToolTip_Error_CannotAttachToSelfWithMultipleSelection", "Cannot attach the selected components here because it would result in {0} being attached to itself. Remove it from the selection and try again."), DraggedNodeData->GetDragDropDisplayText());
					}
					else
					{
						Message = FText::Format(LOCTEXT("DropActionToolTip_Error_CannotAttachToSelf", "Cannot attach {0} to itself."), DraggedNodeData->GetDragDropDisplayText());
					}
				}
				else if (NodePtr->IsAttachedTo(DraggedNodePtr))
				{
					// Attempted to drop a parent onto a child
					if (DragRowOp->SourceNodes.Num() > 1)
					{
						Message = FText::Format(LOCTEXT("DropActionToolTip_Error_CannotAttachToChildWithMultipleSelection", "Cannot attach the selected components here because it would result in {0} being attached to one of its children. Remove it from the selection and try again."), DraggedNodeData->GetDragDropDisplayText());
					}
					else
					{
						Message = FText::Format(LOCTEXT("DropActionToolTip_Error_CannotAttachToChild", "Cannot attach {0} to one of its children."), DraggedNodeData->GetDragDropDisplayText());
					}
				}
				else if (HoveredTemplate == nullptr || DraggedTemplate == nullptr)
				{
					if (HoveredTemplate == nullptr)
					{
						// Can't attach to non-USceneComponent types
						Message = LOCTEXT("DropActionToolTip_Error_NotAttachable_NotSceneComponent", "Cannot attach to this component as it is not a scene component.");
					}
					else
					{
						// Can't attach non-USceneComponent types
						Message = LOCTEXT("DropActionToolTip_Error_NotAttachable", "Cannot attach to this component.");
					}
				}
				else if (NodePtr == SceneRootNodePtr)
				{
					bool bCanMakeNewRoot = false;
					bool bCanAttachToRoot = !DraggedNodePtr->IsDirectlyAttachedTo(NodePtr)
						&& HoveredTemplate->CanAttachAsChild(DraggedTemplate, NAME_None)
						&& DraggedTemplate->Mobility >= HoveredTemplate->Mobility
						&& (!HoveredTemplate->IsEditorOnly() || DraggedTemplate->IsEditorOnly());

					if (!NodePtr->CanReparent() && (!NodeData->IsDefaultSceneRoot() || NodeData->IsInheritedComponent() || NodeData->IsInstancedInheritedComponent()))
					{
						// Cannot make the dropped node the new root if we cannot reparent the current root
						Message = LOCTEXT("DropActionToolTip_Error_CannotReparentRootNode", "The root component in this Blueprint is inherited and cannot be replaced.");
					}
					else if (DraggedTemplate->IsEditorOnly() && !HoveredTemplate->IsEditorOnly())
					{
						// can't have a new root that's editor-only (when children would be around in-game)
						Message = LOCTEXT("DropActionToolTip_Error_CannotReparentEditorOnly", "Cannot re-parent game components under editor-only ones.");
					}
					else if (DraggedTemplate->Mobility > HoveredTemplate->Mobility)
					{
						// can't have a new root that's movable if the existing root is static or stationary
						Message = LOCTEXT("DropActionToolTip_Error_CannotReparentNonMovable", "Cannot replace a non-movable scene root with a movable component.");
					}
					else if (DragRowOp->SourceNodes.Num() > 1)
					{
						Message = LOCTEXT("DropActionToolTip_Error_CannotAssignMultipleRootNodes", "Cannot replace the scene root with multiple components. Please select only a single component and try again.");
					}
					else
					{
						bCanMakeNewRoot = true;
					}

					if (bCanMakeNewRoot && bCanAttachToRoot)
					{
						// User can choose to either attach to the current root or make the dropped node the new root
						Message = LOCTEXT("DropActionToolTip_AttachToOrMakeNewRoot", "Drop here to see available actions.");
						DragRowOp->PendingDropAction = FSubobjectRowDragDropOp::DropAction_AttachToOrMakeNewRoot;
					}
					else if (SubobjectEditor.Pin()->CanMakeNewRootOnDrag(DraggedNodeData->GetBlueprint()))
					{
						if (bCanMakeNewRoot)
						{
							if (NodeData->IsDefaultSceneRoot())
							{
								// Only available action is to copy the dragged node to the other Blueprint and make it the new root
								// Default root will be deleted
								Message = FText::Format(LOCTEXT("DropActionToolTip_DropMakeNewRootNodeFromCopyAndDelete", "Drop here to copy {0} to a new variable and make it the new root. The default root will be deleted."), DraggedNodeData->GetDragDropDisplayText());
							}
							else
							{
								// Only available action is to copy the dragged node to the other Blueprint and make it the new root
								Message = FText::Format(LOCTEXT("DropActionToolTip_DropMakeNewRootNodeFromCopy", "Drop here to copy {0} to a new variable and make it the new root."), DraggedNodeData->GetDragDropDisplayText());
							}
							DragRowOp->PendingDropAction = FSubobjectRowDragDropOp::DropAction_MakeNewRoot;
						}
						else if (bCanAttachToRoot)
						{
							// Only available action is to copy the dragged node(s) to the other Blueprint and attach it to the root
							if (DragRowOp->SourceNodes.Num() > 1)
							{
								Message = FText::Format(LOCTEXT("DropActionToolTip_AttachComponentsToThisNodeFromCopyWithMultipleSelection", "Drop here to copy the selected components to new variables and attach them to {0}."), NodeData->GetDragDropDisplayText());
							}
							else
							{
								Message = FText::Format(LOCTEXT("DropActionToolTip_AttachToThisNodeFromCopy", "Drop here to copy {0} to a new variable and attach it to {1}."), DraggedNodeData->GetDragDropDisplayText(), NodeData->GetDragDropDisplayText());
							}

							DragRowOp->PendingDropAction = FSubobjectRowDragDropOp::DropAction_AttachTo;
						}
					}
					else if (bCanMakeNewRoot)
					{
						if (NodeData->IsDefaultSceneRoot())
						{
							// Only available action is to make the dragged node the new root
							// Default root will be deleted
							Message = FText::Format(LOCTEXT("DropActionToolTip_DropMakeNewRootNodeAndDelete", "Drop here to make {0} the new root. The default root will be deleted."), DraggedNodeData->GetDragDropDisplayText());
						}
						else
						{
							// Only available action is to make the dragged node the new root
							Message = FText::Format(LOCTEXT("DropActionToolTip_DropMakeNewRootNode", "Drop here to make {0} the new root."), DraggedNodeData->GetDragDropDisplayText());
						}
						DragRowOp->PendingDropAction = FSubobjectRowDragDropOp::DropAction_MakeNewRoot;
					}
					else if (bCanAttachToRoot)
					{
						// Only available action is to attach the dragged node(s) to the root
						if (DragRowOp->SourceNodes.Num() > 1)
						{
							Message = FText::Format(LOCTEXT("DropActionToolTip_AttachToThisNodeWithMultipleSelection", "Drop here to attach the selected components to {0}."), NodeData->GetDragDropDisplayText());
						}
						else
						{
							Message = FText::Format(LOCTEXT("DropActionToolTip_AttachToThisNode", "Drop here to attach {0} to {1}."), DraggedNodeData->GetDragDropDisplayText(), NodeData->GetDragDropDisplayText());
						}

						DragRowOp->PendingDropAction = FSubobjectRowDragDropOp::DropAction_AttachTo;
					}
				}
				else if (DraggedNodePtr->IsDirectlyAttachedTo(NodePtr)) // if dropped onto parent
				{
					// Detach the dropped node(s) from the current node and reattach to the root node
					if (DragRowOp->SourceNodes.Num() > 1)
					{
						Message = FText::Format(LOCTEXT("DropActionToolTip_DetachFromThisNodeWithMultipleSelection", "Drop here to detach the selected components from {0}."), NodeData->GetDragDropDisplayText());
					}
					else
					{
						Message = FText::Format(LOCTEXT("DropActionToolTip_DetachFromThisNode", "Drop here to detach {0} from {1}."), DraggedNodeData->GetDragDropDisplayText(), NodeData->GetDragDropDisplayText());
					}

					DragRowOp->PendingDropAction = FSubobjectRowDragDropOp::DropAction_DetachFrom;
				}
				else if (!DraggedTemplate->IsEditorOnly() && HoveredTemplate->IsEditorOnly())
				{
					// can't have a game component child nested under an editor-only one
					Message = LOCTEXT("DropActionToolTip_Error_CannotAttachToEditorOnly", "Cannot attach game components to editor-only ones.");
				}
				else if ((DraggedTemplate->Mobility == EComponentMobility::Static) && ((HoveredTemplate->Mobility == EComponentMobility::Movable) || (HoveredTemplate->Mobility == EComponentMobility::Stationary)))
				{
					// Can't attach Static components to mobile ones
					Message = LOCTEXT("DropActionToolTip_Error_CannotAttachStatic", "Cannot attach Static components to movable ones.");
				}
				else if ((DraggedTemplate->Mobility == EComponentMobility::Stationary) && (HoveredTemplate->Mobility == EComponentMobility::Movable))
				{
					// Can't attach Static components to mobile ones
					Message = LOCTEXT("DropActionToolTip_Error_CannotAttachStationary", "Cannot attach Stationary components to movable ones.");
				}
				else if ((NodeData->IsInstancedComponent() && HoveredTemplate->CreationMethod == EComponentCreationMethod::Native && !HoveredTemplate->HasAnyFlags(RF_DefaultSubObject)))
				{
					// Can't attach to post-construction C++-added components as they exist outside of the CDO and are not known at SCS execution time
					Message = LOCTEXT("DropActionToolTip_Error_CannotAttachCPPAdded", "Cannot attach to components added in post-construction C++ code.");
				}
				else if (NodeData->IsInstancedComponent() && HoveredTemplate->CreationMethod == EComponentCreationMethod::UserConstructionScript)
				{
					// Can't attach to UCS-added components as they exist outside of the CDO and are not known at SCS execution time
					Message = LOCTEXT("DropActionToolTip_Error_CannotAttachUCSAdded", "Cannot attach to components added in the Construction Script.");
				}
				else if (HoveredTemplate->CanAttachAsChild(DraggedTemplate, NAME_None))
				{
					// Attach the dragged node(s) to this node
					if (DraggedNodeData->GetBlueprint() != NodeData->GetBlueprint())
					{
						if (DragRowOp->SourceNodes.Num() > 1)
						{
							Message = FText::Format(LOCTEXT("DropActionToolTip_AttachToThisNodeFromCopyWithMultipleSelection", "Drop here to copy the selected nodes to new variables and attach them to {0}."), NodeData->GetDragDropDisplayText());
						}
						else
						{
							Message = FText::Format(LOCTEXT("DropActionToolTip_AttachToThisNodeFromCopy", "Drop here to copy {0} to a new variable and attach it to {1}."), DraggedNodeData->GetDragDropDisplayText(), NodeData->GetDragDropDisplayText());
						}
					}
					else if (DragRowOp->SourceNodes.Num() > 1)
					{
						Message = FText::Format(LOCTEXT("DropActionToolTip_AttachToThisNodeWithMultipleSelection", "Drop here to attach the selected components to {0}."), NodeData->GetDragDropDisplayText());
					}
					else
					{
						Message = FText::Format(LOCTEXT("DropActionToolTip_AttachToThisNode", "Drop here to attach {0} to {1}."), DraggedNodeData->GetDragDropDisplayText(), NodeData->GetDragDropDisplayText());
					}

					DragRowOp->PendingDropAction = FSubobjectRowDragDropOp::DropAction_AttachTo;
				}
				else
				{
					// The dropped node cannot be attached to the current node
					Message = FText::Format(LOCTEXT("DropActionToolTip_Error_TooManyAttachments", "Unable to attach {0} to {1}."), DraggedNodeData->GetDragDropDisplayText(), NodeData->GetDragDropDisplayText());
				}
			}
		}

		const FSlateBrush* StatusSymbol = DragRowOp->PendingDropAction != FSubobjectRowDragDropOp::DropAction_None
			? FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.OK"))
			: FAppStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error"));

		if (Message.IsEmpty())
		{
			DragRowOp->SetFeedbackMessage(nullptr);
		}
		else
		{
			DragRowOp->SetSimpleFeedbackMessage(StatusSymbol, FLinearColor::White, Message);
		}
	}
	else if (Operation->IsOfType<FExternalDragOperation>() || Operation->IsOfType<FAssetDragDropOp>())
	{
		// defer to the tree widget's handler for this type of operation
		TSharedPtr<SGenericSubobjectEditor> PinnedEditor = SubobjectEditor.Pin();
		if (PinnedEditor.IsValid() && PinnedEditor->GetDragDropTree().IsValid())
		{
			// The widget geometry is irrelevant to the tree widget's OnDragEnter
			PinnedEditor->GetDragDropTree()->OnDragEnter(FGeometry(), DragDropEvent);
		}
	} // */
}

void SGenericComponent_RowWidget::HandleOnDragLeave(const FDragDropEvent& DragDropEvent)
{
}

FReply SGenericComponent_RowWidget::HandleOnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Unhandled();
}

TOptional<EItemDropZone> SGenericComponent_RowWidget::HandleOnCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, FGenericComponentListNodePtrType TargetItem)
{
	return TOptional<EItemDropZone>();
}

FReply SGenericComponent_RowWidget::HandleOnAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, FGenericComponentListNodePtrType TargetItem)
{
	return FReply::Handled();
}

/////////////////////////// End Drag And Drop ///////////////////////////

/////////////////////////// Begin Attribute Getters ///////////////////////////

FText SGenericComponent_RowWidget::GetAssetName() const
{
	const FGenericComponentListNodePtrType NodePtr = GetComponentPtr();
	if (FGenericSubobjectData* Data = NodePtr->GetDataSource())
	{
		return Data->GetAssetName();

	}
	return FText::GetEmpty();
}

FText SGenericComponent_RowWidget::GetAssetPath() const
{
	const FGenericComponentListNodePtrType NodePtr = GetComponentPtr();
	FGenericSubobjectData* Data = NodePtr->GetDataSource();

	return Data ? Data->GetAssetPath() : FText::GetEmpty();
}

FText SGenericComponent_RowWidget::GetNameLabel() const
{
	const FGenericComponentListNodePtrType NodePtr = GetComponentPtr();
	FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;
	if (Data)
	{
		// NOTE: Whatever this returns also becomes the variable name
		return FText::FromString(Data->GetDisplayString());
	}
	return LOCTEXT("UnknownDataSource", "Unknown Name Label");
}

EVisibility SGenericComponent_RowWidget::GetAssetVisibility() const
{
	const FGenericComponentListNodePtrType NodePtr = GetComponentPtr();
	FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;

	if (Data /*&& Data->IsAssetVisible()*/)
	{
		return EVisibility::Visible;
	}

	return EVisibility::Hidden;
}

const FSlateBrush* SGenericComponent_RowWidget::GetIconBrush() const
{
	const FSlateBrush* ComponentIcon = FAppStyle::GetBrush("SCS.NativeComponent");

	if (const FGenericComponentListNodePtrType NodePtr = GetComponentPtr())
	{
		if (const FGenericSubobjectData* Data = NodePtr->GetDataSource())
		{
			if (const UGenericGraphNode* Node = Cast<UGenericGraphNode>(Data->GetObject()))
			{
				auto FindIconForNode = [](const UGenericGraphNode* InNode)
				{
					FName IconName = InNode->GetCustomIconName();
					if (IconName != NAME_None)
					{
						return FSlateIconFinder::FindIcon(IconName);
					}

					// Actor didn't specify an icon - fallback on the class icon
					return FSlateIconFinder::FindIconForClass(InNode->GetClass());
				};

				ComponentIcon = FindIconForNode(Node).GetOptionalIcon();
			}
			else if (const UGenericGraphNodeComponent* ComponentTemplate = Cast<UGenericGraphNodeComponent>(Data->GetObject()))
			{
				ComponentIcon = FSlateIconFinder::FindIconBrushForClass(ComponentTemplate->GetClass(), TEXT("SCS.Component"));
			}
		}
	}

	return ComponentIcon;
}

FSlateColor SGenericComponent_RowWidget::GetColorTintForIcon() const
{
	return FSlateColor(); // SubobjectEditor.Pin()->GetColorTintForIcon(GetSubobjectPtr());
}

/////////////////////////// End Attribute Getters ///////////////////////////

/////////////////////////// Begin Tool Tip Widget ///////////////////////////

TSharedRef<SToolTip> SGenericComponent_RowWidget::CreateToolTipWidget() const
{
	FGenericComponentListNodePtrType TreeNode = GetComponentPtr();
	check(TreeNode);
	const FGenericSubobjectData* TreeNodeData = TreeNode->GetDataSource();
	check(TreeNodeData);

	// if (TreeNode->IsComponentNode())
	{
		return CreateComponentTooltipWidget(TreeNode);
	}
	// else
	{
		return CreateActorTooltipWidget(TreeNode);
	}
}

TSharedRef<SToolTip> SGenericComponent_RowWidget::CreateComponentTooltipWidget(const FGenericComponentListNodePtrType& InNode) const
{
	// Create a box to hold every line of info in the body of the tooltip
	TSharedRef<SVerticalBox> InfoBox = SNew(SVerticalBox);

	if (FGenericComponentListNodePtrType TreeNode = GetComponentPtr())
	{
		const FGenericSubobjectData* TreeNodeData = TreeNode->GetDataSource();
		check(TreeNodeData);
		if (const UGenericGraphNodeComponent* Template = Cast<UGenericGraphNodeComponent>(TreeNodeData->GetObject()))
		{
			UClass* TemplateClass = Template->GetClass();
			FText ClassTooltip = TemplateClass->GetToolTipText(/*bShortTooltip=*/ true);

			InfoBox->AddSlot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				.Padding(FMargin(0, 2, 0, 4))
				[
					SNew(STextBlock)
						.TextStyle(FAppStyle::Get(), "SCSEditor.ComponentTooltip.ClassDescription")
						.Text(ClassTooltip)
						.WrapTextAt(400.0f)
				];

			// Add introduction point
			/* AddToToolTipInfoBox(InfoBox, LOCTEXT("TooltipAddType", "Source"), SNullWidget::NullWidget, TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SGenericComponent_RowWidget::GetComponentAddSourceToolTipText)), false);
			if (TreeNodeData->IsInheritedComponent())
			{
				AddToToolTipInfoBox(InfoBox, LOCTEXT("TooltipIntroducedIn", "Introduced in"), SNullWidget::NullWidget, TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SGenericComponent_RowWidget::GetIntroducedInToolTipText)), false);
			}

			// Add Underlying Component Name for Native Components
			if (TreeNodeData->IsNativeComponent())
			{
				AddToToolTipInfoBox(InfoBox, LOCTEXT("TooltipNativeComponentName", "Native Component Name"), SNullWidget::NullWidget, TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SGenericComponent_RowWidget::GetNativeComponentNameToolTipText)), false);
			}

			// Add mobility
			TSharedRef<SImage> MobilityIcon = SNew(SImage).Image(this, &SGenericComponent_RowWidget::GetMobilityIconImage);
			AddToToolTipInfoBox(InfoBox, LOCTEXT("TooltipMobility", "Mobility"), MobilityIcon, TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SGenericComponent_RowWidget::GetMobilityToolTipText)), false);
			//
			// Add asset if applicable to this node
			if (GetAssetVisibility() == EVisibility::Visible)
			{
				InfoBox->AddSlot()[SNew(SSpacer).Size(FVector2D(1.0f, 8.0f))];
				AddToToolTipInfoBox(InfoBox, LOCTEXT("TooltipAsset", "Asset"), SNullWidget::NullWidget,
					TAttribute<FText>(this, &SGenericComponent_RowWidget::GetAssetName), false);
			}

			// If the component is marked as editor only, then display that info here
			AddToToolTipInfoBox(InfoBox, LOCTEXT("TooltipEditorOnly", "Editor Only"), SNullWidget::NullWidget, TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SGenericComponent_RowWidget::GetComponentEditorOnlyTooltipText)), false);
			*/
		}
	}

	TSharedRef<SBorder> TooltipContent = SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("NoBorder"))
		.Padding(0)
		[
			SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 4)
				[
					SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								.Padding(2)
								[
									SNew(STextBlock)
										.TextStyle(FAppStyle::Get(), "SCSEditor.ComponentTooltip.Title")
										.Text(this, &SGenericComponent_RowWidget::GetTooltipText)
								]
						]
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
						.BorderImage(FAppStyle::GetBrush("NoBorder"))
						.Padding(2)
						[
							InfoBox
						]
				]
		];

	return IDocumentation::Get()->CreateToolTip(TAttribute<FText>(this, &SGenericComponent_RowWidget::GetTooltipText), TooltipContent, InfoBox, GetDocumentationLink(), GetDocumentationExcerptName());
}

TSharedRef<SToolTip> SGenericComponent_RowWidget::CreateActorTooltipWidget(const FGenericComponentListNodePtrType& InNode) const
{
	return CreateComponentTooltipWidget(InNode);
}

FText SGenericComponent_RowWidget::GetTooltipText() const
{
	const FGenericSubobjectData* Data = GetComponentPtr()->GetDataSource();

	if (!Data)
	{
		return FText::GetEmpty();
	}

	/* if (Data->IsDefaultSceneRoot())
	{
		if (Data->IsInheritedComponent())
		{
			return LOCTEXT("InheritedDefaultSceneRootToolTip",
				"This is the default scene root component. It cannot be renamed or deleted.\nIt has been inherited from the parent class, so its properties cannot be edited here.\nNew scene components will automatically be attached to it.");
		}
		else
		{
			if (Data->CanDelete())
			{
				return LOCTEXT("DefaultSceneRootDeletableToolTip",
					"This is the default scene root component.\nIt can be replaced by drag/dropping another scene component over it.");
			}
			else
			{
				return LOCTEXT("DefaultSceneRootToolTip",
					"This is the default scene root component. It cannot be renamed or deleted.\nIt can be replaced by drag/dropping another scene component over it.");
			}
		}
	} */

	if (const UGenericGraphNodeComponent* Component = Cast<UGenericGraphNodeComponent>(Data->GetObject()))
	{
		const UClass* Class = Component->GetClass();
		const FText ClassDisplayName = Class ? Class->GetDisplayNameText() : LOCTEXT("ClassIsNull", "None");

		FFormatNamedArguments Args;
		Args.Add(TEXT("ClassName"), ClassDisplayName);
		Args.Add(TEXT("NodeName"), FText::FromString(Component->GetFName().ToString()));

		return FText::Format(LOCTEXT("ComponentTooltip", "{NodeName} ({ClassName})"), Args);
	}

	return FText::GetEmpty();
}

FText SGenericComponent_RowWidget::GetObjectContextText() const
{
	const FGenericComponentListNodePtrType NodePtr = GetComponentPtr();
	const FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;

	// We want to display (Self) or (Instance) only if the data is an actor
	if (Data)
	{
		// return Data->GetDisplayNameContextModifiers(GetDefault<UEditorStyleSettings>()->bShowNativeComponentNames);
	}
	return FText::GetEmpty();
}

FString SGenericComponent_RowWidget::GetDocumentationLink() const
{
	const FGenericComponentListNodePtrType NodePtr = GetComponentPtr();
	const FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;

	/* if (Data && (NodePtr == SubobjectEditor.Pin()->GetSceneRootNode() || Data->IsInheritedComponent()))
	{
		return TEXT("Shared/Editors/BlueprintEditor/ComponentsMode");
	} */

	return TEXT("");
}

FString SGenericComponent_RowWidget::GetDocumentationExcerptName() const
{
	const FGenericComponentListNodePtrType NodePtr = GetComponentPtr();
	const FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;

	/* if (NodePtr && Data)
	{
		if (Data->IsNativeComponent())
		{
			return TEXT("NativeComponents");
		}
		else if (Data->IsInheritedComponent())
		{
			return TEXT("InheritedComponents");
		}
	} */

	return TEXT("");
}

/////////////////////////// End Tool Tip Widget ///////////////////////////

void SGenericComponent_RowWidget::OnNameTextCommit(const FText& InNewName, ETextCommit::Type InTextCommit)
{
	// Ask the subsystem to rename this
	const FGenericComponentListNodePtrType NodePtr = GetComponentPtr();
	if (ensure(NodePtr))
	{
		// If there is already an ongoing transaction then use that, otherwise make a
		// new transaction for renaming the component
		TUniquePtr<FScopedTransaction> Transaction;
		NodePtr->GetOngoingCreateTransaction(Transaction);

		// If a 'create' transaction is opened, the rename will be folded into it and will be invisible to the
		// 'undo' as create + give a name is really just one operation from the user point of view.
		FScopedTransaction TransactionContext(LOCTEXT("RenameComponentVariable", "Rename Component Variable"));

		// Ask the subsystem to verify this rename option for us
		UGenericSubobjectDataSubsystem* System = UGenericSubobjectDataSubsystem::Get();
		check(System);
		const bool bSuccess = System->RenameSubobject(NodePtr->GetDataHandle(), InNewName);

		Transaction.Reset();
	}
}

bool SGenericComponent_RowWidget::OnNameTextVerifyChanged(const FText& InNewText, FText& OutErrorMessage)
{
	const FGenericComponentListNodePtrType NodePtr = GetComponentPtr();
	if (ensure(NodePtr))
	{
		// Ask the subsystem to verify this rename option for us
		const UGenericSubobjectDataSubsystem* System = UGenericSubobjectDataSubsystem::Get();
		check(System);
		return System->IsValidRename(NodePtr->GetDataHandle(), InNewText, OutErrorMessage);
	}

	return false;
}

bool SGenericComponent_RowWidget::IsReadOnly() const
{
	const FGenericComponentListNodePtrType NodePtr = GetComponentPtr();
	const FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;
	if (Data)
	{
		// return !Data->CanRename() || (SubobjectEditor.IsValid() && !SubobjectEditor.Pin()->IsEditingAllowed());
	}

	// If there is no valid data then default to read only
	return true;
}

/////////////////////////////////////////////////////
// Wrapper Struct FGenericComponentListNode

FGenericComponentListNode::FGenericComponentListNode(const FGenericSubobjectDataHandle& SubobjectDataSource, bool InbIsSeperator)
	: DataHandle(SubobjectDataSource)
	, FilterFlags((uint8)EFilteredState::Unknown)
	, bIsSeperator(InbIsSeperator)
{
	check(IsValid());
}

FString FGenericComponentListNode::GetDisplayString() const
{
	FGenericSubobjectData* Data = GetDataSource();
	return Data ? Data->GetDisplayString() : TEXT("Unknown");
}

bool FGenericComponentListNode::MatchesFilterType(const UClass* InFilterType) const
{
	if (GetDataSource() && GetDataSource()->GetObject() && GetDataSource()->GetObject()->GetClass())
	{
		if (!GetDataSource()->GetObject()->GetClass()->IsChildOf(InFilterType))
		{
			return false;
		}
	}

	return true;
}

bool FGenericComponentListNode::RefreshFilteredState(const UClass* InFilterType, const TArray<FString>& InFilterTerms)
{
	bool bIsFilteredOut = InFilterType && !MatchesFilterType(InFilterType);
	if (!bIsFilteredOut && !IsSeperator())
	{
		FString DisplayStr = GetDisplayString();
		for (const FString& FilterTerm : InFilterTerms)
		{
			if (!DisplayStr.Contains(FilterTerm))
			{
				bIsFilteredOut = true;
			}
		}
	}

	SetCachedFilterState(!bIsFilteredOut);
	return !bIsFilteredOut;
}

bool FGenericComponentListNode::IsFlaggedForFiltration() const
{
	return FilterFlags != EFilteredState::Unknown ? (FilterFlags & EFilteredState::FilteredInMask) == 0 : false;
}

void FGenericComponentListNode::SetCachedFilterState(bool bMatchesFilter)
{
	bool bFlagsChanged = false;
	if ((FilterFlags & EFilteredState::Unknown) == EFilteredState::Unknown)
	{
		FilterFlags = 0x00;
		bFlagsChanged = true;
	}

	if (bMatchesFilter)
	{
		bFlagsChanged |= (FilterFlags & EFilteredState::MatchesFilter) == 0;
		FilterFlags |= EFilteredState::MatchesFilter;
	}
	else
	{
		bFlagsChanged |= (FilterFlags & EFilteredState::MatchesFilter) != 0;
		FilterFlags &= ~EFilteredState::MatchesFilter;
	}

	// We have no parent, or children.
	// const bool bHadChildMatch = (FilterFlags & EFilteredState::ChildMatches) != 0;

	// refresh the cached child state (don't update the parent, we'll do that below if it's needed)
	// RefreshCachedChildFilterState(/*bUpdateParent =*/false);

	/*bFlagsChanged |= bHadChildMatch != ((FilterFlags & EFilteredState::ChildMatches) != 0);
	if (bUpdateParent && bFlagsChanged)
	{
		ApplyFilteredStateToParent();
	}*/
}

void FGenericComponentListNode::SetOngoingCreateTransaction(TUniquePtr<FScopedTransaction> InTransaction)
{
}

void FGenericComponentListNode::CloseOngoingCreateTransaction()
{
}

bool FGenericComponentListNode::CanDelete() const
{
	if (const FGenericSubobjectData* Data = GetDataSource())
	{
		return Data->CanDelete();
	}
	return false;
}

bool FGenericComponentListNode::CanRename() const
{
	if (const FGenericSubobjectData* Data = GetDataSource())
	{
		return Data->CanRename();
	}
	return false;
}

#undef LOCTEXT_NAMESPACE