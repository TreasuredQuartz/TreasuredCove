// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "GenericGraphAssetEditor/SSubobjectGenericGraphNodeEditor.h"
#include "GenericGraphAssetEditor/GenericGraphNodeSEEC.h"
#include "GenericGraphAssetEditor/SGenericNodeComponentClassCombo.h"
#include "GenericSubobjectDataSubsystem.h"
#include "GenericGraphNodeComponent.h"
#include "GenericGraphNode.h"

#include "GenericComponentAssetBroker.h"

#include "IDocumentation.h"
#include "GraphEditorActions.h"
#include "Selection.h"
#include "ClassViewerFilter.h"

#include "SSubobjectEditor.h"
#include "SPositiveActionButton.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Widgets/Input/SSearchBox.h"
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
#include "K2Node_ComponentBoundEvent.h"
#include "BlueprintEditor.h"

#define LOCTEXT_NAMESPACE "SSubobjectGenericGraphNodeEditor"

extern UNREALED_API UUnrealEdEngine* GUnrealEd;

////////////////////////////////////////////////
// SSubobjectGenericGraphNodeEditor

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SSubobjectGenericGraphNodeEditor::Construct(const FArguments& InArgs)
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

	ConstructTreeWidget();

	// Should only be true when used in the blueprints details panel
	const bool bInlineSearchBarWithButtons = ShowInlineSearchWithButtons();

	TSharedPtr<SWidget> Contents;

	TSharedPtr<SVerticalBox> HeaderBox;
	TSharedPtr<SWidget> SearchBar = SAssignNew(FilterBox, SSearchBox)
		.HintText(!bInlineSearchBarWithButtons ? LOCTEXT("SearchComponentsHint", "Search Components") : LOCTEXT("SearchHint", "Search"))
		.OnTextChanged(this, &SSubobjectGenericGraphNodeEditor::OnFilterTextChanged)
		.Visibility(this, &SSubobjectGenericGraphNodeEditor::GetComponentsFilterBoxVisibility);

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
				.Visibility(this, &SSubobjectGenericGraphNodeEditor::GetComponentClassComboButtonVisibility)
				.OnSubobjectClassSelected(this, &SSubobjectGenericGraphNodeEditor::PerformComboAddClass)
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
				.Visibility(this, &SSubobjectGenericGraphNodeEditor::GetPromoteToBlueprintButtonVisibility)
				.OnClicked(this, &SSubobjectGenericGraphNodeEditor::OnPromoteToBlueprintClicked)
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
				.Visibility(this, &SSubobjectGenericGraphNodeEditor::GetEditBlueprintButtonVisibility)
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
				.Visibility(this, &SSubobjectGenericGraphNodeEditor::GetComponentsTreeVisibility)
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
	// UpdateTree();
	UpdateGenericTree();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FSubobjectDataHandle SSubobjectGenericGraphNodeEditor::PerformComboAddClass(TSubclassOf<UGenericGraphNodeComponent> ComponentClass, EComponentCreateAction::Type ComponentCreateAction, UObject* AssetOverride)
{
	FSubobjectDataHandle NewComponentHandle = FSubobjectDataHandle::InvalidHandle;

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

		// This adds components according to the type selected in the drop down. If the user
		// has the appropriate objects selected in the content browser then those are added,
		// else we go down the previous route of adding components by type.
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
						const TArray<FSubobjectDataHandle>& SelectedHandles = GetSelectedHandles();
						const FSubobjectDataHandle& ParentHandle = SelectedHandles.Num() > 0 ? SelectedHandles[0] : CachedRootHandle;
						ensureMsgf(ParentHandle.IsValid(), TEXT("Attempting to add a component from an invalid selection!"));

						NewComponentHandle = AddNewSubobject(ParentHandle, NewClass, Object, OutFailReason, MoveTemp(AddTransaction));
						bAddedComponent = true;
						break;
					}
				}
			}
		}

		if (!bAddedComponent)
		{
			// Attach this component to the override asset first, but if none is given then use the actor context			
			FSubobjectDataHandle ParentHandle = SubobjectSystem->FindHandleForObject(CachedRootHandle, AssetOverride);

			if (!ParentHandle.IsValid())
			{
				// If we have something selected, then we should attach it to that
				TArray<FSubobjectEditorTreeNodePtrType> SelectedTreeNodes = TreeWidget->GetSelectedItems();
				if (SelectedTreeNodes.Num() > 0)
				{
					ParentHandle = SelectedTreeNodes[0]->GetDataHandle();
				}
				// Otherwise fall back to the root node
				else
				{
					ParentHandle = RootNodes.Num() > 0
						? RootNodes[0]->GetDataHandle()
						: FSubobjectDataHandle::InvalidHandle;
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
			FSubobjectEditorTreeNodePtrType NewNode = FindSlateNodeForHandle(NewComponentHandle);
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

UClass* SSubobjectGenericGraphNodeEditor::SpawnCreateNewCppComponentWindow(TSubclassOf<UGenericGraphNodeComponent> ComponentClass)
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

UClass* SSubobjectGenericGraphNodeEditor::SpawnCreateNewBPComponentWindow(TSubclassOf<UGenericGraphNodeComponent> ComponentClass)
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

void SSubobjectGenericGraphNodeEditor::OnDeleteNodes()
{
	const FScopedTransaction Transaction(LOCTEXT("RemoveComponents", "Remove Components"));

	// Invalidate any active component in the visualizer
	GUnrealEd->ComponentVisManager.ClearActiveComponentVis();

	UBlueprint* Blueprint = GetBlueprint();

	// Get the current render info for the blueprint. If this is NULL then the blueprint is not currently visualizable (no visible primitive components)
	FThumbnailRenderingInfo* RenderInfo = Blueprint ? GUnrealEd->GetThumbnailManager()->GetRenderingInfo(Blueprint) : nullptr;

	// A lamda for displaying a confirm message to the user if there is a dynamic delegate bound to the 
	// component they are trying to delete
	auto ConfirmDeleteLambda = [](const FSubobjectData* Data) -> FSuppressableWarningDialog::EResult
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

	// Gather the handles of the components that we want to delete
	TArray<FSubobjectDataHandle> HandlesToDelete;
	TArray<FSubobjectEditorTreeNodePtrType> SelectedNodes = GetSelectedNodes();
	for (const FSubobjectEditorTreeNodePtrType& Node : SelectedNodes)
	{
		check(Node->IsValid());

		if (const FSubobjectData* Data = Node->GetDataSource())
		{
			// If this node is in use by Dynamic delegates, then confirm before continuing
			if (FKismetEditorUtilities::PropertyHasBoundEvents(Blueprint, Data->GetVariableName()))
			{
				// The user has decided not to delete the component, stop trying to delete this component
				if (ConfirmDeleteLambda(Data) == FSuppressableWarningDialog::Cancel)
				{
					return;
				}
			}
			HandlesToDelete.Add(Data->GetHandle());
		}
	}

	if (UGenericSubobjectDataSubsystem* System = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>())
	{
		FSubobjectDataHandle HandleToSelect;

		int32 NumDeleted = System->DeleteSubobjects(RootNodes[0]->GetDataHandle(), HandlesToDelete, HandleToSelect, GetBlueprint());

		if (NumDeleted > 0)
		{
			if (HandleToSelect.IsValid())
			{
				FSubobjectEditorTreeNodePtrType NodeToSelect = FindSlateNodeForHandle(HandleToSelect);
				if (NodeToSelect.IsValid())
				{
					TreeWidget->SetSelection(NodeToSelect);
				}
			}

			UpdateTree();

			// If we had a thumbnail before we deleted any components, check to see if we should clear it
			// If we deleted the final visualizable primitive from the blueprint, GetRenderingInfo should return NULL
			if (Blueprint && RenderInfo)
			{
				FThumbnailRenderingInfo* NewRenderInfo = GUnrealEd->GetThumbnailManager()->GetRenderingInfo(Blueprint);
				if (RenderInfo && !NewRenderInfo)
				{
					// We removed the last visible primitive component, clear the thumbnail
					const FString BPFullName = FString::Printf(TEXT("%s %s"), *Blueprint->GetClass()->GetName(), *Blueprint->GetPathName());
					UPackage* BPPackage = Blueprint->GetOutermost();
					ThumbnailTools::CacheEmptyThumbnail(BPFullName, BPPackage);
				}
			}
			// Do this AFTER marking the Blueprint as modified
			UpdateSelectionFromNodes(TreeWidget->GetSelectedItems());
		}
	}
}

void SSubobjectGenericGraphNodeEditor::CopySelectedNodes()
{
	TArray<FSubobjectDataHandle> SelectedHandles = GetSelectedHandles();
	if (UGenericSubobjectDataSubsystem* System = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>())
	{
		return System->CopySubobjects(SelectedHandles, GetBlueprint());
	}
}

void SSubobjectGenericGraphNodeEditor::OnDuplicateComponent()
{
	TArray<FSubobjectDataHandle> SelectedNodes = GetSelectedHandles();
	if (SelectedNodes.Num() > 0)
	{
		// Force the text box being edited (if any) to commit its text. The duplicate operation may trigger a regeneration of the tree view,
		// releasing all row widgets. If one row was in edit mode (rename/rename on create), it was released before losing the focus and
		// this would prevent the completion of the 'rename' or 'create + give initial name' transaction (occurring on focus lost).
		FSlateApplication::Get().ClearKeyboardFocus();

		TUniquePtr<FScopedTransaction> Transaction = MakeUnique<FScopedTransaction>(SelectedNodes.Num() > 1 ? LOCTEXT("DuplicateComponents", "Duplicate Components") : LOCTEXT("DuplicateComponent", "Duplicate Component"));

		if (UGenericSubobjectDataSubsystem* System = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>())
		{
			TArray<FSubobjectDataHandle> DuplicatedHandles;
			System->DuplicateSubobjects(GetObjectContextHandle(), SelectedNodes, GetBlueprint(), DuplicatedHandles);
			UpdateTree();

			// Set focus to the newly created subobject
			FSubobjectEditorTreeNodePtrType NewNode = DuplicatedHandles.Num() > 0 ? FindSlateNodeForHandle(DuplicatedHandles[0]) : nullptr;
			if (NewNode != nullptr)
			{
				TreeWidget->SetSelection(NewNode);
				OnRenameComponent(MoveTemp(Transaction));
			}
		}
	}
}

bool SSubobjectGenericGraphNodeEditor::CanPasteNodes() const
{
	if (!IsEditingAllowed())
	{
		return false;
	}

	if (FSubobjectEditorTreeNodePtrType SceneRoot = GetSceneRootNode())
	{
		if (UGenericSubobjectDataSubsystem* System = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>())
		{
			return System->CanPasteSubobjects(SceneRoot->GetDataHandle(), GetBlueprint());
		}
	}

	return false;
}

void SSubobjectGenericGraphNodeEditor::PasteNodes()
{
	if (UGenericSubobjectDataSubsystem* System = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>())
	{
		TArray<FSubobjectDataHandle> OutHandles;

		// stop allowing tree updates during paste, otherwise MarkBlueprintAsModified will trigger in the middle of it
		bool bRestoreAllowTreeUpdates = bAllowTreeUpdates;
		bAllowTreeUpdates = false;
		TArray<FSubobjectDataHandle> HandlesToPasteOnto = GetSelectedHandles();
		if (HandlesToPasteOnto.IsEmpty())
		{
			if (FSubobjectEditorTreeNodePtrType RootPtr = GetSceneRootNode())
			{
				if (RootPtr.IsValid())
				{
					HandlesToPasteOnto.Emplace(RootPtr->GetDataHandle());
				}
			}
		}

		System->PasteSubobjects(GetObjectContextHandle(), HandlesToPasteOnto, GetBlueprint(), OutHandles);

		// allow tree updates again
		bAllowTreeUpdates = bRestoreAllowTreeUpdates;

		if (OutHandles.Num() > 0)
		{
			// We only want the pasted node(s) to be selected
			TreeWidget->ClearSelection();
			UpdateTree();

			for (const FSubobjectDataHandle& Handle : OutHandles)
			{
				if (FSubobjectEditorTreeNodePtrType SlateNode = FindSlateNodeForHandle(Handle))
				{
					TreeWidget->RequestScrollIntoView(SlateNode);
					TreeWidget->SetItemSelection(SlateNode, true);
				}
			}
		}
	}
}

void SSubobjectGenericGraphNodeEditor::OnAttachToDropAction(FSubobjectEditorTreeNodePtrType DroppedOn, const TArray<FSubobjectEditorTreeNodePtrType>& DroppedNodePtrs)
{
	// Ask the subsystem to attach the dropped nodes onto the dropped on node
	check(DroppedOn.IsValid());
	check(DroppedNodePtrs.Num() > 0);

	UGenericSubobjectDataSubsystem* System = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>();
	check(System);

	const FScopedTransaction TransactionContext(DroppedNodePtrs.Num() > 1 ? LOCTEXT("AttachComponents", "Attach Components") : LOCTEXT("AttachComponent", "Attach Component"));
	TArray<FSubobjectDataHandle> HandlesToMove;
	for (const FSubobjectEditorTreeNodePtrType& DroppedNodePtr : DroppedNodePtrs)
	{
		HandlesToMove.Add(DroppedNodePtr->GetDataHandle());
	}

	FReparentSubobjectParams Params;
	Params.NewParentHandle = DroppedOn->GetDataHandle();

	// These both do nothing if not an actor.
	// Params.BlueprintContext = GetBlueprint();
	// Params.ActorPreviewContext = GetActorPreview();

	System->ReparentSubobjects(Params, HandlesToMove);

	check(TreeWidget.IsValid());
	TreeWidget->SetItemExpansion(DroppedOn, true);

	PostDragDropAction(true);
}

void SSubobjectGenericGraphNodeEditor::OnDetachFromDropAction(const TArray<FSubobjectEditorTreeNodePtrType>& DroppedNodePtrs)
{
	check(DroppedNodePtrs.Num() > 0);

	const FScopedTransaction TransactionContext(DroppedNodePtrs.Num() > 1 ? LOCTEXT("DetachComponents", "Detach Components") : LOCTEXT("DetachComponent", "Detach Component"));

	TArray<FSubobjectDataHandle> HandlesToMove;
	// Utils::PopulateHandlesArray(DroppedNodePtrs, HandlesToMove); <-- Function expanded below.
	for (const FSubobjectEditorTreeNodePtrType& DroppedNodePtr : DroppedNodePtrs)
	{
		HandlesToMove.Add(DroppedNodePtr->GetDataHandle());
	}

	// Attach the dropped node to the current scene root node
	FSubobjectEditorTreeNodePtrType SceneRootNodePtr = GetSceneRootNode();
	check(SceneRootNodePtr.IsValid());

	// Ask the subsystem to reparent this object to the scene root
	if (UGenericSubobjectDataSubsystem* System = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>())
	{
		FReparentSubobjectParams Params;
		Params.NewParentHandle = SceneRootNodePtr->GetDataHandle();
		// Params.BlueprintContext = GetBlueprint();
	}

	PostDragDropAction(true);
}

void SSubobjectGenericGraphNodeEditor::PostDragDropAction(bool bRegenerateTreeNodes)
{
	GUnrealEd->ComponentVisManager.ClearActiveComponentVis();

	UpdateTree(bRegenerateTreeNodes);

	FBlueprintEditorUtils::PostEditChangeBlueprintActors(GetBlueprint(), true);
}

FSubobjectDataHandle SSubobjectGenericGraphNodeEditor::AddNewSubobject(const FSubobjectDataHandle& ParentHandle, UClass* NewClass, UObject* AssetOverride, FText& OutFailReason, TUniquePtr<FScopedTransaction> InOngoingTransaction)
{
	FAddNewSubobjectParams Params;
	Params.ParentHandle = ParentHandle;
	Params.NewClass = NewClass;
	Params.AssetOverride = AssetOverride;

	// Make sure to populate the blueprint context of what we are currently editing!
	Params.BlueprintContext = GetBlueprint();
	UGenericSubobjectDataSubsystem* System = GEngine->GetEngineSubsystem<UGenericSubobjectDataSubsystem>();
	check(System);

	return System->AddNewSubobject(Params, OutFailReason);
}

void SSubobjectGenericGraphNodeEditor::PopulateContextMenuImpl(UToolMenu* InMenu, TArray<FSubobjectEditorTreeNodePtrType>& InSelectedItems, bool bIsChildActorSubtreeNodeSelected)
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
			FSubobjectEditorTreeNodePtrType TreeNode = *NodeIter;
			const FSubobjectData* Data = TreeNode->GetDataSource();
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
					FNewMenuDelegate::CreateStatic(&SSubobjectGenericGraphNodeEditor::BuildMenuEventsSection,
						GetBlueprint(), SelectedClass, FCanExecuteAction::CreateSP(this, &SSubobjectEditor::IsEditingAllowed),
						FGetSelectedObjectsDelegate::CreateSP(this, &SSubobjectEditor::GetSelectedItemsForContextMenu)));
			}
		}
	}

	TArray<UGenericGraphNodeComponent*> SelectedComponents;
	for (const FSubobjectEditorTreeNodePtrType& SelectedNodePtr : InSelectedItems)
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

