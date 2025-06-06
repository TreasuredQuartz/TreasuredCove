#include "GenericGraphAssetEditor/SEdNode_GenericGraphNode.h"
#include "GenericGraphAssetEditor/Colors_GenericGraph.h"
#include "GenericGraphAssetEditor/EdNode_GenericGraphNode.h"
#include "GenericGraphAssetEditor/GenericGraphDragConnection.h"
#include "GenericGraphAssetEditor/SGenericSubobjectEditor.h"
#include "GenericGraphAssetEditor/SGenericNodeComponentClassCombo.h"
#include "GenericGraphEditorPCH.h"

#include "SLevelOfDetailBranchNode.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "SCommentBubble.h"
#include "SlateOptMacros.h"
#include "SGraphPin.h"
#include "GraphEditorSettings.h"

#include "ClassViewerFilter.h"

// Generic Node Component Include list

#include "GenericComponentAssetBroker.h"
#include "GenericSubobjectDataHandle.h"
#include "TutorialMetaData.h"
#include "Selection.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "EdNode_GenericGraph"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SGenericNodeComponentList::Construct(const FArguments& InArgs, UGenericGraphNode* InNode)
{
	GenericGraphNode = InNode;

	TSharedPtr<SHeaderRow> HeaderRow = SNew(SHeaderRow)
		+ SHeaderRow::Column(FName("ComponentClass"))
		.DefaultLabel(LOCTEXT("Class", "Class"))
		.FillWidth(4);

	ListWidget = SNew(SListView<FGenericComponentListNodePtrType>)
		.ListItemsSource(&NodeList)
		.OnGenerateRow(this, &SGenericNodeComponentList::GenerateListRow)
		.ToolTipText(LOCTEXT("DropAssetToAddComponent", "Drop asset here to add a component."))
		.SelectionMode(ESelectionMode::Multi)
		.ItemHeight(24)
		.HeaderRow
		(
			HeaderRow
		);

	HeaderRow->SetVisibility(EVisibility::Collapsed);

	TSharedPtr<SVerticalBox> HeaderBox;
	TSharedPtr<SWidget> Contents;
	TSharedPtr<SHorizontalBox> ButtonBox = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(0.0f, 0.0f, 4.0f, 0.0f)
		.AutoWidth()
		[
			SNew(SGenericNodeComponentClassCombo)
				.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("GenericGraphNode.AddComponent")))
				// .Visibility(this, &SGenericSubobjectEditor::GetComponentClassComboButtonVisibility)
				.OnSubobjectClassSelected(this, &SGenericNodeComponentList::PerformComboAddClass)
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

	Contents = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		.Padding(4.f, 0, 4.f, 4.f)
		[
			SAssignNew(HeaderBox, SVerticalBox)
		]

		+ SVerticalBox::Slot()
		[
			SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("SCSEditor.Background"))
				.Padding(4.f)
				.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("ComponentsPanel")))
				// .Visibility(this, &SGenericSubobjectEditor::GetComponentsTreeVisibility)
				[
					ListWidget.ToSharedRef()
				]
		];


	ButtonBox->AddSlot()
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.Padding(3.0f, 3.0f)
		[
			// SearchBar.ToSharedRef()
			SNew(SBox)
		];

	HeaderBox->AddSlot()
		.VAlign(VAlign_Center)
		.AutoHeight()
		[
			ButtonBox.ToSharedRef()
		];

	this->ChildSlot
		[
			Contents.ToSharedRef()
		];

	UpdateTree();
}

