// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "GenericSubobjectEditor/SGenericSubobjectEditor.h"
#include "GenericSubobjectEditor/GenericGraphNodeSEEC.h"
#include "GenericSubobjectEditor/GenericSubobjectData.h"
#include "GenericSubobjectEditor/GenericSubobjectDataHandle.h"
#include "GenericSubobjectEditor/GenericSubobjectDataSubsystem.h"
#include "GenericGraphAssetEditor/SGenericNodeComponentClassCombo.h"
#include "GenericGraphNodeComponent.h"
#include "GenericGraphNode.h"

#include "GenericSubobjectEditor/GenericComponentAssetBroker.h"
#include "GenericSubobjectEditor/GenericSubobjectDataHandle.h"

#include "IDocumentation.h"
#include "TutorialMetaData.h"
#include "GraphEditorActions.h"
#include "Selection.h"
#include "ClassViewerFilter.h"

#include "SPositiveActionButton.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Kismet2/ChildActorComponentEditorUtils.h"
#include "Subsystems/PanelExtensionSubsystem.h"	// SExtensionPanel

#include "Editor/UnrealEdEngine.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "SubobjectEditorExtensionContext.h"
#include "Dialogs/Dialogs.h"					// FSuppressableWarningDialog
#include "ObjectTools.h"						// ThumbnailTools::CacheEmptyThumbnail

#include "Kismet2/ComponentEditorUtils.h"
#include "ToolMenus.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"

#include "GameProjectGenerationModule.h"
#include "AddToProjectConfig.h"
#include "FeaturedClasses.inl"
// #include "K2Node_ComponentBoundEvent.h"
#include "BlueprintEditor.h"
#include "Preferences/UnrealEdOptions.h"

#include "ClassIconFinder.h"

#define LOCTEXT_NAMESPACE "SGenericSubobjectEditor"

static const FName SubobjectTree_ColumnName_ComponentClass("ComponentClass");
static const FName SubobjectTree_ColumnName_Asset("Asset");
static const FName SubobjectTree_ColumnName_Mobility("Mobility");
static const FName SubobjectTree_ContextMenuName("Kismet.SubobjectEditorContextMenu");

extern UNREALED_API UUnrealEdEngine* GUnrealEd;

void SGenericSubobject_RowWidget::Construct(const FArguments& InArgs, TWeakPtr<SGenericSubobjectEditor> InEditor, FGenericSubobjectEditorTreeNodePtrType InNodePtr, TSharedPtr<STableViewBase> InOwnerTableView)
{
	check(InNodePtr.IsValid());
	SubobjectEditor = InEditor;
	SubobjectPtr = InNodePtr;

	bool bIsSeparator = InNodePtr->IsSeperator();

	FSuperRowType::FArguments Args = FSuperRowType::FArguments()
		.Style(bIsSeparator ?
			&FAppStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.NoHoverTableRow") :
			&FAppStyle::Get().GetWidgetStyle<FTableRowStyle>("SceneOutliner.TableViewRow")) //@todo create editor style for the SCS tree
		.Padding(FMargin(0.f, 4.f, 0.f, 4.f))
		.ShowSelection(!bIsSeparator)
		.OnDragDetected(this, &SGenericSubobject_RowWidget::HandleOnDragDetected)
		.OnDragEnter(this, &SGenericSubobject_RowWidget::HandleOnDragEnter)
		.OnDragLeave(this, &SGenericSubobject_RowWidget::HandleOnDragLeave)
		.OnCanAcceptDrop(this, &SGenericSubobject_RowWidget::HandleOnCanAcceptDrop)
		.OnAcceptDrop(this, &SGenericSubobject_RowWidget::HandleOnAcceptDrop);

	SMultiColumnTableRow<FGenericSubobjectEditorTreeNodePtrType>::Construct(Args, InOwnerTableView.ToSharedRef());
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
TSharedRef<SWidget> SGenericSubobject_RowWidget::GenerateWidgetForColumn(const FName& ColumnName)
{
	FGenericSubobjectEditorTreeNodePtrType Node = GetSubobjectPtr();
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

	FGenericSubobjectData* Data = Node->GetDataSource();

	if (ColumnName == SubobjectTree_ColumnName_ComponentClass)
	{
		InlineWidget =
			SNew(SInlineEditableTextBlock)
			.Text(this, &SGenericSubobject_RowWidget::GetNameLabel)
			.OnVerifyTextChanged(this, &SGenericSubobject_RowWidget::OnNameTextVerifyChanged)
			.OnTextCommitted(this, &SGenericSubobject_RowWidget::OnNameTextCommit)
			.IsSelected(this, &SGenericSubobject_RowWidget::IsSelectedExclusively)
			.IsReadOnly(this, &SGenericSubobject_RowWidget::IsReadOnly);

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
					.ColorAndOpacity(this, &SGenericSubobject_RowWidget::GetColorTintForIcon)
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
					.Text(this, &SGenericSubobject_RowWidget::GetObjectContextText)
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
					.Visibility(this, &SGenericSubobject_RowWidget::GetAssetVisibility)
					.Text(this, &SGenericSubobject_RowWidget::GetAssetName)
					.ToolTipText(this, &SGenericSubobject_RowWidget::GetAssetPath)
			];
	}
	else if (ColumnName == SubobjectTree_ColumnName_Mobility)
	{
		if (Data && Data->GetComponentTemplate())
		{
			TSharedPtr<SToolTip> MobilityTooltip = SNew(SToolTip)
				.Text(this, &SGenericSubobject_RowWidget::GetMobilityToolTipText);

			return SNew(SHorizontalBox)
				.ToolTip(MobilityTooltip)
				.Visibility(EVisibility::Visible) // so we still get tooltip text for an empty SHorizontalBox
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.FillWidth(1.0f)
				[
					SNew(SImage)
						.Image(this, &SGenericSubobject_RowWidget::GetMobilityIconImage)
						.ToolTip(MobilityTooltip)
				];
		}
		else
		{
			return SNew(SSpacer);
		}
	}

	return SNew(STextBlock)
		.Text(LOCTEXT("UnknownColumn", "Unknown Column"));
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FText SGenericSubobject_RowWidget::GetNameLabel() const
{
	FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;
	if (Data)
	{
		// NOTE: Whatever this returns also becomes the variable name
		return FText::FromString(NodePtr->GetDisplayString());
	}
	return LOCTEXT("UnknownDataSource", "Unknown Name Label");
}

FText SGenericSubobject_RowWidget::GetAssetPath() const
{
	FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	FGenericSubobjectData* Data = NodePtr->GetDataSource();

	return Data ? Data->GetAssetPath() : FText::GetEmpty();
}

FText SGenericSubobject_RowWidget::GetAssetName() const
{
	FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	if (FGenericSubobjectData* Data = NodePtr->GetDataSource())
	{
		return Data->GetAssetName();

	}
	return FText::GetEmpty();
}

EVisibility SGenericSubobject_RowWidget::GetAssetVisibility() const
{
	FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;

	if (Data && Data->IsAssetVisible())
	{
		return EVisibility::Visible;
	}

	return EVisibility::Hidden;
}

const FSlateBrush* SGenericSubobject_RowWidget::GetIconBrush() const
{
	const FSlateBrush* ComponentIcon = FAppStyle::GetBrush("SCS.NativeComponent");

	if (FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr())
	{
		if (const FGenericSubobjectData* Data = NodePtr->GetDataSource())
		{
			if (const UGenericGraphNode* Actor = Data->GetObject<UGenericGraphNode>())
			{
				// return FClassIconFinder::FindIconForActor(Actor);
			}
			else if (const UGenericGraphNodeComponent* ComponentTemplate = Data->GetComponentTemplate())
			{
				ComponentIcon = FSlateIconFinder::FindIconBrushForClass(ComponentTemplate->GetClass(), TEXT("SCS.Component"));
			}
		}
	}

	return ComponentIcon;
}

FSlateColor SGenericSubobject_RowWidget::GetColorTintForIcon() const
{
	return FSlateColor(); // SubobjectEditor.Pin()->GetColorTintForIcon(GetSubobjectPtr());
}

ESelectionMode::Type SGenericSubobject_RowWidget::GetSelectionMode() const
{
	FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	if (NodePtr && NodePtr->IsSeperator())
	{
		return ESelectionMode::None;
	}

	return SMultiColumnTableRow<FGenericSubobjectEditorTreeNodePtrType>::GetSelectionMode();
}

void SGenericSubobject_RowWidget::HandleOnDragEnter(const FDragDropEvent& DragDropEvent)
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
	} */
}

void SGenericSubobject_RowWidget::HandleOnDragLeave(const FDragDropEvent& DragDropEvent)
{
	/* TSharedPtr<FSubobjectRowDragDropOp> DragRowOp = DragDropEvent.GetOperationAs<FSubobjectRowDragDropOp>();
	if (DragRowOp.IsValid())
	{
		bool bCanReparentAllNodes = true;
		for (auto SourceNodeIter = DragRowOp->SourceNodes.CreateConstIterator(); SourceNodeIter && bCanReparentAllNodes; ++SourceNodeIter)
		{
			FGenericSubobjectEditorTreeNodePtrType DraggedNodePtr = *SourceNodeIter;
			check(DraggedNodePtr.IsValid());
			const FGenericSubobjectData* Data = DraggedNodePtr->GetDataSource();

			bCanReparentAllNodes = Data->CanReparent();
		}

		// Only clear the tooltip text if all dragged nodes support it
		if (bCanReparentAllNodes)
		{
			TSharedPtr<SWidget> NoWidget;
			DragRowOp->SetFeedbackMessage(NoWidget);
			DragRowOp->PendingDropAction = FSubobjectRowDragDropOp::DropAction_None;
		}
	} */
}

FReply SGenericSubobject_RowWidget::HandleOnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	TSharedPtr<SGenericSubobjectEditor> SubobjectEditorPtr = SubobjectEditor.Pin();
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)
		&& SubobjectEditorPtr.IsValid()
		&& SubobjectEditorPtr->IsEditingAllowed()) //can only drag when editing
	{
		TArray<FGenericSubobjectEditorTreeNodePtrType> SelectedNodePtrs = SubobjectEditorPtr->GetSelectedNodes();
		if (SelectedNodePtrs.Num() == 0)
		{
			SelectedNodePtrs.Add(GetSubobjectPtr());
		}

		FGenericSubobjectEditorTreeNodePtrType FirstNode = SelectedNodePtrs[0];
		const FGenericSubobjectData* Data = FirstNode->GetDataSource();

		/* if (Data && FirstNode->IsComponentNode())
		{
			// Do not use the Blueprint from FirstNode, it may still be referencing the parent.
			UBlueprint* Blueprint = SubobjectEditorPtr->GetBlueprint();
			const FName VariableName = Data->GetVariableName();
			UStruct* VariableScope = (Blueprint != nullptr) ? Blueprint->SkeletonGeneratedClass : nullptr;

			TSharedRef<FSubobjectRowDragDropOp> Operation = FSubobjectRowDragDropOp::New(VariableName, VariableScope, FNodeCreationAnalytic());
			Operation->SetCtrlDrag(true); // Always put a getter
			Operation->PendingDropAction = FSubobjectRowDragDropOp::DropAction_None;
			Operation->SourceNodes = SelectedNodePtrs;

			return FReply::Handled().BeginDragDrop(Operation);
		} */
	}

	return FReply::Unhandled();
}

