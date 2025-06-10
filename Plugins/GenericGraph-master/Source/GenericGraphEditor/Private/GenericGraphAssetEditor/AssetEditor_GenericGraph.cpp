#include "GenericGraphAssetEditor/AssetEditor_GenericGraph.h"
#include "GenericGraphEditorPCH.h"
#include "GenericGraphAssetEditor/AssetEditorToolbar_GenericGraph.h"
#include "GenericGraphAssetEditor/AssetGraphSchema_GenericGraph.h"
#include "GenericGraphAssetEditor/EditorCommands_GenericGraph.h"
#include "GenericGraphAssetEditor/EdGraph_GenericGraph.h"
#include "GenericGraphAssetEditor/EdGraph_GenericGraph.h"
#include "GenericGraphAssetEditor/EdNode_GenericGraphNode.h"
#include "GenericGraphAssetEditor/SEdNode_GenericGraphNode.h"
#include "GenericGraphAssetEditor/EdNode_GenericGraphEdge.h"

#include "GenericGraphAssetEditor/SGenericComponentList.h"
#include "GenericSubobjectEditor/GenericSubobjectData.h"
#include "GenericSubobjectEditor/GenericSubobjectDataSubsystem.h"

#include "AssetToolsModule.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EdGraphUtilities.h"
#include "AutoLayout/TreeLayoutStrategy.h"
#include "AutoLayout/ForceDirectedLayoutStrategy.h"
#include "SKismetInspector.h"

#define LOCTEXT_NAMESPACE "AssetEditor_GenericGraph"

const FName GenericGraphEditorAppName = FName(TEXT("GenericGraphEditorApp"));

struct FGenericGraphAssetEditorTabs
{
	// Tab identifiers
	static const FName DetailsID;
	static const FName ViewportID;
	static const FName GenericGraphEditorSettingsID;
};

struct FGenericGraphUISelectionState
{
	// State Identifiers
	static const FName SelectionState_MyBlueprint;
	static const FName SelectionState_Components;
	static const FName SelectionState_Graph;
	static const FName SelectionState_ClassSettings;
	static const FName SelectionState_ClassDefaults;
};

//////////////////////////////////////////////////////////////////////////

const FName FGenericGraphAssetEditorTabs::DetailsID(TEXT("Details"));
const FName FGenericGraphAssetEditorTabs::ViewportID(TEXT("Viewport"));
const FName FGenericGraphAssetEditorTabs::GenericGraphEditorSettingsID(TEXT("GenericGraphEditorSettings"));

const FName FGenericGraphUISelectionState::SelectionState_MyBlueprint(TEXT("MyBlueprint"));
const FName FGenericGraphUISelectionState::SelectionState_Components(TEXT("Components"));
const FName FGenericGraphUISelectionState::SelectionState_Graph(TEXT("Graph"));
const FName FGenericGraphUISelectionState::SelectionState_ClassSettings(TEXT("ClassSettings"));
const FName FGenericGraphUISelectionState::SelectionState_ClassDefaults(TEXT("ClassDefaults"));

//////////////////////////////////////////////////////////////////////////

FAssetEditor_GenericGraph::FAssetEditor_GenericGraph()
	: bFlash(true)
{
	EditingGraph = nullptr;

	GenricGraphEditorSettings = NewObject<UGenericGraphEditorSettings>(UGenericGraphEditorSettings::StaticClass());

	DocumentManager = MakeShareable(new FDocumentTracker);

#if ENGINE_MAJOR_VERSION < 5
	OnPackageSavedDelegateHandle = UPackage::PackageSavedEvent.AddRaw(this, &FAssetEditor_GenericGraph::OnPackageSaved);
#else // #if ENGINE_MAJOR_VERSION < 5
	OnPackageSavedDelegateHandle = UPackage::PackageSavedWithContextEvent.AddRaw(this, &FAssetEditor_GenericGraph::OnPackageSavedWithContext);
#endif // #else // #if ENGINE_MAJOR_VERSION < 5
}