void SGenericNodeComponentList::UpdateTree()
{
	check(ListWidget.IsValid());

	// Obtain the list of selected items
	// TArray<FSubobjectEditorTreeNodePtrType> SelectedTreeNodes = GetSelectedNodes();

	// Clear the current tree
	// if (SelectedTreeNodes.Num() != 0)
	{
		ListWidget->ClearSelection();
	}
	NodeList.Empty();

	if (GenericGraphNode)
	{
		TArray<UGenericGraphNodeComponent*> Components = GenericGraphNode->GetComponents();

		if (Components.Num() > 0)
		{
			for (UGenericGraphNodeComponent* Component : Components)
			{
				FGenericComponentListNodePtrType Node = MakeShareable<FGenericComponentListNode>(
					new FGenericComponentListNode(Component));

				NodeList.Add(Node);

				// RefreshFilteredState(Node, false);
			}
		}

		// RestoreSelectionState(SelectedTreeNodes);

		// If we have a pending deferred rename request, redirect it to the new tree node
		/* if (DeferredRenameRequest.IsValid())
		{
			FGenericComponentListNodePtrType NodeToRenamePtr = FindSlateNodeForHandle(DeferredRenameRequest);
			if (NodeToRenamePtr.IsValid())
			{
				ListWidget->RequestScrollIntoView(NodeToRenamePtr);
			}
		} */
	}

	ListWidget->RequestListRefresh();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FGenericSubobjectDataHandle SGenericNodeComponentList::AddNewSubobject(UClass* NewClass, UObject* Asset, FText& FailReason)
{
	FGenericSubobjectDataHandle NewComponentHandle = FGenericSubobjectDataHandle::InvalidHandle;

	// Ensure that the new class is actually a valid subobject type.
	// As of right now, that means the class has to be a child of an Generic Graph Node Component. 
	if (!NewClass->IsChildOf(UGenericGraphNodeComponent::StaticClass()))
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

		// NewComponentHandle = FGenericSubobjectDataHandle(); 
	}

	return NewComponentHandle;
}

FGenericSubobjectDataHandle SGenericNodeComponentList::PerformComboAddClass(TSubclassOf<UGenericGraphNodeComponent> ComponentClass, EComponentCreateAction::Type ComponentCreateAction, UObject* AssetOverride)
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

TSharedRef<ITableRow> SGenericNodeComponentList::GenerateListRow(FGenericComponentListNodePtrType InNodePtr, const TSharedRef<STableViewBase>& OwnerTable)
{
	FGraphNodeMetaData TagMeta(TEXT("TableRow"));
	const UGenericGraphNodeComponent* Template = InNodePtr && InNodePtr->Data
		? Cast<UGenericGraphNodeComponent>(InNodePtr->Data)
		: nullptr;
	if (Template)
	{
		TagMeta.FriendlyName = FString::Printf(TEXT("TableRow,%s,0"), *Template->GetReadableName());
	}

	return SNew(STableRow<FGenericComponentListNodePtrType>, OwnerTable);
		// SNew(SSubobject_RowWidget, SharedThis(this), InNodePtr, OwnerTable)
		// .AddMetaData<FTutorialMetaData>(TagMeta);
}

//////////////////////////////////////////////////////////////////////////

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SGenericComponent_RowWidget::Construct(const FArguments& InArgs, TWeakPtr<SGenericNodeComponentList> InEditor, FGenericComponentListNodePtrType InNodePtr, TSharedPtr<STableViewBase> InOwnerTableView)
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

	/* FGenericSubobjectData* Data = Node->GetDataSource();

	if (ColumnName == SubobjectTree_ColumnName_ComponentClass)
	{
		InlineWidget =
			SNew(SInlineEditableTextBlock)
			.Text(this, &SGenericComponent_RowWidget::GetNameLabel)
			.OnVerifyTextChanged(this, &SGenericComponent_RowWidget::OnNameTextVerifyChanged)
			.OnTextCommitted(this, &SGenericComponent_RowWidget::OnNameTextCommit)
			.IsSelected(this, &SGenericComponent_RowWidget::IsSelectedExclusively)
			.IsReadOnly(this, &SGenericComponent_RowWidget::IsReadOnly);

		Node->SetRenameRequestedDelegate(FSubobjectEditorTreeNode::FOnRenameRequested::CreateSP(InlineWidget.Get(), &SInlineEditableTextBlock::EnterEditingMode));

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
				GetInheritedLinkWidget()
			];

		return Contents;
	}
	else if (ColumnName == SubobjectTree_ColumnName_Asset)
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
	else if (ColumnName == SubobjectTree_ColumnName_Mobility)
	{
		return SNew(SSpacer);
	}

	return SNew(STextBlock)
		.Text(LOCTEXT("UnknownColumn", "Unknown Column")); */

	return SNew(SBox);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

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