TOptional<EItemDropZone> SGenericSubobject_RowWidget::HandleOnCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, FGenericSubobjectEditorTreeNodePtrType TargetItem)
{
	TOptional<EItemDropZone> ReturnDropZone;

	/* TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();
	if (Operation.IsValid())
	{
		if (Operation->IsOfType<FSubobjectRowDragDropOp>() && (Cast<USceneComponent>(GetSubobjectPtr()->GetComponentTemplate()) != nullptr))
		{
			TSharedPtr<FSubobjectRowDragDropOp> DragRowOp = StaticCastSharedPtr<FSubobjectRowDragDropOp>(Operation);
			check(DragRowOp.IsValid());

			if (DragRowOp->PendingDropAction != FSubobjectRowDragDropOp::DropAction_None)
			{
				ReturnDropZone = EItemDropZone::OntoItem;
			}
		}
		else if (Operation->IsOfType<FExternalDragOperation>() || Operation->IsOfType<FAssetDragDropOp>())
		{
			ReturnDropZone = EItemDropZone::OntoItem;
		}
	} */

	return ReturnDropZone;
}

FReply SGenericSubobject_RowWidget::HandleOnAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, FGenericSubobjectEditorTreeNodePtrType TargetItem)
{
	/* TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();
	if (!Operation.IsValid())
	{
		return FReply::Handled();
	}

	TSharedPtr<SGenericSubobjectEditor> SubobjectEditorPtr = SubobjectEditor.Pin();
	if (!SubobjectEditorPtr.IsValid())
	{
		return FReply::Handled();
	}

	if (Operation->IsOfType<FSubobjectRowDragDropOp>() && (Cast<USceneComponent>(GetSubobjectPtr()->GetComponentTemplate()) != nullptr))
	{
		TSharedPtr<FSubobjectRowDragDropOp> DragRowOp = StaticCastSharedPtr<FSubobjectRowDragDropOp>(Operation);
		check(DragRowOp.IsValid());

		switch (DragRowOp->PendingDropAction)
		{
		case FSubobjectRowDragDropOp::DropAction_AttachTo:
			SubobjectEditorPtr->OnAttachToDropAction(GetSubobjectPtr(), DragRowOp->SourceNodes);
			break;

		case FSubobjectRowDragDropOp::DropAction_DetachFrom:
			SubobjectEditorPtr->OnDetachFromDropAction(DragRowOp->SourceNodes);
			break;

		case FSubobjectRowDragDropOp::DropAction_MakeNewRoot:
			check(DragRowOp->SourceNodes.Num() == 1);
			SubobjectEditorPtr->OnMakeNewRootDropAction(DragRowOp->SourceNodes[0]);
			break;

		case FSubobjectRowDragDropOp::DropAction_AttachToOrMakeNewRoot:
		{
			check(DragRowOp->SourceNodes.Num() == 1);
			FWidgetPath WidgetPath = DragDropEvent.GetEventPath() != nullptr ? *DragDropEvent.GetEventPath() : FWidgetPath();
			FSlateApplication::Get().PushMenu(
				SharedThis(this),
				WidgetPath,
				SubobjectEditorPtr->BuildSceneRootDropActionMenu(GetSubobjectPtr(), DragRowOp->SourceNodes[0]).ToSharedRef(),
				FSlateApplication::Get().GetCursorPos(),
				FPopupTransitionEffect(FPopupTransitionEffect::TypeInPopup)
			);
		}
		break;

		case FSubobjectRowDragDropOp::DropAction_None:
		default:
			break;
		}
	}
	else if (Operation->IsOfType<FExternalDragOperation>() || Operation->IsOfType<FAssetDragDropOp>())
	{
		// defer to the tree widget's handler for this type of operation
		TSharedPtr<SGenericSubobjectEditor> PinnedEditor = SubobjectEditor.Pin();
		TSharedPtr<SGenericSubobjectEditorDragDropTree> TreeWidget = PinnedEditor.IsValid() ? PinnedEditor->GetDragDropTree() : nullptr;
		if (TreeWidget.IsValid())
		{
			// The widget geometry is irrelevant to the tree widget's OnDrop
			PinnedEditor->GetDragDropTree()->OnDrop(FGeometry(), DragDropEvent);
		}
	} */

	return FReply::Handled();
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SGenericSubobject_RowWidget::AddToToolTipInfoBox(const TSharedRef<SVerticalBox>& InfoBox, const FText& Key, TSharedRef<SWidget> ValueIcon, const TAttribute<FText>& Value, bool bImportant)
{
	InfoBox->AddSlot()
		.AutoHeight()
		.Padding(0, 1)
		[
			SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0, 0, 4, 0)
				[
					SNew(STextBlock)
						.TextStyle(FAppStyle::Get(),
							bImportant ? "SCSEditor.ComponentTooltip.ImportantLabel" : "SCSEditor.ComponentTooltip.Label")
						.Text(FText::Format(LOCTEXT("AssetViewTooltipFormat", "{0}:"), Key))
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					ValueIcon
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
						.TextStyle(FAppStyle::Get(),
							bImportant ? "SCSEditor.ComponentTooltip.ImportantValue" : "SCSEditor.ComponentTooltip.Value")
						.Text(Value)
				]
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

TSharedRef<SToolTip> SGenericSubobject_RowWidget::CreateToolTipWidget() const
{
	FGenericSubobjectEditorTreeNodePtrType TreeNode = GetSubobjectPtr();
	check(TreeNode);
	const FGenericSubobjectData* TreeNodeData = TreeNode->GetDataSource();
	check(TreeNodeData);

	if (TreeNode->IsComponentNode())
	{
		return CreateComponentTooltipWidget(TreeNode);
	}
	else
	{
		return CreateActorTooltipWidget(TreeNode);
	}
}

TSharedRef<SToolTip> SGenericSubobject_RowWidget::CreateComponentTooltipWidget(const FGenericSubobjectEditorTreeNodePtrType& InNode) const
{
	// Create a box to hold every line of info in the body of the tooltip
	TSharedRef<SVerticalBox> InfoBox = SNew(SVerticalBox);

	if (FGenericSubobjectEditorTreeNodePtrType TreeNode = GetSubobjectPtr())
	{
		const FGenericSubobjectData* TreeNodeData = TreeNode->GetDataSource();
		check(TreeNodeData);
		if (TreeNode->IsComponentNode())
		{
			if (const UGenericGraphNodeComponent* Template = Cast<UGenericGraphNodeComponent>(TreeNode->GetObject()))
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
			}

			// Add introduction point
			AddToToolTipInfoBox(InfoBox, LOCTEXT("TooltipAddType", "Source"), SNullWidget::NullWidget, TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SGenericSubobject_RowWidget::GetComponentAddSourceToolTipText)), false);
			if (TreeNodeData->IsInheritedComponent())
			{
				AddToToolTipInfoBox(InfoBox, LOCTEXT("TooltipIntroducedIn", "Introduced in"), SNullWidget::NullWidget, TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SGenericSubobject_RowWidget::GetIntroducedInToolTipText)), false);
			}

			// Add Underlying Component Name for Native Components
			if (TreeNodeData->IsNativeComponent())
			{
				AddToToolTipInfoBox(InfoBox, LOCTEXT("TooltipNativeComponentName", "Native Component Name"), SNullWidget::NullWidget, TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SGenericSubobject_RowWidget::GetNativeComponentNameToolTipText)), false);
			}

			// Add mobility
			TSharedRef<SImage> MobilityIcon = SNew(SImage).Image(this, &SGenericSubobject_RowWidget::GetMobilityIconImage);
			AddToToolTipInfoBox(InfoBox, LOCTEXT("TooltipMobility", "Mobility"), MobilityIcon, TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SGenericSubobject_RowWidget::GetMobilityToolTipText)), false);

			// Add asset if applicable to this node
			if (GetAssetVisibility() == EVisibility::Visible)
			{
				InfoBox->AddSlot()[SNew(SSpacer).Size(FVector2D(1.0f, 8.0f))];
				AddToToolTipInfoBox(InfoBox, LOCTEXT("TooltipAsset", "Asset"), SNullWidget::NullWidget,
					TAttribute<FText>(this, &SGenericSubobject_RowWidget::GetAssetName), false);
			}

			// If the component is marked as editor only, then display that info here
			AddToToolTipInfoBox(InfoBox, LOCTEXT("TooltipEditorOnly", "Editor Only"), SNullWidget::NullWidget, TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SGenericSubobject_RowWidget::GetComponentEditorOnlyTooltipText)), false);
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
										.Text(this, &SGenericSubobject_RowWidget::GetTooltipText)
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

	return IDocumentation::Get()->CreateToolTip(TAttribute<FText>(this, &SGenericSubobject_RowWidget::GetTooltipText),
		TooltipContent, InfoBox, GetDocumentationLink(),
		GetDocumentationExcerptName());
}

TSharedRef<SToolTip> SGenericSubobject_RowWidget::CreateActorTooltipWidget(const FGenericSubobjectEditorTreeNodePtrType& InNode) const
{
	// Create a box to hold every line of info in the body of the tooltip
	TSharedRef<SVerticalBox> InfoBox = SNew(SVerticalBox);

	// Add class
	AddToToolTipInfoBox(InfoBox, LOCTEXT("TooltipClass", "Class"), SNullWidget::NullWidget, TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SGenericSubobject_RowWidget::GetActorClassNameText)), false);

	// Add super class
	AddToToolTipInfoBox(InfoBox, LOCTEXT("TooltipSuperClass", "Parent Class"), SNullWidget::NullWidget, TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SGenericSubobject_RowWidget::GetActorSuperClassNameText)), false);

	// Add mobility
	AddToToolTipInfoBox(InfoBox, LOCTEXT("TooltipMobility", "Mobility"), SNullWidget::NullWidget, TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &SGenericSubobject_RowWidget::GetActorMobilityText)), false);

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
								.Padding(4)
								[
									SNew(STextBlock)
										.TextStyle(FAppStyle::Get(), "SCSEditor.ComponentTooltip.Title")
										.Text(this, &SGenericSubobject_RowWidget::GetActorDisplayText)
								]
						]
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
						.BorderImage(FAppStyle::GetBrush("NoBorder"))
						.Padding(4)
						[
							InfoBox
						]
				]
		];

	return IDocumentation::Get()->CreateToolTip(TAttribute<FText>(this, &SGenericSubobject_RowWidget::GetActorDisplayText), TooltipContent, InfoBox, TEXT(""), TEXT(""));
}