FAssetEditor_GenericGraph::~FAssetEditor_GenericGraph()
{
#if ENGINE_MAJOR_VERSION < 5
	UPackage::PackageSavedEvent.Remove(OnPackageSavedDelegateHandle);
#else // #if ENGINE_MAJOR_VERSION < 5
	UPackage::PackageSavedWithContextEvent.Remove(OnPackageSavedDelegateHandle);
#endif // #else // #if ENGINE_MAJOR_VERSION < 5
}

void FAssetEditor_GenericGraph::InitGenericGraphAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UGenericGraph* Graph)
{
	EditingGraph = Graph;
	CreateEdGraph();
	UGenericSubobjectDataSubsystem* System = UGenericSubobjectDataSubsystem::Get(); 
	System->OnSubobjectSelectedDelegate.BindSP(this, &FAssetEditor_GenericGraph::OnSelectionUpdated);

	FGenericCommands::Register();
	FGraphEditorCommands::Register();
	FEditorCommands_GenericGraph::Register();

	if (!ToolbarBuilder.IsValid())
	{
		ToolbarBuilder = MakeShareable(new FAssetEditorToolbar_GenericGraph(SharedThis(this)));
	}

	BindCommands();

	CreateInternalWidgets();

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);

	ToolbarBuilder->AddGenericGraphToolbar(ToolbarExtender);

	// Layout
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_GenericGraphEditor_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
#if ENGINE_MAJOR_VERSION < 5
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)->SetHideTabWell(true)
			)
#endif // #if ENGINE_MAJOR_VERSION < 5
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.65f)
					->AddTab(FGenericGraphAssetEditorTabs::ViewportID, ETabState::OpenedTab)->SetHideTabWell(true)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.7f)
						->AddTab(FGenericGraphAssetEditorTabs::DetailsID, ETabState::OpenedTab)->SetHideTabWell(true)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.3f)
						->AddTab(FGenericGraphAssetEditorTabs::GenericGraphEditorSettingsID, ETabState::OpenedTab)
					)
				)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, GenericGraphEditorAppName, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, EditingGraph, false);

	RegenerateMenusAndToolbars();
}

void FAssetEditor_GenericGraph::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	//@TODO: Can't we do this sooner?
	DocumentManager->SetTabManager(InTabManager);

	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_GenericGraphEditor", "Generic Graph Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FGenericGraphAssetEditorTabs::ViewportID, FOnSpawnTab::CreateSP(this, &FAssetEditor_GenericGraph::SpawnTab_Viewport))
		.SetDisplayName(LOCTEXT("GraphCanvasTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner(FGenericGraphAssetEditorTabs::DetailsID, FOnSpawnTab::CreateSP(this, &FAssetEditor_GenericGraph::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("DetailsTab", "Property"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FGenericGraphAssetEditorTabs::GenericGraphEditorSettingsID, FOnSpawnTab::CreateSP(this, &FAssetEditor_GenericGraph::SpawnTab_EditorSettings))
		.SetDisplayName(LOCTEXT("EditorSettingsTab", "Generic Graph Editor Setttings"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}

void FAssetEditor_GenericGraph::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(FGenericGraphAssetEditorTabs::ViewportID);
	InTabManager->UnregisterTabSpawner(FGenericGraphAssetEditorTabs::DetailsID);
	InTabManager->UnregisterTabSpawner(FGenericGraphAssetEditorTabs::GenericGraphEditorSettingsID);
}

FName FAssetEditor_GenericGraph::GetToolkitFName() const
{
	return FName("FGenericGraphEditor");
}

FText FAssetEditor_GenericGraph::GetBaseToolkitName() const
{
	return LOCTEXT("GenericGraphEditorAppLabel", "Generic Graph Editor");
}

FText FAssetEditor_GenericGraph::GetToolkitName() const
{
	const bool bDirtyState = EditingGraph->GetOutermost()->IsDirty();

	FFormatNamedArguments Args;
	Args.Add(TEXT("GenericGraphName"), FText::FromString(EditingGraph->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());
	return FText::Format(LOCTEXT("GenericGraphEditorToolkitName", "{GenericGraphName}{DirtyState}"), Args);
}

FText FAssetEditor_GenericGraph::GetToolkitToolTipText() const
{
	return FAssetEditorToolkit::GetToolTipTextForObject(EditingGraph);
}

FLinearColor FAssetEditor_GenericGraph::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

FString FAssetEditor_GenericGraph::GetWorldCentricTabPrefix() const
{
	return TEXT("GenericGraphEditor");
}

FString FAssetEditor_GenericGraph::GetDocumentationLink() const
{
	return TEXT("");
}

void FAssetEditor_GenericGraph::SaveAsset_Execute()
{
	if (EditingGraph != nullptr)
	{
		RebuildGenericGraph();
	}

	FAssetEditorToolkit::SaveAsset_Execute();
}

void FAssetEditor_GenericGraph::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EditingGraph);
	Collector.AddReferencedObject(EditingGraph->EdGraph);
}

UGenericGraphEditorSettings* FAssetEditor_GenericGraph::GetSettings() const
{
	return GenricGraphEditorSettings;
}

TSharedRef<SDockTab> FAssetEditor_GenericGraph::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FGenericGraphAssetEditorTabs::ViewportID);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("ViewportTab_Title", "Viewport"));

	if (ViewportWidget.IsValid())
	{
		SpawnedTab->SetContent(ViewportWidget.ToSharedRef());
	}

	return SpawnedTab;
}