//////////////////////////////////////////////////////////////////////////
class SGenericGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SGenericGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		this->SetCursor(EMouseCursor::Default);

		bShowLabel = true;

		GraphPinObj = InPin;
		check(GraphPinObj != nullptr);

		const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
		check(Schema);

		SBorder::Construct(SBorder::FArguments()
			.BorderImage(this, &SGenericGraphPin::GetPinBorder)
			.BorderBackgroundColor(this, &SGenericGraphPin::GetPinColor)
			.OnMouseButtonDown(this, &SGenericGraphPin::OnPinMouseDown)
			.Cursor(this, &SGenericGraphPin::GetPinCursor)
			.Padding(FMargin(5.0f))
		);
	}

protected:
	virtual FSlateColor GetPinColor() const override
	{
		return GenericGraphColors::Pin::Default;
	}

	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override
	{
		return SNew(STextBlock);
	}

	const FSlateBrush* GetPinBorder() const
	{
		return FAppStyle::GetBrush(TEXT("Graph.StateNode.Body"));
	}

	virtual TSharedRef<FDragDropOperation> SpawnPinDragEvent(const TSharedRef<class SGraphPanel>& InGraphPanel, const TArray< TSharedRef<SGraphPin> >& InStartingPins) override
	{
		FGenericGraphDragConnection::FDraggedPinTable PinHandles;
		PinHandles.Reserve(InStartingPins.Num());
		// since the graph can be refreshed and pins can be reconstructed/replaced 
		// behind the scenes, the DragDropOperation holds onto FGraphPinHandles 
		// instead of direct widgets/graph-pins
		for (const TSharedRef<SGraphPin>& PinWidget : InStartingPins)
		{
			PinHandles.Add(PinWidget->GetPinObj());
		}

		return FGenericGraphDragConnection::New(InGraphPanel, PinHandles);
	}

};

/** Class viewer filter proxy for imported namespace type selectors, controlled by a custom filter option */
class FImportedClassViewerFilterProxy : public IClassViewerFilter, public TSharedFromThis<FImportedClassViewerFilterProxy>
{
public:
	FImportedClassViewerFilterProxy()
		: ClassViewerFilter()
		, bIsFilterEnabled(false)
	{
	}

	// IClassViewerFilter interface
	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
	{
		if (bIsFilterEnabled && ClassViewerFilter.IsValid())
		{
			return ClassViewerFilter->IsClassAllowed(InInitOptions, InClass, InFilterFuncs);
		}

		if (!InClass->IsChildOf(UGenericGraphNodeComponent::StaticClass())) {
			return false;
		}

		return true;
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InBlueprint, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		if (bIsFilterEnabled && ClassViewerFilter.IsValid())
		{
			return ClassViewerFilter->IsUnloadedClassAllowed(InInitOptions, InBlueprint, InFilterFuncs);
		}

		if (!InBlueprint->IsChildOf(UGenericGraphNodeComponent::StaticClass())) {
			return false;
		}

		return true;
	}

	virtual void GetFilterOptions(TArray<TSharedRef<FClassViewerFilterOption>>& OutFilterOptions)
	{
		if (!ToggleFilterOption.IsValid())
		{
			ToggleFilterOption = MakeShared<FClassViewerFilterOption>();
			ToggleFilterOption->bEnabled = bIsFilterEnabled;
			ToggleFilterOption->LabelText = LOCTEXT("ClassViewerNamespaceFilterMenuOptionLabel", "Show Only Imported Types");
			ToggleFilterOption->ToolTipText = LOCTEXT("ClassViewerNamespaceFilterMenuOptionToolTip", "Don't include non-imported class types.");
			ToggleFilterOption->OnOptionChanged = FOnClassViewerFilterOptionChanged::CreateSP(this, &FImportedClassViewerFilterProxy::OnFilterOptionChanged);
		}

		OutFilterOptions.Add(ToggleFilterOption.ToSharedRef());
	}

protected:
	void OnFilterOptionChanged(bool bIsEnabled)
	{
		bIsFilterEnabled = bIsEnabled;
	}

private:
	/** Imported namespace class viewer filter. */
	TSharedPtr<IClassViewerFilter> ClassViewerFilter;

	/** Filter option for the class viewer settings menu. */
	TSharedPtr<FClassViewerFilterOption> ToggleFilterOption;

	/** Whether or not the filter is enabled. */
	bool bIsFilterEnabled;
};