FText SGenericSubobject_RowWidget::GetTooltipText() const
{
	const FGenericSubobjectData* Data = GetSubobjectPtr()->GetDataSource();

	if (!Data)
	{
		return FText::GetEmpty();
	}

	if (Data->IsDefaultSceneRoot())
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
	}
	else
	{
		const UClass* Class = (Data->GetComponentTemplate() != nullptr)
			? Data->GetComponentTemplate()->GetClass()
			: nullptr;
		const FText ClassDisplayName = FBlueprintEditorUtils::GetFriendlyClassDisplayName(Class);

		FFormatNamedArguments Args;
		Args.Add(TEXT("ClassName"), ClassDisplayName);
		Args.Add(TEXT("NodeName"), FText::FromString(Data->GetDisplayString()));

		return FText::Format(LOCTEXT("ComponentTooltip", "{NodeName} ({ClassName})"), Args);
	}
}

FText SGenericSubobject_RowWidget::GetActorClassNameText() const
{
	FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	const FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;
	if (Data)
	{
		if (const UGenericGraphNode* DefaultActor = Data->GetObject<UGenericGraphNode>())
		{
			return FText::FromString(DefaultActor->GetClass()->GetName());
		}
	}

	return FText::GetEmpty();
}

FText SGenericSubobject_RowWidget::GetActorSuperClassNameText() const
{
	FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	const FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;
	if (Data)
	{
		if (const UGenericGraphNode* DefaultActor = Data->GetObject<UGenericGraphNode>())
		{
			return FText::FromString(DefaultActor->GetClass()->GetSuperClass()->GetName());
		}
	}

	return FText::GetEmpty();
}

FText SGenericSubobject_RowWidget::GetActorMobilityText() const
{
	FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	const FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;
	if (Data)
	{
		if (const UGenericGraphNode* DefaultActor = Data->GetObject<UGenericGraphNode>())
		{
			// 
		}
	}

	return FText::GetEmpty();
}

TSharedRef<SWidget> SGenericSubobject_RowWidget::GetInheritedLinkWidget()
{
	FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	const FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;
	if (!Data)
	{
		return SNullWidget::NullWidget;
	}

	// Native components are inherited and have a gray hyperlink to their C++ class
	/* if (Data->IsNativeComponent())
	{
		static const FText NativeCppLabel = LOCTEXT("NativeCppInheritedLabel", "Edit in C++");

		return SNew(SHyperlink)
			.Style(FAppStyle::Get(), "Common.GotoNativeCodeHyperlink")
			.OnNavigate(this, &SGenericSubobject_RowWidget::OnEditNativeCppClicked)
			.Text(NativeCppLabel)
			.ToolTipText(FText::Format(LOCTEXT("GoToCode_ToolTip", "Click to open this source file in {0}"), FSourceCodeNavigation::GetSelectedSourceCodeIDE()))
			.Visibility(this, &SGenericSubobject_RowWidget::GetEditNativeCppVisibility);
	}
	// If the subobject is inherited and not native then it must be from a blueprint
	else if (Data->IsInstancedInheritedComponent() || Data->IsBlueprintInheritedComponent())
	{
		if (const UBlueprint* BP = Data->GetBlueprint())
		{
			static const FText InheritedBPLabel = LOCTEXT("InheritedBpLabel", "Edit in Blueprint");
			return SNew(SHyperlink)
				.Style(FAppStyle::Get(), "Common.GotoBlueprintHyperlink")
				.OnNavigate(this, &SGenericSubobject_RowWidget::OnEditBlueprintClicked)
				.Text(InheritedBPLabel)
				.ToolTipText(LOCTEXT("EditBlueprint_ToolTip", "Click to edit the blueprint"))
				.Visibility(this, &SGenericSubobject_RowWidget::GetEditBlueprintVisibility);
		}
	} */

	// Non-inherited subobjects shouldn't show anything! 
	return SNullWidget::NullWidget;
}

FText SGenericSubobject_RowWidget::GetObjectContextText() const
{
	FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	const FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;

	// We want to display (Self) or (Instance) only if the data is an actor
	if (Data)
	{
		// return Data->GetDisplayNameContextModifiers(GetDefault<UEditorStyleSettings>()->bShowNativeComponentNames);
	}
	return FText::GetEmpty();
}

void SGenericSubobject_RowWidget::OnEditBlueprintClicked()
{
	FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	const FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;
	if (!Data)
	{
		return;
	}
}

EVisibility SGenericSubobject_RowWidget::GetEditBlueprintVisibility() const
{
	FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	const FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;

	if (!Data)
	{
		return EVisibility::Collapsed;
	}

	return EVisibility::Collapsed;
}

EVisibility SGenericSubobject_RowWidget::GetEditNativeCppVisibility() const
{
	return ensure(GUnrealEd) && GUnrealEd->GetUnrealEdOptions()->IsCPPAllowed() ? EVisibility::Visible : EVisibility::Collapsed;
}

void SGenericSubobject_RowWidget::OnEditNativeCppClicked()
{
	FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	const FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;
	if (!Data)
	{
		return;
	}

	const FName VariableName = Data->GetVariableName();
	const UClass* ParentClass = nullptr;
	if (const FGenericSubobjectData* ParentData = Data->GetRootSubobject().GetData())
	{
		const UObject* ParentObj = ParentData->GetObject();
		ParentClass = ParentObj ? ParentObj->GetClass() : nullptr;
	}

	const FProperty* VariableProperty = ParentClass ? FindFProperty<FProperty>(ParentClass, VariableName) : nullptr;
	// FSourceCodeNavigation::NavigateToProperty(VariableProperty);
}

FString SGenericSubobject_RowWidget::GetDocumentationLink() const
{
	FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	const FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;

	/* if (Data && (NodePtr == SubobjectEditor.Pin()->GetSceneRootNode() || Data->IsInheritedComponent()))
	{
		return TEXT("Shared/Editors/BlueprintEditor/ComponentsMode");
	} */

	return TEXT("");
}

FString SGenericSubobject_RowWidget::GetDocumentationExcerptName() const
{
	FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	const FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;

	if (NodePtr && Data)
	{
		if (Data->IsNativeComponent())
		{
			return TEXT("NativeComponents");
		}
		else if (Data->IsInheritedComponent())
		{
			return TEXT("InheritedComponents");
		}
	}

	return TEXT("");
}

FText SGenericSubobject_RowWidget::GetMobilityToolTipText() const
{
	const FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	const FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;

	if (Data)
	{
		return Data->GetMobilityToolTipText();
	}

	return LOCTEXT("ErrorNoMobilityTooltip", "Invalid component");
}

FSlateBrush const* SGenericSubobject_RowWidget::GetMobilityIconImage() const
{
	//if (FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr())
	//{
	//	if (FGenericSubobjectData* Data = NodePtr->GetDataSource())
	//	{
	//		if (const USceneComponent* SceneComponentTemplate = Cast<USceneComponent>(Data->GetComponentTemplate()))
	//		{
	//			if (SceneComponentTemplate->Mobility == EComponentMobility::Movable)
	//			{
	//				return FAppStyle::GetBrush(TEXT("ClassIcon.MovableMobilityIcon"));
	//			}
	//			else if (SceneComponentTemplate->Mobility == EComponentMobility::Stationary)
	//			{
	//				return FAppStyle::GetBrush(TEXT("ClassIcon.StationaryMobilityIcon"));
	//			}
	//			// static components don't get an icon (because static is the most common
	//			// mobility type, and we'd like to keep the icon clutter to a minimum)
	//		}
	//	}
	//}

	return nullptr;
}

FText SGenericSubobject_RowWidget::GetIntroducedInToolTipText() const
{
	FText IntroducedInTooltip = LOCTEXT("IntroducedInThisBPTooltip", "this class");
	if (const FGenericSubobjectData* TreeNode = SubobjectPtr->GetDataSource())
	{
		return TreeNode->GetIntroducedInToolTipText();
	}

	return IntroducedInTooltip;
}

FText SGenericSubobject_RowWidget::GetComponentAddSourceToolTipText() const
{
	FText NodeType;

	if (const FGenericSubobjectData* TreeNode = SubobjectPtr->GetDataSource())
	{
		if (TreeNode->IsInheritedComponent())
		{
			if (TreeNode->IsNativeComponent())
			{
				NodeType = LOCTEXT("InheritedNativeComponent", "Inherited (C++)");
			}
			else
			{
				NodeType = LOCTEXT("InheritedBlueprintComponent", "Inherited (Blueprint)");
			}
		}
		else
		{
			if (TreeNode->IsInstancedComponent())
			{
				NodeType = LOCTEXT("ThisInstanceAddedComponent", "This actor instance");
			}
			else
			{
				NodeType = LOCTEXT("ThisBlueprintAddedComponent", "This Blueprint");
			}
		}
	}

	return NodeType;
}

FText SGenericSubobject_RowWidget::GetComponentEditorOnlyTooltipText() const
{
	const FGenericSubobjectData* TreeNode = SubobjectPtr->GetDataSource();
	return TreeNode ? TreeNode->GetComponentEditorOnlyTooltipText() : LOCTEXT("ComponentEditorOnlyFalse", "False");
}

FText SGenericSubobject_RowWidget::GetNativeComponentNameToolTipText() const
{
	const FGenericSubobjectData* TreeNode = SubobjectPtr->GetDataSource();
	const UGenericGraphNodeComponent* Template = TreeNode ? TreeNode->GetComponentTemplate() : nullptr;

	if (Template)
	{
		return FText::FromName(Template->GetFName());
	}
	else
	{
		return FText::GetEmpty();
	}
}

FText SGenericSubobject_RowWidget::GetActorDisplayText() const
{
	const FGenericSubobjectData* TreeNode = SubobjectPtr->GetDataSource();

	if (TreeNode)
	{
		return TreeNode->GetActorDisplayText();
	}

	return FText::GetEmpty();
}

void SGenericSubobject_RowWidget::OnNameTextCommit(const FText& InNewName, ETextCommit::Type InTextCommit)
{
	// Ask the subsystem to rename this
	FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
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
		USubobjectDataSubsystem* System = USubobjectDataSubsystem::Get();
		check(System);
		// const bool bSuccess = System->RenameSubobject(NodePtr->GetDataHandle(), InNewName);

		Transaction.Reset();
	}
}

bool SGenericSubobject_RowWidget::OnNameTextVerifyChanged(const FText& InNewText, FText& OutErrorMessage)
{
	const FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	if (ensure(NodePtr))
	{
		// Ask the subsystem to verify this rename option for us
		const USubobjectDataSubsystem* System = USubobjectDataSubsystem::Get();
		check(System);
		// return System->IsValidRename(NodePtr->GetDataHandle(), InNewText, OutErrorMessage);
	}

	return false;
}

bool SGenericSubobject_RowWidget::IsReadOnly() const
{
	FGenericSubobjectEditorTreeNodePtrType NodePtr = GetSubobjectPtr();
	const FGenericSubobjectData* Data = NodePtr ? NodePtr->GetDataSource() : nullptr;
	if (Data)
	{
		return !Data->CanRename() || (SubobjectEditor.IsValid() && !SubobjectEditor.Pin()->IsEditingAllowed());
	}

	// If there is no valid data then default to read only
	return true;
}