void SSubobjectGenericGraphNodeEditor::BuildMenuEventsSection(FMenuBuilder& Menu, UBlueprint* Blueprint, UClass* SelectedClass, FCanExecuteAction CanExecuteActionDelegate, FGetSelectedObjectsDelegate GetSelectedObjectsDelegate)
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
					FObjectProperty* VariableProperty = FindFProperty<FObjectProperty>(Blueprint->SkeletonGeneratedClass, VariableName);

					if (VariableProperty && FKismetEditorUtilities::FindBoundEventForComponent(Blueprint, EventName, VariableProperty->GetFName()))
					{
						FMenuEntry NewEntry;
						NewEntry.Label = (SelectedNodes.Num() > 1) ? FText::Format(LOCTEXT("ViewEvent_ToolTipFor", "{0} for {1}"), FText::FromName(EventName), FText::FromName(VariableName)) :
							FText::Format(LOCTEXT("ViewEvent_ToolTip", "{0}"), FText::FromName(EventName));
						NewEntry.UIAction = FUIAction(FExecuteAction::CreateStatic(&SSubobjectGenericGraphNodeEditor::ViewEvent, Blueprint, EventName, *NodeIter), CanExecuteActionDelegate);
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
				NewEntry.UIAction = FUIAction(FExecuteAction::CreateStatic(&SSubobjectGenericGraphNodeEditor::CreateEventsForSelection, Blueprint, EventName, GetSelectedObjectsDelegate), CanExecuteActionDelegate);
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

FSubobjectEditorTreeNodePtrType SSubobjectGenericGraphNodeEditor::FindSlateNodeForObject(const UObject* InObject, bool bIncludeAttachmentComponents) const
{
	if (const UGenericGraphNodeComponent* NodeComponent = Cast<UGenericGraphNodeComponent>(InObject))
	{
		// If the given component instance is not already an archetype object
		if (!NodeComponent->IsTemplate())
		{
			// Get the component owner's class object
			check(NodeComponent->GetOwner() != NULL);
			UClass* OwnerClass = NodeComponent->GetOwner()->GetClass();

			// If the given component is one that's created during Blueprint construction
			if (NodeComponent->IsCreatedByConstructionScript())
			{
				// Check the entire Class hierarchy for the node
				TArray<UBlueprintGeneratedClass*> ParentBPStack;
				UBlueprint::GetBlueprintHierarchyFromClass(OwnerClass, ParentBPStack);
			
				for (int32 StackIndex = ParentBPStack.Num() - 1; StackIndex >= 0; --StackIndex)
				{
					USimpleConstructionScript* ParentSCS = ParentBPStack[StackIndex] ? ParentBPStack[StackIndex]->SimpleConstructionScript.Get() : nullptr;
					if (ParentSCS)
					{
						// Attempt to locate an SCS node with a variable name that matches the name of the given component
						for (USCS_Node* SCS_Node : ParentSCS->GetAllNodes())
						{
							check(SCS_Node != nullptr);
							if (SCS_Node->GetVariableName() == NodeComponent->GetFName())
							{
								// We found a match; redirect to the component archetype instance that may be associated with a tree node
								NodeComponent = Cast<UGenericGraphNodeComponent>(SCS_Node->ComponentClass->GetArchetype());
								break;
							}
						}
					}
				}
			}
			else
			{
				// Get the class default object
				const UGenericGraphNode* CDO = Cast<UGenericGraphNode>(OwnerClass->GetDefaultObject());
				if (CDO)
				{
					// Iterate over the Components array and attempt to find a component with a matching name
					for (UGenericGraphNodeComponent* ComponentTemplate : CDO->GetComponents())
					{
						if (ComponentTemplate && ComponentTemplate->GetFName() == NodeComponent->GetFName())
						{
							// We found a match; redirect to the component archetype instance that may be associated with a tree node
							NodeComponent = ComponentTemplate;
							break;
						}
					}
				}
			}
		}
		InObject = NodeComponent;
	}

	return SSubobjectEditor::FindSlateNodeForObject(InObject, bIncludeAttachmentComponents);
}

void SSubobjectGenericGraphNodeEditor::UpdateGenericTree(bool bRegenerateTreeNodes)
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
		TArray<FSubobjectEditorTreeNodePtrType> SelectedTreeNodes = GetSelectedNodes();

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

			TArray<FSubobjectDataHandle> SubobjectData;

			DataSubsystem->GatherSubobjectData(Context, SubobjectData);

			FSubobjectEditorTreeNodePtrType SeperatorNode;
			TMap<FSubobjectDataHandle, FSubobjectEditorTreeNodePtrType> AddedNodes;

			// By default, root node will always be expanded. If possible we will restore the collapsed state later.
			if (SubobjectData.Num() > 0)
			{
				FSubobjectEditorTreeNodePtrType Node = MakeShareable<FSubobjectEditorTreeNode>(
					new FSubobjectEditorTreeNode(SubobjectData[0]));
				RootNodes.Add(Node);
				AddedNodes.Add(Node->GetDataHandle(), Node);
				CachedRootHandle = Node->GetDataHandle();

				TreeWidget->SetItemExpansion(Node, true);
				TreeWidget->SetItemExpansion(SeperatorNode, true);

				RefreshFilteredState(Node, false);
			}

			// Create slate nodes for each subobject
			for (FSubobjectDataHandle& Handle : SubobjectData)
			{
				// Do we have a slate node for this handle already? If not, then we need to make one
				FSubobjectEditorTreeNodePtrType NewNode = SSubobjectEditor::FindOrCreateSlateNodeForHandle(Handle, AddedNodes);

				FSubobjectData* Data = Handle.GetData();

				const FSubobjectDataHandle& ParentHandle = Data->GetParentHandle();

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
						Data->IsDefaultSceneRoot() && // Always false
						Data->IsInheritedComponent() &&
						SubobjectData.Find(Handle) < SubobjectData.Num() - 1)
					{
						SeperatorNode = MakeShareable<FSubobjectEditorTreeNode>(
							new FSubobjectEditorTreeNode(FSubobjectDataHandle::InvalidHandle, /** bIsSeperator */true));
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
				FSubobjectEditorTreeNodePtrType NodeToRenamePtr = FindSlateNodeForHandle(DeferredRenameRequest);
				if (NodeToRenamePtr.IsValid())
				{
					TreeWidget->RequestScrollIntoView(NodeToRenamePtr);
				}
			}
		}
	}

	TreeWidget->RequestTreeRefresh();
}