TSharedRef<SDockTab> FAssetEditor_GenericGraph::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FGenericGraphAssetEditorTabs::DetailsID);

	return SNew(SDockTab)
#if ENGINE_MAJOR_VERSION < 5
		.Icon(FAppStyle::GetBrush("LevelEditor.Tabs.Details"))
#endif // #if ENGINE_MAJOR_VERSION < 5
		.Label(LOCTEXT("Details_Title", "Property"))
		[
			PropertyWidget.ToSharedRef()
		];
}

TSharedRef<SDockTab> FAssetEditor_GenericGraph::SpawnTab_EditorSettings(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FGenericGraphAssetEditorTabs::GenericGraphEditorSettingsID);

	return SNew(SDockTab)
#if ENGINE_MAJOR_VERSION < 5
		.Icon(FAppStyle::GetBrush("LevelEditor.Tabs.Details"))
#endif // #if ENGINE_MAJOR_VERSION < 5
		.Label(LOCTEXT("EditorSettings_Title", "Generic Graph Editor Setttings"))
		[
			EditorSettingsWidget.ToSharedRef()
		];
}

void FAssetEditor_GenericGraph::TryInvokingDetailsTab()
{
	if (TabManager->HasTabSpawner(FGenericGraphAssetEditorTabs::DetailsID))
	{
		TSharedPtr<SDockTab> BlueprintTab = FGlobalTabmanager::Get()->GetMajorTabForTabManager(TabManager.ToSharedRef());

		// We don't want to force this tab into existence when the blueprint editor isn't in the foreground and actively
		// being interacted with.  So we make sure the window it's in is focused and the tab is in the foreground.
		if (BlueprintTab.IsValid() && BlueprintTab->IsForeground())
		{
			TSharedPtr<SWindow> ParentWindow = BlueprintTab->GetParentWindow();
			if (ParentWindow.IsValid() && ParentWindow->HasFocusedDescendants())
			{
				if (!Inspector.IsValid() || !Inspector->GetOwnerTab().IsValid() || Inspector->GetOwnerTab()->GetDockArea().IsValid())
				{
					// Show the details panel if it doesn't exist.
					TabManager->TryInvokeTab(FGenericGraphAssetEditorTabs::DetailsID);

					if (bFlash)
					{
						TSharedPtr<SDockTab> OwnerTab = Inspector->GetOwnerTab();
						if (OwnerTab.IsValid())
						{
							OwnerTab->FlashTab();
						}
					}
				}
			}
		}
	}
}

void FAssetEditor_GenericGraph::CreateInternalWidgets()
{
	ViewportWidget = CreateViewportWidget();

	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.NotifyHook = this;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyWidget = PropertyModule.CreateDetailView(Args);
	PropertyWidget->SetObject(EditingGraph);
	PropertyWidget->OnFinishedChangingProperties().AddSP(this, &FAssetEditor_GenericGraph::OnFinishedChangingProperties);

	EditorSettingsWidget = PropertyModule.CreateDetailView(Args);
	EditorSettingsWidget->SetObject(GenricGraphEditorSettings);
}