////////////////////////////////////////////////
// SGenericSubobjectEditor

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SGenericSubobjectEditor::Construct(const FArguments& InArgs)
{
	ObjectContext = InArgs._ObjectContext;
	OnSelectionUpdated = InArgs._OnSelectionUpdated;
	OnItemDoubleClicked = InArgs._OnItemDoubleClicked;
	AllowEditing = InArgs._AllowEditing;
	HideComponentClassCombo = InArgs._HideComponentClassCombo;
	bAllowTreeUpdates = true;

	CreateCommandList();

	// Build the tree widget
	FSlateBrush const* MobilityHeaderBrush = FAppStyle::GetBrush(TEXT("ClassIcon.ComponentMobilityHeaderIcon"));

	// Should only be true when used in the blueprints details panel
	const bool bInlineSearchBarWithButtons = ShowInlineSearchWithButtons();

	TSharedPtr<SWidget> Contents;

	TSharedPtr<SVerticalBox> HeaderBox;
	TSharedPtr<SWidget> SearchBar = SAssignNew(FilterBox, SSearchBox)
		.HintText(!bInlineSearchBarWithButtons ? LOCTEXT("SearchComponentsHint", "Search Components") : LOCTEXT("SearchHint", "Search"))
		.OnTextChanged(this, &SGenericSubobjectEditor::OnFilterTextChanged)
		.Visibility(this, &SGenericSubobjectEditor::GetComponentsFilterBoxVisibility);

	FMenuBuilder EditBlueprintMenuBuilder = CreateMenuBuilder();

	// USubobjectEditorExtensionContext* ExtensionContext = NewObject<UGenericGraphNodeSEEC>();
	// ExtensionContext->SubobjectEditor = SharedThis(this);
	// ExtensionContext->AddToRoot();

	ButtonBox = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(0.0f, 0.0f, 4.0f, 0.0f)
		.AutoWidth()
		[
			SNew(SGenericNodeComponentClassCombo)
				.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("GenericGraphNode.AddComponent")))
				.Visibility(this, &SGenericSubobjectEditor::GetComponentClassComboButtonVisibility)
				.OnSubobjectClassSelected(this, &SGenericSubobjectEditor::PerformComboAddClass)
				.ToolTipText(LOCTEXT("AddComponent_Tooltip", "Adds a new component to this generic graph node"))
				.IsEnabled(true)
				.CustomClassFilters(InArgs._SubobjectClassListFilters)
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(0.0f, 0.0f, 4.0f, 0.0f)
		.AutoWidth()
		[
			// Search Panel Context Extension
			SAssignNew(ExtensionPanel, SExtensionPanel)
				.ExtensionPanelID("SCSEditor.NextToAddComponentButton")
				// .ExtensionContext(ExtensionContext)
		]

		// horizontal slot index #2 => reserved for BP-editor search bar (see 'ButtonBox' and 'SearchBarHorizontalSlotIndex' usage below)

		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(0.0f, 0.0f, 4.0f, 0.0f)
		.AutoWidth()
		[
			SNew(SPositiveActionButton)
				.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("Actor.ConvertToBlueprint")))
				// .Visibility(this, &SGenericSubobjectEditor::GetPromoteToBlueprintButtonVisibility)
				// .OnClicked(this, &SGenericSubobjectEditor::OnPromoteToBlueprintClicked)
				.Icon(FAppStyle::Get().GetBrush("Icons.Blueprints"))
				.ToolTip(IDocumentation::Get()->CreateToolTip(
					TAttribute<FText>(LOCTEXT("PromoteToBluerprintTooltip", "Converts this actor into a reusable Blueprint Class that can have script behavior")),
					nullptr,
					TEXT("Shared/LevelEditor"),
					TEXT("ConvertToBlueprint")))
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SNew(SPositiveActionButton)
				.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("Actor.EditBlueprint")))
				// .Visibility(this, &SGenericSubobjectEditor::GetEditBlueprintButtonVisibility)
				.ToolTipText(LOCTEXT("EditActorBlueprint_Tooltip", "Edit the Blueprint for this Actor"))
				.Icon(FAppStyle::Get().GetBrush("Icons.Blueprints"))
				.MenuContent()
				[
					EditBlueprintMenuBuilder.MakeWidget()
				]
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
				.Visibility(this, &SGenericSubobjectEditor::GetComponentsTreeVisibility)
				[
					TreeWidget.ToSharedRef()
				]
		];

	// Only insert the buttons and search bar in the Blueprints version
	if (bInlineSearchBarWithButtons) // Blueprints
	{
		ButtonBox->AddSlot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			.Padding(3.0f, 3.0f)
			[
				SearchBar.ToSharedRef()
			];

		HeaderBox->AddSlot()
			.VAlign(VAlign_Center)
			.AutoHeight()
			[
				ButtonBox.ToSharedRef()
			];
	}

	this->ChildSlot
		[
			Contents.ToSharedRef()
		];

	// Update tree method? 
	UpdateTree();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FGenericSubobjectDataHandle SGenericSubobjectEditor::PerformComboAddClass(TSubclassOf<UGenericGraphNodeComponent> ComponentClass, EComponentCreateAction::Type ComponentCreateAction, UObject* AssetOverride)
{
	FGenericSubobjectDataHandle NewComponentHandle = FGenericSubobjectDataHandle::InvalidHandle;

	// Display a class picked for which class to use for the new component
	UClass* NewClass = ComponentClass;

	// Display a class creation menu if the user wants to create a new class
	if (ComponentCreateAction == EComponentCreateAction::CreateNewCPPClass)
	{
		NewClass = SpawnCreateNewCppComponentWindow(ComponentClass);
	}
	else if (ComponentCreateAction == EComponentCreateAction::CreateNewBlueprintClass)
	{
		NewClass = SpawnCreateNewBPComponentWindow(ComponentClass);
	}

	UGenericSubobjectDataSubsystem* SubobjectSystem = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>();

	if (NewClass && SubobjectSystem)
	{
		TUniquePtr<FScopedTransaction> AddTransaction = MakeUnique<FScopedTransaction>(LOCTEXT("AddComponent", "Add Component"));

		FEditorDelegates::LoadSelectedAssetsIfNeeded.Broadcast();
		USelection* Selection = GEditor->GetSelectedObjects();

		bool bAddedComponent = false;
		FText OutFailReason;

		if (!bAddedComponent)
		{
			// Attach this component to the override asset first, but if none is given then use the actor context			
			FGenericSubobjectDataHandle ParentHandle = SubobjectSystem->FindHandleForObject(CachedRootHandle, AssetOverride);

			if (!ParentHandle.IsValid())
			{
				// If we have something selected, then we should attach it to that
				TArray<FGenericSubobjectEditorTreeNodePtrType> SelectedTreeNodes = TreeWidget->GetSelectedItems();
				if (SelectedTreeNodes.Num() > 0)
				{
					ParentHandle = SelectedTreeNodes[0]->GetDataHandle();
				}
				// Otherwise fall back to the root node
				else
				{
					ParentHandle = RootNodes.Num() > 0
						? RootNodes[0]->GetDataHandle()
						: FGenericSubobjectDataHandle::InvalidHandle;
				}
			}

			if (ParentHandle.IsValid())
			{
				NewComponentHandle = AddNewSubobject(ParentHandle, NewClass, AssetOverride, OutFailReason, MoveTemp(AddTransaction));
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
			FGenericSubobjectEditorTreeNodePtrType NewNode = FindSlateNodeForHandle(NewComponentHandle);
			if (NewNode != nullptr)
			{
				TreeWidget->SetSelection(NewNode);
				OnRenameComponent(MoveTemp(AddTransaction));
			}
		}
	}

	return NewComponentHandle;
}

class FComponentClassParentFilter : public IClassViewerFilter
{
public:
	FComponentClassParentFilter(const TSubclassOf<UGenericGraphNodeComponent>& InComponentClass) : ComponentClass(InComponentClass) {}

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return InClass->IsChildOf(ComponentClass);
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return InUnloadedClassData->IsChildOf(ComponentClass);
	}

	TSubclassOf<UGenericGraphNodeComponent> ComponentClass;
};

typedef FComponentClassParentFilter FNativeComponentClassParentFilter;

FMenuBuilder SGenericSubobjectEditor::CreateMenuBuilder()
{
	// Menu builder for editing a blueprint
	FMenuBuilder EditBlueprintMenuBuilder(true, nullptr);

	EditBlueprintMenuBuilder.BeginSection(
		NAME_None, LOCTEXT("EditBlueprintMenu_ExistingBlueprintHeader", "Existing Blueprint"));

	/* EditBlueprintMenuBuilder.AddMenuEntry
	(
		LOCTEXT("OpenBlueprintEditor", "Open Blueprint Editor"),
		LOCTEXT("OpenBlueprintEditor_ToolTip", "Opens the blueprint editor for this asset"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateSP(this, &SGenericSubobjectEditor::OnOpenBlueprintEditor, /*bForceCodeEditing= false))
	);

	EditBlueprintMenuBuilder.AddMenuEntry
	(
		LOCTEXT("OpenBlueprintEditorScriptMode", "Add or Edit Script"),
		LOCTEXT("OpenBlueprintEditorScriptMode_ToolTip",
			"Opens the blueprint editor for this asset, showing the event graph"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateSP(this, &SGenericSubobjectEditor::OnOpenBlueprintEditor, /*bForceCodeEditing= true))
	); */

	return EditBlueprintMenuBuilder;
}

void SGenericSubobjectEditor::ConstructTreeWidget()
{
}

void SGenericSubobjectEditor::CreateCommandList()
{
}

void SGenericSubobjectEditor::DepthFirstTraversal(const FGenericSubobjectEditorTreeNodePtrType& InNodePtr, TSet<FGenericSubobjectEditorTreeNodePtrType>& OutVisitedNodes, const TFunctionRef<void(const FGenericSubobjectEditorTreeNodePtrType&)> InFunction) const
{
}

void SGenericSubobjectEditor::GetCollapsedNodes(const FGenericSubobjectEditorTreeNodePtrType& InNodePtr, TSet<FGenericSubobjectEditorTreeNodePtrType>& OutCollapsedNodes) const
{
}

FGenericSubobjectEditorTreeNodePtrType SGenericSubobjectEditor::FindOrCreateSlateNodeForHandle(const FGenericSubobjectDataHandle& Handle, TMap<FGenericSubobjectDataHandle, FGenericSubobjectEditorTreeNodePtrType>& ExistingNodes)
{
	return FGenericSubobjectEditorTreeNodePtrType();
}

UObject* SGenericSubobjectEditor::GetObjectContext() const
{
	return ObjectContext.Get();
}