//////////////////////////////////////////////////////////////////////////
void SEdNode_GenericGraphNode::Construct(const FArguments& InArgs, UEdNode_GenericGraphNode* InNode)
{
	GraphNode = InNode;

	// Create imported namespace type filters for value editing.
	ImportedClassViewerFilter = MakeShared<FImportedClassViewerFilterProxy>();

	UpdateGraphNode();
	InNode->SEdNode = this;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SEdNode_GenericGraphNode::UpdateGraphNode()
{
	const FMargin NodePadding = FMargin(5);
	const FMargin NamePadding = FMargin(2);

	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	const FSlateBrush* NodeTypeIcon = GetNameIcon();

	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);
	TSharedPtr<SErrorText> ErrorText;
	TSharedPtr<SVerticalBox> NodeBody;
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);
	UEdNode_GenericGraphNode* EdNode = Cast<UEdNode_GenericGraphNode>(GraphNode);
	UGenericGraphNode* GenericGraphNode = EdNode->GenericGraphNode ? EdNode->GenericGraphNode : nullptr;

	TArray<TSharedRef<IClassViewerFilter>> ClassFilters;
	if (ImportedClassViewerFilter.IsValid())
	{
		ClassFilters.Add(ImportedClassViewerFilter.ToSharedRef());
	}

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0.0f)
			.BorderBackgroundColor(this, &SEdNode_GenericGraphNode::GetBorderBackgroundColor)
			[
				SNew(SOverlay)

				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SVerticalBox)

					// Input Pin Area
					+ SVerticalBox::Slot()
					.FillHeight(1)
					[
						SAssignNew(LeftNodeBox, SVerticalBox)
					]

					// Output Pin Area	
					+ SVerticalBox::Slot()
					.FillHeight(1)
					[
						SAssignNew(RightNodeBox, SVerticalBox)
					]
				]

				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(8.0f)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("Graph.StateNode.ColorSpill"))
					.BorderBackgroundColor(TitleShadowColor)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Visibility(EVisibility::SelfHitTestInvisible)
					.Padding(6.0f)
					[
						SAssignNew(NodeBody, SVerticalBox)
									
						// Title
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SHorizontalBox)

							// Error message
							+ SHorizontalBox::Slot()
							.AutoWidth()
							[
								SAssignNew(ErrorText, SErrorText)
								.BackgroundColor(this, &SEdNode_GenericGraphNode::GetErrorColor)
								.ToolTipText(this, &SEdNode_GenericGraphNode::GetErrorMsgToolTip)
							]

							// Icon
							+ SHorizontalBox::Slot()
							.AutoWidth()
							.VAlign(VAlign_Center)
							[
								SNew(SImage)
								.Image(NodeTypeIcon)
							] 
										
							// Node Title
							+ SHorizontalBox::Slot()
							.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
							.FillWidth(1.0)
							[
								SNew(SVerticalBox)
								+ SVerticalBox::Slot()
								.AutoHeight()
								[
									SAssignNew(InlineEditableText, SInlineEditableTextBlock)
									.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
									.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
									.OnVerifyTextChanged(this, &SEdNode_GenericGraphNode::OnVerifyNameTextChanged)
									.OnTextCommitted(this, &SEdNode_GenericGraphNode::OnNameTextCommited)
									.IsReadOnly(this, &SEdNode_GenericGraphNode::IsNameReadOnly)
									.IsSelected(this, &SEdNode_GenericGraphNode::IsSelectedExclusively)
								]
								+ SVerticalBox::Slot()
								.AutoHeight()
								[
									NodeTitle.ToSharedRef()
								]
							]
						]

						// Body
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SGenericNodeComponentList, GenericGraphNode)
						]
					]
				]
			]
		];

	// Create comment bubble
	TSharedPtr<SCommentBubble> CommentBubble;
	const FSlateColor CommentColor = GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;

	SAssignNew(CommentBubble, SCommentBubble)
		.GraphNode(GraphNode)
		.Text(this, &SGraphNode::GetNodeComment)
		.OnTextCommitted(this, &SGraphNode::OnCommentTextCommitted)
		.ColorAndOpacity(CommentColor)
		.AllowPinning(true)
		.EnableTitleBarBubble(true)
		.EnableBubbleCtrls(true)
		.GraphLOD(this, &SGraphNode::GetCurrentLOD)
		.IsGraphNodeHovered(this, &SGraphNode::IsHovered);

	GetOrAddSlot(ENodeZone::TopCenter)
		.SlotOffset(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetOffset))
		.SlotSize(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetSize))
		.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
		.VAlign(VAlign_Top)
		[
			CommentBubble.ToSharedRef()
		];

	ErrorReporting = ErrorText;
	ErrorReporting->SetError(ErrorMsg);
	// ListViewWidget->RequestListRefresh();
	CreatePinWidgets();
}