TSharedRef<SGraphEditor> FAssetEditor_GenericGraph::CreateViewportWidget()
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText_GenericGraph", "Generic Graph");

	CreateCommandList();

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FAssetEditor_GenericGraph::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FAssetEditor_GenericGraph::OnNodeDoubleClicked);

	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.GraphToEdit(EditingGraph->EdGraph)
		.GraphEvents(InEvents)
		.AutoExpandActionMenu(true)
		.ShowGraphStateOverlay(false);
}

void FAssetEditor_GenericGraph::CreateEdGraph()
{
	if (EditingGraph->EdGraph == nullptr)
	{
		UEdGraph_GenericGraph* EdGraph = CastChecked<UEdGraph_GenericGraph>(FBlueprintEditorUtils::CreateNewGraph(EditingGraph, NAME_None, UEdGraph_GenericGraph::StaticClass(), UAssetGraphSchema_GenericGraph::StaticClass()));
		EdGraph->bAllowDeletion = false;
		EditingGraph->EdGraph = EdGraph;

		// Give the schema a chance to fill out any required nodes (like the results node)
		const UEdGraphSchema* Schema = EditingGraph->EdGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*EditingGraph->EdGraph);
	}
}

TSharedPtr<SGraphEditor> FAssetEditor_GenericGraph::GetCurrGraphEditor() const
{
	return ViewportWidget;
}

FGraphPanelSelectionSet FAssetEditor_GenericGraph::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	TSharedPtr<SGraphEditor> FocusedGraphEd = GetCurrGraphEditor();
	if (FocusedGraphEd.IsValid())
	{
		CurrentSelection = FocusedGraphEd->GetSelectedNodes();
	}

	return CurrentSelection;
}

void FAssetEditor_GenericGraph::RebuildGenericGraph()
{
	if (EditingGraph == nullptr)
	{
		LOG_WARNING(TEXT("FGenericGraphAssetEditor::RebuildGenericGraph EditingGraph is nullptr"));
		return;
	}

	UEdGraph_GenericGraph* EdGraph = Cast<UEdGraph_GenericGraph>(EditingGraph->EdGraph);
	check(EdGraph != nullptr);

	EdGraph->RebuildGenericGraph();
}

/////////////////////////// Begin Command Events ///////////////////////////

void FAssetEditor_GenericGraph::BindCommands()
{
	ToolkitCommands->MapAction(FEditorCommands_GenericGraph::Get().GraphSettings,
		FExecuteAction::CreateSP(this, &FAssetEditor_GenericGraph::GraphSettings),
		FCanExecuteAction::CreateSP(this, &FAssetEditor_GenericGraph::CanGraphSettings)
	);

	ToolkitCommands->MapAction(FEditorCommands_GenericGraph::Get().AutoArrange,
		FExecuteAction::CreateSP(this, &FAssetEditor_GenericGraph::AutoArrange),
		FCanExecuteAction::CreateSP(this, &FAssetEditor_GenericGraph::CanAutoArrange)
	);
}

void FAssetEditor_GenericGraph::CreateCommandList()
{
	if (GraphEditorCommands.IsValid())
	{
		return;
	}

	GraphEditorCommands = MakeShareable(new FUICommandList);

	// Can't use CreateSP here because derived editor are already implementing TSharedFromThis<FAssetEditorToolkit>
	// however it should be safe, since commands are being used only within this editor
	// if it ever crashes, this function will have to go away and be reimplemented in each derived class

	GraphEditorCommands->MapAction(FEditorCommands_GenericGraph::Get().GraphSettings,
		FExecuteAction::CreateRaw(this, &FAssetEditor_GenericGraph::GraphSettings),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_GenericGraph::CanGraphSettings));

	GraphEditorCommands->MapAction(FEditorCommands_GenericGraph::Get().AutoArrange,
		FExecuteAction::CreateRaw(this, &FAssetEditor_GenericGraph::AutoArrange),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_GenericGraph::CanAutoArrange));

	// Editing commands
	GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FAssetEditor_GenericGraph::SelectAllNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_GenericGraph::CanSelectAllNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FAssetEditor_GenericGraph::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_GenericGraph::CanDeleteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FAssetEditor_GenericGraph::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_GenericGraph::CanCopyNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FAssetEditor_GenericGraph::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_GenericGraph::CanCutNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FAssetEditor_GenericGraph::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_GenericGraph::CanPasteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FAssetEditor_GenericGraph::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_GenericGraph::CanDuplicateNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Rename,
		FExecuteAction::CreateSP(this, &FAssetEditor_GenericGraph::OnRenameNode),
		FCanExecuteAction::CreateSP(this, &FAssetEditor_GenericGraph::CanRenameNodes)
	);
}