UClass* SGenericSubobjectEditor::SpawnCreateNewCppComponentWindow(TSubclassOf<UGenericGraphNodeComponent> ComponentClass)
{
	TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(SharedThis(this));

	FString AddedClassName;
	auto OnCodeAddedToProject = [&AddedClassName](const FString& ClassName, const FString& ClassPath,
		const FString& ModuleName)
	{
		if (!ClassName.IsEmpty() && !ClassPath.IsEmpty())
		{
			AddedClassName = FString::Printf(TEXT("/Script/%s.%s"), *ModuleName, *ClassName);
		}
	};

	FGameProjectGenerationModule::Get().OpenAddCodeToProjectDialog(
		FAddToProjectConfig()
		.WindowTitle(LOCTEXT("AddNewC++Component", "Add C++ Component"))
		.ParentWindow(ParentWindow)
		.Modal()
		.OnAddedToProject(FOnAddedToProject::CreateLambda(OnCodeAddedToProject))
		.FeatureComponentClasses()
		.AllowableParents(MakeShareable(new FNativeComponentClassParentFilter(ComponentClass)))
		.DefaultClassPrefix(TEXT("New"))
	);


	return LoadClass<UGenericGraphNodeComponent>(nullptr, *AddedClassName, nullptr, LOAD_None, nullptr);
}

class FBlueprintComponentClassParentFilter : public FComponentClassParentFilter
{
public:
	FBlueprintComponentClassParentFilter(const TSubclassOf<UGenericGraphNodeComponent>& InComponentClass)
		: FComponentClassParentFilter(InComponentClass)
	{}

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
	{
		return
			FComponentClassParentFilter::IsClassAllowed(InInitOptions, InClass, InFilterFuncs) &&
			FKismetEditorUtilities::CanCreateBlueprintOfClass(InClass);
	}
};

UClass* SGenericSubobjectEditor::SpawnCreateNewBPComponentWindow(TSubclassOf<UGenericGraphNodeComponent> ComponentClass)
{
	UClass* NewClass = nullptr;

	auto OnAddedToProject = [&](const FString& ClassName, const FString& PackagePath, const FString& ModuleName)
	{
		if (!ClassName.IsEmpty() && !PackagePath.IsEmpty())
		{
			if (UPackage* Package = FindPackage(nullptr, *PackagePath))
			{
				if (UBlueprint* NewBP = FindObjectFast<UBlueprint>(Package, *ClassName))
				{
					NewClass = NewBP->GeneratedClass;

					TArray<UObject*> Objects;
					Objects.Emplace(NewBP);
					GEditor->SyncBrowserToObjects(Objects);

					// Open the editor for the new blueprint
					GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(NewBP);
				}
			}
		}
	};

	FGameProjectGenerationModule::Get().OpenAddBlueprintToProjectDialog(
		FAddToProjectConfig()
		.WindowTitle(LOCTEXT("AddNewBlueprintComponent", "Add Blueprint Component"))
		.ParentWindow(FSlateApplication::Get().FindWidgetWindow(SharedThis(this)))
		.Modal()
		.AllowableParents(MakeShareable(new FBlueprintComponentClassParentFilter(ComponentClass)))
		.FeatureComponentClasses()
		.OnAddedToProject(FOnAddedToProject::CreateLambda(OnAddedToProject))
		.DefaultClassPrefix(TEXT("New"))
	);

	return NewClass;
}

void SGenericSubobjectEditor::OnDeleteNodes()
{
	const FScopedTransaction Transaction(LOCTEXT("RemoveComponents", "Remove Components"));

	// Invalidate any active component in the visualizer
	GUnrealEd->ComponentVisManager.ClearActiveComponentVis();

	// A lamda for displaying a confirm message to the user if there is a dynamic delegate bound to the 
	// component they are trying to delete
	auto ConfirmDeleteLambda = [](const FGenericSubobjectData* Data) -> FSuppressableWarningDialog::EResult
	{
		if (ensure(Data))
		{
			FText VarNam = FText::FromName(Data->GetVariableName());
			FText ConfirmDelete = FText::Format(LOCTEXT("ConfirmDeleteDynamicDelegate", "Component \"{0}\" has bound events in use! If you delete it then those nodes will become invalid. Are you sure you want to delete it?"), VarNam);

			// Warn the user that this may result in data loss
			FSuppressableWarningDialog::FSetupInfo Info(ConfirmDelete, LOCTEXT("DeleteComponent", "Delete Component"), "DeleteComponentInUse_Warning");
			Info.ConfirmText = LOCTEXT("ConfirmDeleteDynamicDelegate_Yes", "Yes");
			Info.CancelText = LOCTEXT("ConfirmDeleteDynamicDelegate_No", "No");

			FSuppressableWarningDialog DeleteVariableInUse(Info);

			// If the user selects cancel then return false
			return DeleteVariableInUse.ShowModal();
		}

		return FSuppressableWarningDialog::Cancel;
	};

	ObjectContext.Get(nullptr)->GetClass()->ClassGeneratedBy;

	// Gather the handles of the components that we want to delete
	TArray<FGenericSubobjectDataHandle> HandlesToDelete;
	TArray<FGenericSubobjectEditorTreeNodePtrType> SelectedNodes = GetSelectedNodes();
	for (const FGenericSubobjectEditorTreeNodePtrType& Node : SelectedNodes)
	{
		check(Node->IsValid());

		if (const FGenericSubobjectData* Data = Node->GetDataSource())
		{
			FName PropertyName = Data->GetVariableName();
			if (PropertyName != NAME_None)
			{
				// FBlueprintEditorUtils::GetAllNodesOfClass(EventNodes)
				/* TArray<UK2Node_ComponentBoundEvent*> AllNodes;
				GraphContext.Get()->GetNodesOfClass<UK2Node_ComponentBoundEvent>(AllNodes);

				// FKismetEditorUtilities::FindAllBoundEventsForComponent(EventNodes)
				TArray<UK2Node_ComponentBoundEvent*> EventNodes;
				for (UK2Node_ComponentBoundEvent* CurNode : AllNodes)
				{
					if (CurNode && CurNode->ComponentPropertyName == PropertyName)
					{
						EventNodes.Add(CurNode);
					}
				}
				// If this node is in use by Dynamic delegates, then confirm before continuing
				// FKismetEditorUtilities::PropertyHasBoundEvents(Data->GetVariableName())
				if (EventNodes.Num() > 0)
				{
					// The user has decided not to delete the component, stop trying to delete this component
					if (ConfirmDeleteLambda(Data) == FSuppressableWarningDialog::Cancel)
					{
						return;
					}
				} */
				HandlesToDelete.Add(Data->GetHandle());
			}
		}
	}

	if (UGenericSubobjectDataSubsystem* System = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>())
	{
		FGenericSubobjectDataHandle HandleToSelect;

		int32 NumDeleted = System->DeleteSubobjects(RootNodes[0]->GetDataHandle(), HandlesToDelete, HandleToSelect);

		if (NumDeleted > 0)
		{
			if (HandleToSelect.IsValid())
			{
				FGenericSubobjectEditorTreeNodePtrType NodeToSelect = FindSlateNodeForHandle(HandleToSelect);
				if (NodeToSelect.IsValid())
				{
					TreeWidget->SetSelection(NodeToSelect);
				}
			}

			UpdateTree();

			// Do this AFTER marking the Blueprint as modified
			UpdateSelectionFromNodes(TreeWidget->GetSelectedItems());
		}
	}
}

void SGenericSubobjectEditor::CopySelectedNodes()
{
	TArray<FGenericSubobjectDataHandle> SelectedHandles = GetSelectedHandles();
	if (UGenericSubobjectDataSubsystem* System = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>())
	{
		return System->CopySubobjects(SelectedHandles);
	}
}

void SGenericSubobjectEditor::OnDuplicateComponent()
{
	TArray<FGenericSubobjectDataHandle> SelectedNodes = GetSelectedHandles();
	if (SelectedNodes.Num() > 0)
	{
		// Force the text box being edited (if any) to commit its text. The duplicate operation may trigger a regeneration of the tree view,
		// releasing all row widgets. If one row was in edit mode (rename/rename on create), it was released before losing the focus and
		// this would prevent the completion of the 'rename' or 'create + give initial name' transaction (occurring on focus lost).
		FSlateApplication::Get().ClearKeyboardFocus();

		TUniquePtr<FScopedTransaction> Transaction = MakeUnique<FScopedTransaction>(SelectedNodes.Num() > 1 ? LOCTEXT("DuplicateComponents", "Duplicate Components") : LOCTEXT("DuplicateComponent", "Duplicate Component"));

		if (UGenericSubobjectDataSubsystem* System = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>())
		{
			TArray<FGenericSubobjectDataHandle> DuplicatedHandles;
			System->DuplicateSubobjects(GetObjectContextHandle(), SelectedNodes, DuplicatedHandles);
			UpdateTree();

			// Set focus to the newly created subobject
			FGenericSubobjectEditorTreeNodePtrType NewNode = DuplicatedHandles.Num() > 0 ? FindSlateNodeForHandle(DuplicatedHandles[0]) : nullptr;
			if (NewNode != nullptr)
			{
				TreeWidget->SetSelection(NewNode);
				OnRenameComponent(MoveTemp(Transaction));
			}
		}
	}
}

bool SGenericSubobjectEditor::CanPasteNodes() const
{
	if (!IsEditingAllowed())
	{
		return false;
	}

	FGenericSubobjectDataHandle SceneRoot = GetObjectContextHandle();

	if (UGenericSubobjectDataSubsystem* System = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>())
	{
		return System->CanPasteSubobjects(SceneRoot);
	}

	return false;
}

void SGenericSubobjectEditor::PasteNodes()
{
	if (UGenericSubobjectDataSubsystem* System = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>())
	{
		TArray<FGenericSubobjectDataHandle> OutHandles;

		// stop allowing tree updates during paste, otherwise MarkBlueprintAsModified will trigger in the middle of it
		bool bRestoreAllowTreeUpdates = bAllowTreeUpdates;
		bAllowTreeUpdates = false;
		TArray<FGenericSubobjectDataHandle> HandlesToPasteOnto = GetSelectedHandles();
		if (HandlesToPasteOnto.IsEmpty())
		{
			FGenericSubobjectDataHandle RootPtr = GetObjectContextHandle();

			HandlesToPasteOnto.Emplace(RootPtr);
		}

		System->PasteSubobjects(GetObjectContextHandle(), HandlesToPasteOnto, OutHandles);

		// allow tree updates again
		bAllowTreeUpdates = bRestoreAllowTreeUpdates;

		if (OutHandles.Num() > 0)
		{
			// We only want the pasted node(s) to be selected
			TreeWidget->ClearSelection();
			UpdateTree();

			for (const FGenericSubobjectDataHandle& Handle : OutHandles)
			{
				if (FGenericSubobjectEditorTreeNodePtrType SlateNode = FindSlateNodeForHandle(Handle))
				{
					TreeWidget->RequestScrollIntoView(SlateNode);
					TreeWidget->SetItemSelection(SlateNode, true);
				}
			}
		}
	}
}