void SEdNode_GenericGraphNode::CreatePinWidgets()
{
	UEdNode_GenericGraphNode* StateNode = CastChecked<UEdNode_GenericGraphNode>(GraphNode);

	for (int32 PinIdx = 0; PinIdx < StateNode->Pins.Num(); PinIdx++)
	{
		UEdGraphPin* MyPin = StateNode->Pins[PinIdx];
		if (!MyPin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SGenericGraphPin, MyPin);

			AddPin(NewPin.ToSharedRef());
		}
	}
}

void SEdNode_GenericGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility( TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced) );
	}

	TSharedPtr<SVerticalBox> PinBox;
	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		PinBox = LeftNodeBox;
		InputPins.Add(PinToAdd);
	}
	else // Direction == EEdGraphPinDirection::EGPD_Output
	{
		PinBox = RightNodeBox;
		OutputPins.Add(PinToAdd);
	}

	if (PinBox)
	{
		PinBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.0f)
			//.Padding(6.0f, 0.0f)
			[
				PinToAdd
			];
	}
}

TSharedRef<SWidget> SEdNode_GenericGraphNode::GenerateComponentPicker()
{
	return SNew(SBox);

	/* FOnClassPicked OnPicked(FOnClassPicked::CreateSP(this, &SEdNode_GenericGraphNode::OnComponentPicked));

	/* return SNew(SBox)
		.WidthOverride(280.0f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.MaxHeight(500.0f)
			[
				FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer").CreateClassViewer(ComponentComponentClassViewerOptions, OnPicked)
			]
		]; */
}

void SEdNode_GenericGraphNode::OnComponentPicked(UClass* InClass)
{
	if (!InClass)
	{
		SendToObjects(TEXT("None"));
	}
	else
	{
		SendToObjects(InClass->GetPathName());
	}

	AddComponentButton->SetIsOpen(false);
}

void SEdNode_GenericGraphNode::SendToObjects(const FString& NewValue)
{
	/* if (PropertyEditor.IsValid())
	{
		const TSharedRef<IPropertyHandle> PropertyHandle = PropertyEditor->GetPropertyHandle();
		PropertyHandle->SetValueFromFormattedString(NewValue);
	}
	else*/if (!NewValue.IsEmpty() && NewValue != TEXT("None"))
	{
		auto FindOrLoadClass = [](const FString& ClassName) {
			UClass* Class = UClass::TryFindTypeSlow<UClass>(ClassName, EFindFirstObjectOptions::EnsureIfAmbiguous);

			if (!Class)
			{
				Class = LoadObject<UClass>(nullptr, *ClassName);
			}

			return Class;
		};

		const UClass* NewClass = FindOrLoadClass(NewValue);
		OnSetClass.Execute(NewClass);
	}
	else
	{
		OnSetClass.Execute(nullptr);
	}
}