void FAssetEditor_GenericGraph::SelectAllNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (CurrentGraphEditor.IsValid())
	{
		CurrentGraphEditor->SelectAllNodes();
	}
}

bool FAssetEditor_GenericGraph::CanSelectAllNodes()
{
	return true;
}

void FAssetEditor_GenericGraph::DeleteSelectedNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());

	CurrentGraphEditor->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* EdNode = Cast<UEdGraphNode>(*NodeIt);
		if (EdNode == nullptr || !EdNode->CanUserDeleteNode())
			continue;;

		if (UEdNode_GenericGraphNode* EdNode_Node = Cast<UEdNode_GenericGraphNode>(EdNode))
		{
			EdNode_Node->Modify();

			const UEdGraphSchema* Schema = EdNode_Node->GetSchema();
			if (Schema != nullptr)
			{
				Schema->BreakNodeLinks(*EdNode_Node);
			}

			EdNode_Node->DestroyNode();
		}
		else
		{
			EdNode->Modify();
			EdNode->DestroyNode();
		}
	}
}

bool FAssetEditor_GenericGraph::CanDeleteNodes()
{
	// If any of the nodes can be deleted then we should allow deleting
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node != nullptr && Node->CanUserDeleteNode())
		{
			return true;
		}
	}

	return false;
}

void FAssetEditor_GenericGraph::DeleteSelectedDuplicatableNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FGraphPanelSelectionSet OldSelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}

	// Delete the duplicatable nodes
	DeleteSelectedNodes();

	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}
}

void FAssetEditor_GenericGraph::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedDuplicatableNodes();
}

bool FAssetEditor_GenericGraph::CanCutNodes()
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FAssetEditor_GenericGraph::CopySelectedNodes()
{
	// Export the selected nodes and place the text on the clipboard
	FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	FString ExportedText;

	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node == nullptr)
		{
			SelectedIter.RemoveCurrent();
			continue;
		}

		if (UEdNode_GenericGraphEdge* EdNode_Edge = Cast<UEdNode_GenericGraphEdge>(*SelectedIter))
		{
			UEdNode_GenericGraphNode* StartNode = EdNode_Edge->GetStartNode();
			UEdNode_GenericGraphNode* EndNode = EdNode_Edge->GetEndNode();

			if (!SelectedNodes.Contains(StartNode) || !SelectedNodes.Contains(EndNode))
			{
				SelectedIter.RemoveCurrent();
				continue;
			}
		}

		Node->PrepareForCopying();
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool FAssetEditor_GenericGraph::CanCopyNodes()
{
	// If any of the nodes can be duplicated then we should allow copying
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			return true;
		}
	}

	return false;
}

void FAssetEditor_GenericGraph::PasteNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (CurrentGraphEditor.IsValid())
	{
		PasteNodesHere(CurrentGraphEditor->GetPasteLocation());
	}
}