void SGenericSubobjectEditor::OnAttachToDropAction(FGenericSubobjectEditorTreeNodePtrType DroppedOn, const TArray<FGenericSubobjectEditorTreeNodePtrType>& DroppedNodePtrs)
{
	// Ask the subsystem to attach the dropped nodes onto the dropped on node
	check(DroppedOn.IsValid());
	check(DroppedNodePtrs.Num() > 0);

	UGenericSubobjectDataSubsystem* System = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>();
	check(System);

	const FScopedTransaction TransactionContext(DroppedNodePtrs.Num() > 1 ? LOCTEXT("AttachComponents", "Attach Components") : LOCTEXT("AttachComponent", "Attach Component"));
	TArray<FGenericSubobjectDataHandle> HandlesToMove;
	for (const FGenericSubobjectEditorTreeNodePtrType& DroppedNodePtr : DroppedNodePtrs)
	{
		HandlesToMove.Add(DroppedNodePtr->GetDataHandle());
	}

	FReparentSubobjectParams Params;
	// Params.NewParentHandle = DroppedOn->GetDataHandle();

	// These both do nothing if not an actor.
	// Params.BlueprintContext = GetBlueprint();
	// Params.ActorPreviewContext = GetActorPreview();

	// System->ReparentSubobjects(Params, HandlesToMove);

	check(TreeWidget.IsValid());
	TreeWidget->SetItemExpansion(DroppedOn, true);

	PostDragDropAction(true);
}

void SGenericSubobjectEditor::OnDetachFromDropAction(const TArray<FGenericSubobjectEditorTreeNodePtrType>& DroppedNodePtrs)
{
	check(DroppedNodePtrs.Num() > 0);

	const FScopedTransaction TransactionContext(DroppedNodePtrs.Num() > 1 ? LOCTEXT("DetachComponents", "Detach Components") : LOCTEXT("DetachComponent", "Detach Component"));

	TArray<FGenericSubobjectDataHandle> HandlesToMove;
	// Utils::PopulateHandlesArray(DroppedNodePtrs, HandlesToMove); <-- Function expanded below.
	for (const FGenericSubobjectEditorTreeNodePtrType& DroppedNodePtr : DroppedNodePtrs)
	{
		HandlesToMove.Add(DroppedNodePtr->GetDataHandle());
	}

	// Attach the dropped node to the current scene root node
	FGenericSubobjectDataHandle SceneRootNodePtr = GetObjectContextHandle();
	check(SceneRootNodePtr.IsValid());

	// Ask the subsystem to reparent this object to the scene root
	if (UGenericSubobjectDataSubsystem* System = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>())
	{
		FReparentSubobjectParams Params;
		// Params.NewParentHandle = SceneRootNodePtr;
		// Params.BlueprintContext = GetBlueprint();
	}

	PostDragDropAction(true);
}

void SGenericSubobjectEditor::PostDragDropAction(bool bRegenerateTreeNodes)
{
	GUnrealEd->ComponentVisManager.ClearActiveComponentVis();

	UpdateTree(bRegenerateTreeNodes);

	// FBlueprintEditorUtils::PostEditChangeBlueprintActors(GetBlueprint(), true);
}

FGenericSubobjectDataHandle SGenericSubobjectEditor::AddNewSubobject(const FGenericSubobjectDataHandle& ParentHandle, UClass* NewClass, UObject* AssetOverride, FText& OutFailReason, TUniquePtr<FScopedTransaction> InOngoingTransaction)
{
	FAddNewGenericSubobjectParams Params;
	// Params.ParentHandle = ParentHandle;
	Params.NewClass = NewClass;
	Params.AssetOverride = AssetOverride;

	UGenericSubobjectDataSubsystem* System = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>();
	check(System);

	return System->AddNewSubobject(Params, OutFailReason);
}

void SGenericSubobjectEditor::PopulateContextMenuImpl(UToolMenu* InMenu, TArray<FGenericSubobjectEditorTreeNodePtrType>& InSelectedItems, bool bIsChildActorSubtreeNodeSelected)
{
	FToolMenuSection& BlueprintSCSSection = InMenu->AddSection("BlueprintSCS");
	if (InSelectedItems.Num() == 1)
	{
		BlueprintSCSSection.AddSubMenu(
			FName("FindReferenceSubMenu"),
			LOCTEXT("FindReferences_Label", "Find References"),
			LOCTEXT("FindReferences_Tooltip", "Options for finding references to class members"),
			FNewToolMenuChoice(FNewMenuDelegate::CreateStatic(&FGraphEditorCommands::BuildFindReferencesMenu))
		);
	}

	// Create an "Add Event" option in the context menu only if we can edit
	// the currently selected objects
	if (IsEditingAllowed())
	{
		// Collect the classes of all selected objects
		TArray<UClass*> SelectionClasses;
		for (auto NodeIter = InSelectedItems.CreateConstIterator(); NodeIter; ++NodeIter)
		{
			FGenericSubobjectEditorTreeNodePtrType TreeNode = *NodeIter;
			const FGenericSubobjectData* Data = TreeNode->GetDataSource();
			check(Data);

			if (const UGenericGraphNodeComponent* ComponentTemplate = Cast<UGenericGraphNodeComponent>(TreeNode->GetObject()))
			{
				// If the component is native then we need to ensure it can actually be edited before we display it
				// if (!Data->IsNativeComponent() || FComponentEditorUtils::GetPropertyForEditableNativeComponent(ComponentTemplate))
				{
					SelectionClasses.Add(ComponentTemplate->GetClass());
				}
			}
		}

		if (SelectionClasses.Num())
		{
			// Find the common base class of all selected classes
			UClass* SelectedClass = UClass::FindCommonBase(SelectionClasses);
			// Build an event submenu if we can generate events
			if (FBlueprintEditorUtils::CanClassGenerateEvents(SelectedClass))
			{
				BlueprintSCSSection.AddSubMenu(
					"AddEventSubMenu",
					LOCTEXT("AddEventSubMenu", "Add Event"),
					LOCTEXT("ActtionsSubMenu_ToolTip", "Add Event"),
					FNewMenuDelegate::CreateStatic(&SGenericSubobjectEditor::BuildMenuEventsSection,
						SelectedClass, FCanExecuteAction::CreateSP(this, &SGenericSubobjectEditor::IsEditingAllowed),
						FGetSelectedObjectsDelegate::CreateSP(this, &SGenericSubobjectEditor::GetSelectedItemsForContextMenu)));
			}
		}
	}

	TArray<UGenericGraphNodeComponent*> SelectedComponents;
	for (const FGenericSubobjectEditorTreeNodePtrType& SelectedNodePtr : InSelectedItems)
	{
		check(SelectedNodePtr->IsValid());
		// Get the component template associated with the selected node
		const UGenericGraphNodeComponent* ComponentTemplate = Cast<UGenericGraphNodeComponent>(SelectedNodePtr->GetObject());
		if (ComponentTemplate)
		{
			// #TODO_BH Remove this const cast
			SelectedComponents.Add(const_cast<UGenericGraphNodeComponent*>(ComponentTemplate));
		}
	}

	// Common menu options added for all component types
	// FComponentEditorUtils::FillComponentContextMenuOptions(InMenu, SelectedComponents);

	// For a selection outside of a child actor subtree, we may choose to include additional options
	if (SelectedComponents.Num() == 1 && !bIsChildActorSubtreeNodeSelected)
	{
		// Extra options for a child actor component
		// if (UChildActorComponent* SelectedChildActorComponent = Cast<UChildActorComponent>(SelectedComponents[0]))
		{
			// These options will get added only in SCS mode
			// FChildActorComponentEditorUtils::FillComponentContextMenuOptions(InMenu, SelectedChildActorComponent);
		}
	}
}

void SGenericSubobjectEditor::GetSelectedItemsForContextMenu(TArray<FComponentEventConstructionData>& OutSelectedItems) const
{

}

void SGenericSubobjectEditor::BuildMenuEventsSection(FMenuBuilder& Menu, UClass* SelectedClass, FCanExecuteAction CanExecuteActionDelegate, FGetSelectedObjectsDelegate GetSelectedObjectsDelegate)
{
	// Get Selected Nodes
	TArray<FComponentEventConstructionData> SelectedNodes;
	GetSelectedObjectsDelegate.ExecuteIfBound(SelectedNodes);

	struct FMenuEntry
	{
		FText Label;
		FText ToolTip;
		FUIAction UIAction;
	};

	TArray<FMenuEntry> Actions;
	TArray<FMenuEntry> NodeActions;
	// Build Events entries
	for (TFieldIterator<FMulticastDelegateProperty> PropertyIt(SelectedClass, EFieldIteratorFlags::IncludeSuper); PropertyIt; ++PropertyIt)
	{
		FMulticastDelegateProperty* Property = *PropertyIt;

		// Check for multicast delegates that we can safely assign
		if (!Property->HasAnyPropertyFlags(CPF_Parm) && Property->HasAllPropertyFlags(CPF_BlueprintAssignable))
		{
			FName EventName = Property->GetFName();
			int32 ComponentEventViewEntries = 0;
			// Add View Event Per Component
			for (auto NodeIter = SelectedNodes.CreateConstIterator(); NodeIter; ++NodeIter)
			{
				if (NodeIter->Component.IsValid())
				{
					FName VariableName = NodeIter->VariableName;

					// if (VariableProperty && FKismetEditorUtilities::FindBoundEventForComponent(EventNodes.Num() > 0))
					{
						FMenuEntry NewEntry;
						NewEntry.Label = (SelectedNodes.Num() > 1) ? FText::Format(LOCTEXT("ViewEvent_ToolTipFor", "{0} for {1}"), FText::FromName(EventName), FText::FromName(VariableName)) :
							FText::Format(LOCTEXT("ViewEvent_ToolTip", "{0}"), FText::FromName(EventName));
						NewEntry.UIAction = FUIAction(FExecuteAction::CreateStatic(&SGenericSubobjectEditor::ViewEvent, EventName, *NodeIter), CanExecuteActionDelegate);
						NodeActions.Add(NewEntry);
						ComponentEventViewEntries++;
					}
				}
			}
			if (ComponentEventViewEntries < SelectedNodes.Num())
			{
				// Create menu Add entry
				FMenuEntry NewEntry;
				NewEntry.Label = FText::Format(LOCTEXT("AddEvent_ToolTip", "Add {0}"), FText::FromName(EventName));
				NewEntry.UIAction = FUIAction(FExecuteAction::CreateStatic(&SGenericSubobjectEditor::CreateEventsForSelection, EventName, GetSelectedObjectsDelegate), CanExecuteActionDelegate);
				Actions.Add(NewEntry);
			}
		}
	}
	// Build Menu Sections
	Menu.BeginSection("AddComponentActions", LOCTEXT("AddEventHeader", "Add Event"));
	for (auto ItemIter = Actions.CreateConstIterator(); ItemIter; ++ItemIter)
	{
		Menu.AddMenuEntry(ItemIter->Label, ItemIter->ToolTip, FSlateIcon(), ItemIter->UIAction);
	}
	Menu.EndSection();
	Menu.BeginSection("ViewComponentActions", LOCTEXT("ViewEventHeader", "View Existing Events"));
	for (auto ItemIter = NodeActions.CreateConstIterator(); ItemIter; ++ItemIter)
	{
		Menu.AddMenuEntry(ItemIter->Label, ItemIter->ToolTip, FSlateIcon(), ItemIter->UIAction);
	}
	Menu.EndSection();
}