void SSubobjectGenericGraphNodeEditor::CreateEventsForSelection(UBlueprint* Blueprint, FName EventName, FGetSelectedObjectsDelegate GetSelectedObjectsDelegate)
{
	if (EventName != NAME_None)
	{
		TArray<FComponentEventConstructionData> SelectedNodes;
		GetSelectedObjectsDelegate.ExecuteIfBound(SelectedNodes);

		for (auto SelectionIter = SelectedNodes.CreateConstIterator(); SelectionIter; ++SelectionIter)
		{
			ConstructEvent(Blueprint, EventName, *SelectionIter);
		}
	}
}

void SSubobjectGenericGraphNodeEditor::ConstructEvent(UBlueprint* Blueprint, const FName EventName, const FComponentEventConstructionData EventData)
{
	// Find the corresponding variable property in the Blueprint
	FObjectProperty* VariableProperty = FindFProperty<FObjectProperty>(Blueprint->SkeletonGeneratedClass, EventData.VariableName);

	if (VariableProperty)
	{
		if (!FKismetEditorUtilities::FindBoundEventForComponent(Blueprint, EventName, VariableProperty->GetFName()))
		{
			FKismetEditorUtilities::CreateNewBoundEventForComponent(EventData.Component.Get(), EventName, Blueprint, VariableProperty);
		}
	}
}

void SSubobjectGenericGraphNodeEditor::ViewEvent(UBlueprint* Blueprint, const FName EventName, const FComponentEventConstructionData EventData)
{
	// Find the corresponding variable property in the Blueprint
	FObjectProperty* VariableProperty = FindFProperty<FObjectProperty>(Blueprint->SkeletonGeneratedClass, EventData.VariableName);

	if (VariableProperty)
	{
		const UK2Node_ComponentBoundEvent* ExistingNode = FKismetEditorUtilities::FindBoundEventForComponent(Blueprint, EventName, VariableProperty->GetFName());
		if (ExistingNode)
		{
			FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(ExistingNode);
		}
	}
}

#undef LOCTEXT_NAMESPACE