void FAssetEditor_GenericGraph::PasteNodesHere(const FVector2D& Location)
{
	// Find the graph editor with focus
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}
	// Select the newly pasted stuff
	UEdGraph* EdGraph = CurrentGraphEditor->GetCurrentGraph();

	{
		const FScopedTransaction Transaction(FGenericCommands::Get().Paste->GetDescription());
		EdGraph->Modify();

		// Clear the selection set (newly pasted stuff will be selected)
		CurrentGraphEditor->ClearSelectionSet();

		// Grab the text to paste from the clipboard.
		FString TextToImport;
		FPlatformApplicationMisc::ClipboardPaste(TextToImport);

		// Import the nodes
		TSet<UEdGraphNode*> PastedNodes;
		FEdGraphUtilities::ImportNodesFromText(EdGraph, TextToImport, PastedNodes);

		//Average position of nodes so we can move them while still maintaining relative distances to each other
		FVector2D AvgNodePosition(0.0f, 0.0f);

		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			UEdGraphNode* Node = *It;
			AvgNodePosition.X += Node->NodePosX;
			AvgNodePosition.Y += Node->NodePosY;
		}

		float InvNumNodes = 1.0f / float(PastedNodes.Num());
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;

		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			UEdGraphNode* Node = *It;
			CurrentGraphEditor->SetNodeSelection(Node, true);

			Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
			Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;

			Node->SnapToGrid(16);

			// Give new node a different Guid from the old one
			Node->CreateNewGuid();
		}
	}

	// Update UI
	CurrentGraphEditor->NotifyGraphChanged();

	UObject* GraphOwner = EdGraph->GetOuter();
	if (GraphOwner)
	{
		GraphOwner->PostEditChange();
		GraphOwner->MarkPackageDirty();
	}
}

bool FAssetEditor_GenericGraph::CanPasteNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid())
	{
		return false;
	}

	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(CurrentGraphEditor->GetCurrentGraph(), ClipboardContent);
}

void FAssetEditor_GenericGraph::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FAssetEditor_GenericGraph::CanDuplicateNodes()
{
	return CanCopyNodes();
}

void FAssetEditor_GenericGraph::GraphSettings()
{
	PropertyWidget->SetObject(EditingGraph);
}

bool FAssetEditor_GenericGraph::CanGraphSettings() const
{
	return true;
}

void FAssetEditor_GenericGraph::AutoArrange()
{
	UEdGraph_GenericGraph* EdGraph = Cast<UEdGraph_GenericGraph>(EditingGraph->EdGraph);
	check(EdGraph != nullptr);

	const FScopedTransaction Transaction(LOCTEXT("GenericGraphEditorAutoArrange", "Generic Graph Editor: Auto Arrange"));

	EdGraph->Modify();

	UAutoLayoutStrategy* LayoutStrategy = nullptr;
	switch (GenricGraphEditorSettings->AutoLayoutStrategy)
	{
	case EAutoLayoutStrategy::Tree:
		LayoutStrategy = NewObject<UAutoLayoutStrategy>(EdGraph, UTreeLayoutStrategy::StaticClass());
		break;
	case EAutoLayoutStrategy::ForceDirected:
		LayoutStrategy = NewObject<UAutoLayoutStrategy>(EdGraph, UForceDirectedLayoutStrategy::StaticClass());
		break;
	default:
		break;
	}

	if (LayoutStrategy != nullptr)
	{
		LayoutStrategy->Settings = GenricGraphEditorSettings;
		LayoutStrategy->Layout(EdGraph);
		LayoutStrategy->ConditionalBeginDestroy();
	}
	else
	{
		LOG_ERROR(TEXT("FAssetEditor_GenericGraph::AutoArrange LayoutStrategy is null."));
	}
}

bool FAssetEditor_GenericGraph::CanAutoArrange() const
{
	return EditingGraph != nullptr && Cast<UEdGraph_GenericGraph>(EditingGraph->EdGraph) != nullptr;
}

void FAssetEditor_GenericGraph::OnRenameNode()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (CurrentGraphEditor.IsValid())
	{
		const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
		for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
		{
			UEdGraphNode* SelectedNode = Cast<UEdGraphNode>(*NodeIt);
			if (SelectedNode != NULL && SelectedNode->bCanRenameNode)
			{
				CurrentGraphEditor->IsNodeTitleVisible(SelectedNode, true);
				break;
			}
		}
	}
}

bool FAssetEditor_GenericGraph::CanRenameNodes() const
{
	UEdGraph_GenericGraph* EdGraph = Cast<UEdGraph_GenericGraph>(EditingGraph->EdGraph);
	check(EdGraph != nullptr);

	UGenericGraph* Graph = EdGraph->GetGenericGraph();
	check(Graph != nullptr)

	return Graph->bCanRenameNode && GetSelectedNodes().Num() == 1;
}

/////////////////////////// End Command Events ///////////////////////////

void FAssetEditor_GenericGraph::SetUISelectionState(FName SelectionOwner)
{
	if (SelectionOwner != CurrentUISelection)
	{
		ClearSelectionStateFor(CurrentUISelection);

		CurrentUISelection = SelectionOwner;
	}
}