bool SGenericSubobjectEditor::IsEditingAllowed() const
{
	return AllowEditing.Get() && nullptr == GEditor->PlayWorld;
}

FGenericSubobjectEditorTreeNodePtrType SGenericSubobjectEditor::FindSlateNodeForHandle(const FGenericSubobjectDataHandle& InHandleToFind, FGenericSubobjectEditorTreeNodePtrType InStartNodePtr) const
{
	FGenericSubobjectEditorTreeNodePtrType OutNodePtr;
	if (InHandleToFind.IsValid() && RootNodes.Num() > 0)
	{
		FGenericSubobjectData* DataToFind = InHandleToFind.GetSharedDataPtr().Get();

		TSet<FGenericSubobjectEditorTreeNodePtrType> VisitedNodes;
		DepthFirstTraversal(RootNodes[0], VisitedNodes,
			[&OutNodePtr, &DataToFind](
				const FGenericSubobjectEditorTreeNodePtrType& CurNodePtr)
		{
			if (CurNodePtr->GetDataHandle().IsValid())
			{
				if (CurNodePtr->GetDataHandle().GetSharedDataPtr()->GetObject() == DataToFind->GetObject())
				{
					OutNodePtr = CurNodePtr;
				}
			}
		});
	}

	return OutNodePtr;
}

FGenericSubobjectEditorTreeNodePtrType SGenericSubobjectEditor::FindSlateNodeForVariableName(FName InVariableName) const
{
	FGenericSubobjectEditorTreeNodePtrType OutNodePtr;
	if (RootNodes.Num() > 0)
	{
		TSet<FGenericSubobjectEditorTreeNodePtrType> VisitedNodes;
		DepthFirstTraversal(RootNodes[0], VisitedNodes,
			[&OutNodePtr, InVariableName](
				const FGenericSubobjectEditorTreeNodePtrType& CurNodePtr)
		{
			if (CurNodePtr->GetDataHandle().IsValid())
			{
				if (CurNodePtr->GetDataHandle().GetSharedDataPtr()->GetVariableName() == InVariableName)
				{
					OutNodePtr = CurNodePtr;
				}
			}
		});
	}

	return OutNodePtr;
}

FGenericSubobjectEditorTreeNodePtrType SGenericSubobjectEditor::FindSlateNodeForObject(const UObject* InObject, bool bIncludeAttachmentComponents) const
{
	FGenericSubobjectEditorTreeNodePtrType OutNodePtr;

	if (InObject && RootNodes.Num() > 0)
	{
		TSet<FGenericSubobjectEditorTreeNodePtrType> VisitedNodes;

		DepthFirstTraversal(RootNodes[0], VisitedNodes,
			[&OutNodePtr, InObject](
				const FGenericSubobjectEditorTreeNodePtrType& CurNodePtr)
		{
			if (CurNodePtr->GetDataHandle().IsValid())
			{
				if (const FGenericSubobjectData* Data = CurNodePtr->GetDataHandle().GetData())
				{
					if (Data->GetObject() == InObject)
					{
						OutNodePtr = CurNodePtr;
					}
				}
			}
		});

		// If we didn't find it in the tree, step up the chain to the parent of the given component and recursively see if that is in the tree (unless the flag is false)
		if (!OutNodePtr.IsValid() && bIncludeAttachmentComponents)
		{
			const USceneComponent* SceneComponent = Cast<const USceneComponent>(InObject);
			if (SceneComponent && SceneComponent->GetAttachParent())
			{
				return FindSlateNodeForObject(SceneComponent->GetAttachParent(), bIncludeAttachmentComponents);
			}
		}
	}

	return OutNodePtr;
}

TArray<FGenericSubobjectEditorTreeNodePtrType> SGenericSubobjectEditor::GetSelectedNodes() const
{
	TArray<FGenericSubobjectEditorTreeNodePtrType> SelectedTreeNodes = TreeWidget->GetSelectedItems();

	struct FCompareSelectedSubobjectEditorTreeNodes
	{
		FORCEINLINE bool operator()(const FGenericSubobjectEditorTreeNodePtrType& A,
			const FGenericSubobjectEditorTreeNodePtrType& B) const
		{
			return B.IsValid() && B->IsAttachedTo(A);
		}
	};

	// Ensure that nodes are ordered from parent to child (otherwise they are sorted in the order that they were selected)
	SelectedTreeNodes.Sort(FCompareSelectedSubobjectEditorTreeNodes());

	return SelectedTreeNodes;
}

TArray<FGenericSubobjectDataHandle> SGenericSubobjectEditor::GetSelectedHandles() const
{
	TArray<FGenericSubobjectEditorTreeNodePtrType> SelectedNodes = GetSelectedNodes();
	TArray<FGenericSubobjectDataHandle> OutHandles;
	for (FGenericSubobjectEditorTreeNodePtrType& Ptr : SelectedNodes)
	{
		OutHandles.Add(Ptr->GetDataHandle());
	}
	return OutHandles;
}

FGenericSubobjectDataHandle SGenericSubobjectEditor::GetObjectContextHandle() const
{
	if (RootNodes.Num() > 0)
	{
		ensure(RootNodes[0]->GetObject() == GetObjectContext());
		return RootNodes[0]->GetDataHandle();
	}
	else
	{
		return FGenericSubobjectDataHandle::InvalidHandle;
	}
}

void SGenericSubobjectEditor::UpdateTree(bool bRegenerateTreeNodes)
{
	// UpdateTree();

	check(TreeWidget.IsValid());

	// Early exit if we're deferring tree updates
	if (!bAllowTreeUpdates)
	{
		return;
	}

	if (bRegenerateTreeNodes)
	{
		// Obtain the list of selected items
		TArray<FGenericSubobjectEditorTreeNodePtrType> SelectedTreeNodes = GetSelectedNodes();

		// Clear the current tree
		if (SelectedTreeNodes.Num() != 0)
		{
			TreeWidget->ClearSelection();
		}
		RootNodes.Empty();

		if (UObject* Context = GetObjectContext())
		{
			ensureMsgf(FModuleManager::Get().LoadModule("SubobjectDataInterface"), TEXT("The Subobject Data Interface module is required."));

			UGenericSubobjectDataSubsystem* DataSubsystem = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>();
			check(DataSubsystem);

			TArray<FGenericSubobjectDataHandle> SubobjectData;

			// DataSubsystem->GatherSubobjectData(Context, SubobjectData);

			FGenericSubobjectEditorTreeNodePtrType SeperatorNode;
			TMap<FGenericSubobjectDataHandle, FGenericSubobjectEditorTreeNodePtrType> AddedNodes;

			// By default, root node will always be expanded. If possible we will restore the collapsed state later.
			if (SubobjectData.Num() > 0)
			{
				FGenericSubobjectEditorTreeNodePtrType Node = MakeShareable<FGenericSubobjectEditorTreeNode>(
					new FGenericSubobjectEditorTreeNode(SubobjectData[0]));
				RootNodes.Add(Node);
				AddedNodes.Add(Node->GetDataHandle(), Node);
				CachedRootHandle = Node->GetDataHandle();

				TreeWidget->SetItemExpansion(Node, true);
				TreeWidget->SetItemExpansion(SeperatorNode, true);

				RefreshFilteredState(Node, false);
			}

			// Create slate nodes for each subobject
			for (FGenericSubobjectDataHandle& Handle : SubobjectData)
			{
				// Do we have a slate node for this handle already? If not, then we need to make one
				FGenericSubobjectEditorTreeNodePtrType NewNode = SGenericSubobjectEditor::FindOrCreateSlateNodeForHandle(Handle, AddedNodes);

				FGenericSubobjectData* Data = Handle.GetData();

				const FGenericSubobjectDataHandle& ParentHandle = Data->GetParentHandle();

				// Have parent? 
				if (ParentHandle.IsValid())
				{
					// Get the parent node for this subobject
					FGenericSubobjectEditorTreeNodePtrType ParentNode = SGenericSubobjectEditor::FindOrCreateSlateNodeForHandle(ParentHandle, AddedNodes);

					check(ParentNode);
					ParentNode->AddChild(NewNode);
					TreeWidget->SetItemExpansion(ParentNode, true);

					const bool bFilteredOut = RefreshFilteredState(NewNode, false);

					// Add a separator after the default scene root, but only it is not filtered out and if there are more items below it
					if (!bFilteredOut &&
						Data->IsDefaultSceneRoot() && // Always false
						Data->IsInheritedComponent() &&
						SubobjectData.Find(Handle) < SubobjectData.Num() - 1)
					{
						SeperatorNode = MakeShareable<FGenericSubobjectEditorTreeNode>(
							new FGenericSubobjectEditorTreeNode(FGenericSubobjectDataHandle::InvalidHandle, /** bIsSeperator */true));
						AddedNodes.Add(SeperatorNode->GetDataHandle(), SeperatorNode);
						ParentNode->AddChild(SeperatorNode);
					}

				}
				TreeWidget->SetItemExpansion(NewNode, true);
			}

			RestoreSelectionState(SelectedTreeNodes);

			// If we have a pending deferred rename request, redirect it to the new tree node
			if (DeferredRenameRequest.IsValid())
			{
				FGenericSubobjectEditorTreeNodePtrType NodeToRenamePtr = FindSlateNodeForHandle(DeferredRenameRequest);
				if (NodeToRenamePtr.IsValid())
				{
					TreeWidget->RequestScrollIntoView(NodeToRenamePtr);
				}
			}
		}
	}

	TreeWidget->RequestTreeRefresh();
}

EVisibility SGenericSubobjectEditor::GetComponentClassComboButtonVisibility() const
{
	return EVisibility();
}

TSubclassOf<UGenericGraphNodeComponent> SGenericSubobjectEditor::GetComponentTypeFilterToApply() const
{
	return UGenericGraphNodeComponent::StaticClass();
}

bool SGenericSubobjectEditor::RefreshFilteredState(FGenericSubobjectEditorTreeNodePtrType TreeNode, bool bRecursive)
{
	const UClass* FilterType = GetComponentTypeFilterToApply();

	FString FilterText = FText::TrimPrecedingAndTrailing(GetFilterText()).ToString();
	TArray<FString> FilterTerms;
	FilterText.ParseIntoArray(FilterTerms, TEXT(" "), /*CullEmpty =*/true);

	TreeNode->RefreshFilteredState(FilterType, FilterTerms, bRecursive);
	return TreeNode->IsFlaggedForFiltration();
}