void SEdNode_GenericGraphNode::CreateClassFilterObtions()
{
	/*ComponentClassViewerOptions.bShowBackgroundBorder = false;
	ComponentClassViewerOptions.bShowUnloadedBlueprints = true;

	const bool bAllowNone = false;
	const bool bIsBlueprintBaseOnly = false;
	const bool bAllowOnlyPlaceable = false;
	const bool bShowDisplayNames = false;
	const bool bShowTree = false;
	const bool bShowViewOptions = false;
	TArray<TSharedRef<IClassViewerFilter>> InClassFilters;
	InClassFilters.Add(MakeShared<FClassViewerFilter>(FClassViewerInitializationOptions()));

	ComponentClassViewerOptions.bShowNoneOption = bAllowNone;
	ComponentClassViewerOptions.bIsBlueprintBaseOnly = bIsBlueprintBaseOnly;
	ComponentClassViewerOptions.bIsPlaceableOnly = bAllowOnlyPlaceable;
	ComponentClassViewerOptions.NameTypeToDisplay = (bShowDisplayNames ? EClassViewerNameTypeToDisplay::DisplayName : EClassViewerNameTypeToDisplay::ClassName);
	ComponentClassViewerOptions.DisplayMode = bShowTree ? EClassViewerDisplayMode::TreeView : EClassViewerDisplayMode::ListView;
	ComponentClassViewerOptions.bAllowViewOptions = bShowViewOptions;
	ComponentClassViewerOptions.ClassFilters.Append(InClassFilters);*/

	/*TSharedRef<FPropertyEditorClassFilter> PropEdClassFilter = MakeShared<FPropertyEditorClassFilter>();
	PropEdClassFilter->ClassPropertyMetaClass = MetaClass;
	PropEdClassFilter->InterfaceThatMustBeImplemented = RequiredInterface;
	PropEdClassFilter->bAllowAbstract = bAllowAbstract;
	PropEdClassFilter->AllowedClassFilters = AllowedClassFilters;
	PropEdClassFilter->DisallowedClassFilters = DisallowedClassFilters;

	ComponentClassViewerOptions.ClassFilters.Add(PropEdClassFilter);*/
}

TSharedRef<ITableRow> SEdNode_GenericGraphNode::OnGenerateRowForList(TSharedPtr<FText> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	// Create the row
	if (!Item.IsValid()) return SNew(STableRow< TSharedPtr<FText> >, OwnerTable);

	return
		SNew(STableRow< TSharedPtr<FText> >, OwnerTable)
		.Padding(2.0f)
		[
			SNew(STextBlock)
			.Text(*Item.Get())
		];
}

bool SEdNode_GenericGraphNode::IsNameReadOnly() const
{
	UEdNode_GenericGraphNode* EdNode_Node = Cast<UEdNode_GenericGraphNode>(GraphNode);
	check(EdNode_Node != nullptr);

	UGenericGraph* GenericGraph = EdNode_Node->GenericGraphNode->Graph;
	check(GenericGraph != nullptr);

	return (!GenericGraph->bCanRenameNode || !EdNode_Node->GenericGraphNode->IsNameEditable()) || SGraphNode::IsNameReadOnly();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SEdNode_GenericGraphNode::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	SGraphNode::OnNameTextCommited(InText, CommitInfo);

	UEdNode_GenericGraphNode* MyNode = CastChecked<UEdNode_GenericGraphNode>(GraphNode);

	if (MyNode != nullptr && MyNode->GenericGraphNode != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("GenericGraphEditorRenameNode", "Generic Graph Editor: Rename Node"));
		MyNode->Modify();
		MyNode->GenericGraphNode->Modify();
		MyNode->GenericGraphNode->SetNodeTitle(InText);
		UpdateGraphNode();
	}
}

FSlateColor SEdNode_GenericGraphNode::GetBorderBackgroundColor() const
{
	UEdNode_GenericGraphNode* MyNode = CastChecked<UEdNode_GenericGraphNode>(GraphNode);
	return MyNode ? MyNode->GetBackgroundColor() : GenericGraphColors::NodeBorder::HighlightAbortRange0;
}

FSlateColor SEdNode_GenericGraphNode::GetBackgroundColor() const
{
	return GenericGraphColors::NodeBody::Default;
}

EVisibility SEdNode_GenericGraphNode::GetDragOverMarkerVisibility() const
{
	return EVisibility::Visible;
}

const FSlateBrush* SEdNode_GenericGraphNode::GetNameIcon() const
{
	return FAppStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Icon"));
}

const TArray<TSharedPtr<FText>>* SEdNode_GenericGraphNode::GetComponentItems()
{
	UEdNode_GenericGraphNode* MyNode = CastChecked<UEdNode_GenericGraphNode>(GraphNode);
	if (MyNode != nullptr && MyNode->GenericGraphNode != nullptr)
	{
		TArray<UGenericGraphNodeComponent*> Components = MyNode->GenericGraphNode->OwnedComponents;
		for (UGenericGraphNodeComponent* Component : Components)
		{
			FString StringName;
			Component->GetName(StringName);
			ListItems.Add(MakeShared<FText>(FText::FromString(StringName)));
		}
	}

	return &ListItems;
}

#undef LOCTEXT_NAMESPACE