void FAssetEditor_GenericGraph::ClearSelectionStateFor(FName SelectionOwner)
{
	if (SelectionOwner == FGenericGraphUISelectionState::SelectionState_Graph)
	{
		TArray< TSharedPtr<SDockTab> > GraphEditorTabs;
		DocumentManager->FindAllTabsForFactory(GraphEditorTabFactoryPtr, /*out*/ GraphEditorTabs);

		for (TSharedPtr<SDockTab>& GraphEditorTab : GraphEditorTabs)
		{
			TSharedRef<SGraphEditor> Editor = StaticCastSharedRef<SGraphEditor>((GraphEditorTab)->GetContent());

			Editor->ClearSelectionSet();
		}
	}
	else if (SelectionOwner == FGenericGraphUISelectionState::SelectionState_Components)
	{
		/*if (SubobjectEditor.IsValid())
		{
			SubobjectEditor->ClearSelection();
		}*/
	}
	else if (SelectionOwner == FGenericGraphUISelectionState::SelectionState_MyBlueprint)
	{
		/*if (MyBlueprintWidget.IsValid())
		{
			MyBlueprintWidget->ClearGraphActionMenuSelection();
		}*/
	}
}

void FAssetEditor_GenericGraph::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	TArray<UObject*> Selection;

	for (UObject* SelectionEntry : NewSelection)
	{
		Selection.Add(SelectionEntry);
	}

	if (Selection.Num() == 0) 
	{
		PropertyWidget->SetObject(EditingGraph);

	}
	else
	{
		PropertyWidget->SetObjects(Selection);
	}

	/////////////////////////// BlueprintEditor::OnSelectedNodesChanged ///////////////////////////

	if (NewSelection.Num() > 0)
	{
		SetUISelectionState(FGenericGraphUISelectionState::SelectionState_Graph);
	}

	/* SKismetInspector::FShowDetailsOptions DetailsOptions;
	DetailsOptions.bForceRefresh = true;
	Inspector->ShowDetailsForObjects(NewSelection.Array(), DetailsOptions);

	bSelectRegularNode = false;
	for (FGraphPanelSelectionSet::TConstIterator It(NewSelection); It; ++It)
	{
		UEdGraphNode_Comment* SeqNode = Cast<UEdGraphNode_Comment>(*It);
		if (!SeqNode)
		{
			bSelectRegularNode = true;
			break;
		}
	}

	if (bHideUnrelatedNodes && !bLockNodeFadeState)
	{
		ResetAllNodesUnrelatedStates();

		if (bSelectRegularNode)
		{
			HideUnrelatedNodes();
		}
	} */
}

void FAssetEditor_GenericGraph::OnSelectionUpdated(const TArray<FGenericSubobjectDataHandle>& SelectedNodes)
{
	/*if (SubobjectViewport.IsValid())
	{
		SubobjectViewport->OnComponentSelectionChanged();
	}*/

	LOG_INFO(TEXT("Asset Editor: Recieved Component Selected!"));

	if (PropertyWidget.IsValid())
	{
		// Clear the my blueprints selection
		if (SelectedNodes.Num() > 0)
		{
			SetUISelectionState(FGenericGraphUISelectionState::SelectionState_Components);
		}

		// Convert the selection set to an array of UObject* pointers
		FText InspectorTitle = FText::GetEmpty();
		TArray<UObject*> InspectorObjects;
		bool bShowComponents = true;
		InspectorObjects.Empty(SelectedNodes.Num());
		for (FGenericSubobjectDataHandle NodePtr : SelectedNodes)
		{
			const FGenericSubobjectData* NodeData = NodePtr.IsValid() ? NodePtr.GetSharedDataPtr().Get() : nullptr;
			if (NodeData)
			{
				/*if (const AActor* Actor = NodeData->GetObject<AActor>())
				{
					if (const AActor* DefaultActor = NodeData->GetObjectForBlueprint<AActor>(GetBlueprintObj()))
					{
						InspectorObjects.Add(const_cast<AActor*>(DefaultActor));

						FString Title;
						DefaultActor->GetName(Title);
						InspectorTitle = FText::FromString(Title);
						bShowComponents = false;

						TryInvokingDetailsTab();
					}
				}
				else
				{*/
					const UGenericGraphNodeComponent* EditableComponent = NodeData->GetObject<UGenericGraphNodeComponent>(true);
					if (EditableComponent)
					{
						InspectorTitle = FText::FromString(NodeData->GetDisplayString());
						InspectorObjects.Add(const_cast<UGenericGraphNodeComponent*>(EditableComponent));
					}
				// }
			}
		}

		// Update the details panel
		SKismetInspector::FShowDetailsOptions Options(InspectorTitle, true);
		Options.bShowComponents = bShowComponents;
		PropertyWidget->SetObjects(InspectorObjects);
	} // */
}