FText SGenericSubobjectEditor::GetFilterText() const
{
	return FilterBox->GetText();
}

void SGenericSubobjectEditor::CreateEventsForSelection(FName EventName, FGetSelectedObjectsDelegate GetSelectedObjectsDelegate)
{
	if (EventName != NAME_None)
	{
		TArray<FComponentEventConstructionData> SelectedNodes;
		GetSelectedObjectsDelegate.ExecuteIfBound(SelectedNodes);

		for (auto SelectionIter = SelectedNodes.CreateConstIterator(); SelectionIter; ++SelectionIter)
		{
			ConstructEvent(EventName, *SelectionIter);
		}
	}
}

void SGenericSubobjectEditor::ConstructEvent(const FName EventName, const FComponentEventConstructionData EventData)
{
	// Find the corresponding variable property in the Blueprint
	/* FObjectProperty* VariableProperty = FindFProperty<FObjectProperty>(Blueprint->SkeletonGeneratedClass, EventData.VariableName);

	if (VariableProperty)
	{
		if (!FKismetEditorUtilities::FindBoundEventForComponent(Blueprint, EventName, VariableProperty->GetFName()))
		{
			FKismetEditorUtilities::CreateNewBoundEventForComponent(EventData.Component.Get(), EventName, Blueprint, VariableProperty);
		}
	} */
}

void SGenericSubobjectEditor::ViewEvent(const FName EventName, const FComponentEventConstructionData EventData)
{
	// Find the corresponding variable property in the Blueprint
	/* FObjectProperty* VariableProperty = FindFProperty<FObjectProperty>(Blueprint->SkeletonGeneratedClass, EventData.VariableName);

	if (VariableProperty)
	{
		const UK2Node_ComponentBoundEvent* ExistingNode = FKismetEditorUtilities::FindBoundEventForComponent(EventName, VariableProperty->GetFName());
		if (ExistingNode)
		{
			FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(ExistingNode);
		}
	} */
}

TSharedRef<ITableRow> SGenericSubobjectEditor::MakeTableRowWidget(FGenericSubobjectEditorTreeNodePtrType InNodePtr, const TSharedRef<STableViewBase>& OwnerTable)
{
	FGraphNodeMetaData TagMeta(TEXT("TableRow"));
	const UGenericGraphNodeComponent* Template = InNodePtr && !InNodePtr->IsSeperator()
		? InNodePtr->GetDataSource()->GetComponentTemplate()
		: nullptr;
	if (Template)
	{
		TagMeta.FriendlyName = FString::Printf(TEXT("TableRow,%s,0"), *Template->GetReadableName());
	}

	return SNew(SGenericSubobject_RowWidget, SharedThis(this), InNodePtr, OwnerTable)
		.AddMetaData<FTutorialMetaData>(TagMeta);
}

EVisibility SGenericSubobjectEditor::GetComponentsTreeVisibility() const
{
	return EVisibility();
}

void SGenericSubobjectEditor::OnGetChildrenForTree(FGenericSubobjectEditorTreeNodePtrType InNodePtr, TArray<FGenericSubobjectEditorTreeNodePtrType>& OutChildren)
{
}

void SGenericSubobjectEditor::UpdateSelectionFromNodes(const TArray<FGenericSubobjectEditorTreeNodePtrType>& SelectedNodes)
{
}

void SGenericSubobjectEditor::OnTreeSelectionChanged(FGenericSubobjectEditorTreeNodePtrType InSelectedNodePtr, ESelectInfo::Type SelectInfo)
{
}

void SGenericSubobjectEditor::HandleItemDoubleClicked(FGenericSubobjectEditorTreeNodePtrType InItem)
{
}

void SGenericSubobjectEditor::OnFindReferences(bool bSearchAllBlueprints, const EGetFindReferenceSearchStringFlags Flags)
{
}

EVisibility SGenericSubobjectEditor::GetComponentsFilterBoxVisibility() const
{
	return EVisibility();
}

void SGenericSubobjectEditor::OnFilterTextChanged(const FText& InFilterText)
{
}

void SGenericSubobjectEditor::OnItemScrolledIntoView(FGenericSubobjectEditorTreeNodePtrType InItem, const TSharedPtr<ITableRow>& InWidget)
{
}

void SGenericSubobjectEditor::RestoreSelectionState(TArray<FGenericSubobjectEditorTreeNodePtrType>& SelectedTreeNodes, bool bFallBackToVariableName)
{
	if (SelectedTreeNodes.Num() > 0)
	{
		// If there is only one item selected, imitate user selection to preserve navigation
		ESelectInfo::Type SelectInfo = SelectedTreeNodes.Num() == 1 ? ESelectInfo::OnMouseClick : ESelectInfo::Direct;

		// Restore the previous selection state on the new tree nodes
		for (int i = 0; i < SelectedTreeNodes.Num(); ++i)
		{
			if (RootNodes.Num() > 0 && SelectedTreeNodes[i] == RootNodes[0])
			{
				TreeWidget->SetItemSelection(RootNodes[0], true, SelectInfo);
			}
			else
			{
				FGenericSubobjectDataHandle CurrentNodeDataHandle = SelectedTreeNodes[i]->GetDataHandle();
				FGenericSubobjectEditorTreeNodePtrType NodeToSelectPtr = FindSlateNodeForHandle(CurrentNodeDataHandle);

				// If we didn't find something for this exact handle, fall back to just search for something
				// with the same variable name. This helps to still preserve selection across re-compiles of a class.
				if (!NodeToSelectPtr.IsValid() && CurrentNodeDataHandle.IsValid() && bFallBackToVariableName)
				{
					NodeToSelectPtr = FindSlateNodeForVariableName(CurrentNodeDataHandle.GetSharedDataPtr()->GetVariableName());
				}

				if (NodeToSelectPtr.IsValid())
				{
					TreeWidget->SetItemSelection(NodeToSelectPtr, true, SelectInfo);
				}
			}

			//if (GetEditorMode() != EComponentEditorMode::BlueprintSCS)
			//{
			//	TArray<FGenericSubobjectEditorTreeNodePtrType> NewSelectedTreeNodes = TreeWidget->GetSelectedItems();
			//	if (NewSelectedTreeNodes.Num() == 0)
			//	{
			//		TreeWidget->SetItemSelection(RootNodes[0], true);
			//	}
			//}
		}
	}
}

FReply SGenericSubobjectEditor::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (CommandList->ProcessCommandBindings(InKeyEvent))
	{
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void SGenericSubobjectEditor::OnRenameComponent()
{
	OnRenameComponent(nullptr);
}

void SGenericSubobjectEditor::OnRenameComponent(TUniquePtr<FScopedTransaction> InComponentCreateTransaction)
{
	TArray<FGenericSubobjectEditorTreeNodePtrType> SelectedNodes = GetSelectedNodes();

	// Should already be prevented from making it here.
	check(SelectedNodes.Num() == 1);

	DeferredRenameRequest = SelectedNodes[0]->GetDataHandle();

	// If this fails, something in the chain of responsibility failed to end the previous transaction.
	check(!DeferredOngoingCreateTransaction.IsValid());
	// If a 'create + give initial name' transaction is ongoing, take responsibility of ending it until the selected item is scrolled into view.
	DeferredOngoingCreateTransaction = MoveTemp(InComponentCreateTransaction);

	TreeWidget->RequestScrollIntoView(SelectedNodes[0]);

	if (DeferredOngoingCreateTransaction.IsValid() && !PostTickHandle.IsValid())
	{
		// Ensure the item will be scrolled into view during the frame (See explanation in OnPostTick()).
		// PostTickHandle = FSlateApplication::Get().OnPostTick().AddSP(this, &SGenericSubobjectEditor::OnPostTick);
	}
}

#undef LOCTEXT_NAMESPACE

FGenericSubobjectEditorTreeNode::FGenericSubobjectEditorTreeNode(const FGenericSubobjectDataHandle& DataSource, bool InbIsSeperator)
{
}

const UObject* FGenericSubobjectEditorTreeNode::GetObject(bool bEvenIfPendingKill) const
{
	return nullptr;
}

FGenericSubobjectData* FGenericSubobjectEditorTreeNode::GetDataSource() const
{
	return nullptr;
}

FString FGenericSubobjectEditorTreeNode::GetDisplayString() const
{
	return FString();
}

bool FGenericSubobjectEditorTreeNode::IsComponentNode() const
{
	return false;
}

bool FGenericSubobjectEditorTreeNode::IsRootActorNode() const
{
	return false;
}

bool FGenericSubobjectEditorTreeNode::CanReparent() const
{
	return false;
}

FName FGenericSubobjectEditorTreeNode::GetVariableName() const
{
	return FName();
}

bool FGenericSubobjectEditorTreeNode::IsChildSubtreeNode() const
{
	return false;
}

bool FGenericSubobjectEditorTreeNode::IsNativeComponent() const
{
	return false;
}

bool FGenericSubobjectEditorTreeNode::IsAttachedTo(FGenericSubobjectEditorTreeNodePtrType InNodePtr) const
{
	return false;
}

bool FGenericSubobjectEditorTreeNode::IsDirectlyAttachedTo(FGenericSubobjectEditorTreeNodePtrType InNodePtr) const
{
	return false;
}

bool FGenericSubobjectEditorTreeNode::CanDelete() const
{
	return false;
}

bool FGenericSubobjectEditorTreeNode::CanRename() const
{
	return false;
}

void FGenericSubobjectEditorTreeNode::AddChild(FGenericSubobjectEditorTreeNodePtrType AttachToPtr)
{
}

void FGenericSubobjectEditorTreeNode::RemoveChild(FGenericSubobjectEditorTreeNodePtrType InChildNodePtr)
{
}

FGenericSubobjectEditorTreeNodePtrType FGenericSubobjectEditorTreeNode::FindChild(const FGenericSubobjectDataHandle& InHandle)
{
	return FGenericSubobjectEditorTreeNodePtrType();
}

bool FGenericSubobjectEditorTreeNode::IsFlaggedForFiltration() const
{
	return false;
}

void FGenericSubobjectEditorTreeNode::SetCachedFilterState(bool bMatchesFilter, bool bUpdateParent)
{
}

void FGenericSubobjectEditorTreeNode::ApplyFilteredStateToParent()
{
}

void FGenericSubobjectEditorTreeNode::RefreshCachedChildFilterState(bool bUpdateParent)
{
}

bool FGenericSubobjectEditorTreeNode::RefreshFilteredState(const UClass* InFilterType, const TArray<FString>& InFilterTerms, bool bRecursive)
{
	return false;
}

bool FGenericSubobjectEditorTreeNode::MatchesFilterType(const UClass* InFilterType) const
{
	return false;
}

void FGenericSubobjectEditorTreeNode::SetOngoingCreateTransaction(TUniquePtr<FScopedTransaction> InTransaction)
{
}

void FGenericSubobjectEditorTreeNode::CloseOngoingCreateTransaction()
{
}