void FAssetEditor_GenericGraph::OnNodeDoubleClicked(UEdGraphNode* Node)
{
	
}

void FAssetEditor_GenericGraph::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (EditingGraph == nullptr)
		return;

	EditingGraph->EdGraph->GetSchema()->ForceVisualizationCacheClear();
}

void FAssetEditor_GenericGraph::RefreshEditors()
{
	bool bForceFocusOnSelectedNodes = false;

	if (CurrentUISelection == FGenericGraphUISelectionState::SelectionState_MyBlueprint)
	{
		// Handled below, here to avoid tripping the ensure
	}
	else if (CurrentUISelection == FGenericGraphUISelectionState::SelectionState_Components)
	{
		/*if (SubobjectEditor.IsValid())
		{
			SubobjectEditor->RefreshSelectionDetails();
		}*/
	}
	else if (CurrentUISelection == FGenericGraphUISelectionState::SelectionState_Graph)
	{
		bForceFocusOnSelectedNodes = true;
	}
	else if (CurrentUISelection == FGenericGraphUISelectionState::SelectionState_ClassSettings)
	{
		// No need for a refresh, the Blueprint object didn't change
	}
	else if (CurrentUISelection == FGenericGraphUISelectionState::SelectionState_ClassDefaults)
	{
		// StartEditingDefaults(/*bAutoFocus=*/ false, true);
	}

	// Remove any tabs are that are pending kill or otherwise invalid UObject pointers.
	DocumentManager->CleanInvalidTabs();

	//@TODO: Should determine when we need to do the invalid/refresh business and if the graph node selection change
	// under non-compiles is necessary (except when the selection mode is appropriate, as already detected above)
	//if (Reason != ERefreshBlueprintEditorReason::BlueprintCompiled)
	//{
	//	DocumentManager->RefreshAllTabs();

	//	bForceFocusOnSelectedNodes = true;
	//}

	//if (bForceFocusOnSelectedNodes)
	//{
	//	FocusInspectorOnGraphSelection(GetSelectedNodes(), /*bForceRefresh=*/ true);
	//}

	//if (ReplaceReferencesWidget.IsValid())
	//{
	//	ReplaceReferencesWidget->Refresh();
	//}

	//if (MyBlueprintWidget.IsValid())
	//{
	//	MyBlueprintWidget->Refresh();
	//}

	//if (SubobjectEditor.IsValid())
	//{
	//	SubobjectEditor->RefreshComponentTypesList();
	//	SubobjectEditor->UpdateTree();

	//	// Note: Don't pass 'true' here because we don't want the preview actor to be reconstructed until after Blueprint modification is complete.
	//	UpdateSubobjectPreview();
	//}
}

#if ENGINE_MAJOR_VERSION < 5
void FAssetEditor_GenericGraph::OnPackageSaved(const FString& PackageFileName, UObject* Outer)
{
	RebuildGenericGraph();
}
#else // #if ENGINE_MAJOR_VERSION < 5
void FAssetEditor_GenericGraph::OnPackageSavedWithContext(const FString& PackageFileName, UPackage* Package, FObjectPostSaveContext ObjectSaveContext)
{
	RebuildGenericGraph();
}
#endif // #else // #if ENGINE_MAJOR_VERSION < 5

void FAssetEditor_GenericGraph::RegisterToolbarTab(const TSharedRef<class FTabManager>& InTabManager) 
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}


#undef LOCTEXT_NAMESPACE

