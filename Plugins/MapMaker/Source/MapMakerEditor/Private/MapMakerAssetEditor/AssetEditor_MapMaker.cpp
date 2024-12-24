// Fill out your copyright notice in the Description page of Project Settings.


#include "MapMakerAssetEditor/AssetEditor_MapMaker.h"
#include "MapMakerAssetEditor/SAssetSelector_MapMaker.h"
#include "MapMakerAssetEditor/SMapMakerPreviewTabBody.h"
#include "MapMakerAssetEditor/FPreviewScene_MapMaker.h"
#include "MapMakerAssetEditor/AssetGraphSchema_MapMaker.h"
#include "MapMakerAssetEditor/EdGraph_MapMaker.h"
#include "MapMakerAssetEditor/EdNode_MapMakerNode.h"
#include "MapMakerAssetEditor/EdNode_MapMakerEdge.h"
#include "AssetEditor_MapMaker.h"
#include "MapMakerEditorPCH.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Framework/Commands/GenericCommands.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"
#include "MapMakerAssetDatabase.h"

#define LOCTEXT_NAMESPACE "AssetEditor_MapMaker"

const FName MapMakerEditorAppName = FName(TEXT("MapMakerEditorApp"));

struct FMapMakerAssetEditorTabs
{
	// Tab identifiers
	static const FName MapMakerPropertyID;
	static const FName ViewportID;
	static const FName MapMakerAssetsID;
	static const FName PreviewID;
};

//////////////////////////////////////////////////////////////////////////

const FName FMapMakerAssetEditorTabs::MapMakerPropertyID(TEXT("MapMakerProperty"));
const FName FMapMakerAssetEditorTabs::ViewportID(TEXT("Viewport"));
const FName FMapMakerAssetEditorTabs::MapMakerAssetsID(TEXT("MapMakerAssets"));
const FName FMapMakerAssetEditorTabs::PreviewID(TEXT("Preview"));

//////////////////////////////////////////////////////////////////////////

FAssetEditor_MapMaker::FAssetEditor_MapMaker()
	: PreviewScene()
{
	EditingGraph = nullptr;

	OnPackageSavedDelegateHandle = UPackage::PackageSavedWithContextEvent.AddRaw(this, &FAssetEditor_MapMaker::OnPackageSavedWithContext);
}

FAssetEditor_MapMaker::~FAssetEditor_MapMaker()
{
	UPackage::PackageSavedWithContextEvent.Remove(OnPackageSavedDelegateHandle);
}

void FAssetEditor_MapMaker::InitMapMakerAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UMapMaker* Graph)
{
	EditingGraph = Graph;
	CreateEdGraph();
	CreateEdAssets();

	/*FGenericCommands::Register();
	FGraphEditorCommands::Register();
	FEditorCommands_MapMaker::Register();

	BindCommands();*/
	CreateInternalWidgets();

	// Layout
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_MapMakerEditor_Layout_v1")
		->AddArea(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(0.9f)
				->Split(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->Split(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.65f)
						->AddTab(FMapMakerAssetEditorTabs::PreviewID, ETabState::OpenedTab)->SetHideTabWell(true)
					)
					->Split(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.65f)
						->AddTab(FMapMakerAssetEditorTabs::ViewportID, ETabState::OpenedTab)->SetHideTabWell(true)
					)
				)
				->Split(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.7f)
						->AddTab(FMapMakerAssetEditorTabs::MapMakerPropertyID, ETabState::OpenedTab)->SetHideTabWell(true)
					)
					->Split(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.3f)
						->AddTab(FMapMakerAssetEditorTabs::MapMakerAssetsID, ETabState::OpenedTab)->SetHideTabWell(true)
					)
				)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, MapMakerEditorAppName, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, EditingGraph, false);

	RegenerateMenusAndToolbars();
}

void FAssetEditor_MapMaker::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_MapMakerEditor", "Map Maker Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FMapMakerAssetEditorTabs::ViewportID, FOnSpawnTab::CreateSP(this, &FAssetEditor_MapMaker::SpawnTab_Viewport))
		.SetDisplayName(LOCTEXT("GraphCanvasTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner(FMapMakerAssetEditorTabs::MapMakerPropertyID, FOnSpawnTab::CreateSP(this, &FAssetEditor_MapMaker::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("DetailsTab", "Property"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FMapMakerAssetEditorTabs::MapMakerAssetsID, FOnSpawnTab::CreateSP(this, &FAssetEditor_MapMaker::SpawnTab_Assets))
		.SetDisplayName(LOCTEXT("MapMakerAssetsTab", "Map Maker Editor Assets"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FMapMakerAssetEditorTabs::PreviewID, FOnSpawnTab::CreateSP(this, &FAssetEditor_MapMaker::SpawnTab_Preview))
		.SetDisplayName(LOCTEXT("MapMakerPreviewTab", "Preview"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Kismet.Tabs.Components"));
}

void FAssetEditor_MapMaker::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(FMapMakerAssetEditorTabs::ViewportID);
	InTabManager->UnregisterTabSpawner(FMapMakerAssetEditorTabs::MapMakerPropertyID);
	InTabManager->UnregisterTabSpawner(FMapMakerAssetEditorTabs::MapMakerAssetsID);
	InTabManager->UnregisterTabSpawner(FMapMakerAssetEditorTabs::PreviewID);
}

FName FAssetEditor_MapMaker::GetToolkitFName() const
{
	return FName("FMapMakerEditor");
}

FText FAssetEditor_MapMaker::GetBaseToolkitName() const
{
	return LOCTEXT("MapMakerEditorAppLabel", "Map Maker Editor");
}

FText FAssetEditor_MapMaker::GetToolkitName() const
{
	const bool bDirtyState = EditingGraph->GetOutermost()->IsDirty();

	FFormatNamedArguments Args;
	Args.Add(TEXT("MapMakerName"), FText::FromString(EditingGraph->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());
	return FText::Format(LOCTEXT("MapMakerEditorToolkitName", "{MapMakerName}{DirtyState}"), Args);
}

FText FAssetEditor_MapMaker::GetToolkitToolTipText() const
{
	return FAssetEditorToolkit::GetToolTipTextForObject(EditingGraph);
}

FLinearColor FAssetEditor_MapMaker::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

FString FAssetEditor_MapMaker::GetWorldCentricTabPrefix() const
{
	return TEXT("MapMakerEditor");
}

FString FAssetEditor_MapMaker::GetDocumentationLink() const
{
	return TEXT("");
}

void FAssetEditor_MapMaker::SaveAsset_Execute()
{
	if (EditingGraph != nullptr)
	{
		RebuildMapMaker();
	}

	FAssetEditorToolkit::SaveAsset_Execute();
}

void FAssetEditor_MapMaker::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EditingGraph);
	Collector.AddReferencedObject(EditingGraph->EdGraph);
}

void FAssetEditor_MapMaker::CreateInternalWidgets()
{
	PreviewWidget = CreatePreviewWidget();
	ViewportWidget = CreateViewportWidget();

	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.NotifyHook = this;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyWidget = PropertyModule.CreateDetailView(Args);
	PropertyWidget->SetObject(EditingGraph);
	PropertyWidget->OnFinishedChangingProperties().AddSP(this, &FAssetEditor_MapMaker::OnFinishedChangingProperties);

	AssetsWidget = CreateAssetSelectionWidget();
}

TSharedRef<SGraphEditor> FAssetEditor_MapMaker::CreateViewportWidget()
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText_MapMaker", "Map Maker");

	CreateCommandList();

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FAssetEditor_MapMaker::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FAssetEditor_MapMaker::OnNodeDoubleClicked);

	return SNew(SGraphEditor)
		.AdditionalCommands(MapEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.GraphToEdit(EditingGraph->EdGraph)
		.GraphEvents(InEvents)
		.AutoExpandActionMenu(true)
		.ShowGraphStateOverlay(false);
}

TSharedRef<SCompoundWidget> FAssetEditor_MapMaker::CreatePreviewWidget()
{
	return SNew(SMapMakerViewportTabBody, EditorModeManager.Get(), PreviewScene);
}

TSharedRef<SCompoundWidget> FAssetEditor_MapMaker::CreateAssetSelectionWidget()
{
	return SNew(SAssetSelector_MapMaker)
		.GraphToEdit(EditingGraph->EdGraph)
		.AssetDatabase(EditingGraph->AssetDatabase);
}

void FAssetEditor_MapMaker::CreateEdGraph()
{
	if (EditingGraph->EdGraph == nullptr)
	{
		EditingGraph->EdGraph = CastChecked<UEdGraph_MapMaker>(FBlueprintEditorUtils::CreateNewGraph(EditingGraph, NAME_None, UEdGraph_MapMaker::StaticClass(), UAssetGraphSchema_MapMaker::StaticClass()));
		EditingGraph->EdGraph->bAllowDeletion = false;

		// Give the schema a chance to fill out any required nodes (like the results node)
		const UEdGraphSchema* Schema = EditingGraph->EdGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*EditingGraph->EdGraph);
	}
}

void FAssetEditor_MapMaker::CreateEdAssets()
{
	if (EditingGraph->AssetDatabase == nullptr)
	{
		EditingGraph->AssetDatabase = NewObject<UMapMakerAssetDatabase>();
	}
}

void FAssetEditor_MapMaker::CreateCommandList()
{
	if (MapEditorCommands.IsValid())
	{
		return;
	}

	MapEditorCommands = MakeShareable(new FUICommandList);

	// Can't use CreateSP here because derived editor are already implementing TSharedFromThis<FAssetEditorToolkit>
	// however it should be safe, since commands are being used only within this editor
	// if it ever crashes, this function will have to go away and be reimplemented in each derived class

	/*MapEditorCommands->MapAction(FEditorCommands_MapMaker::Get().GraphSettings,
		FExecuteAction::CreateRaw(this, &FAssetEditor_MapMaker::GraphSettings),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_MapMaker::CanGraphSettings));

	MapEditorCommands->MapAction(FEditorCommands_MapMaker::Get().AutoArrange,
		FExecuteAction::CreateRaw(this, &FAssetEditor_MapMaker::AutoArrange),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_MapMaker::CanAutoArrange));*/

	MapEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FAssetEditor_MapMaker::SelectAllNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_MapMaker::CanSelectAllNodes)
	);

	MapEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FAssetEditor_MapMaker::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_MapMaker::CanDeleteNodes)
	);

	MapEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FAssetEditor_MapMaker::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_MapMaker::CanCopyNodes)
	);

	MapEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FAssetEditor_MapMaker::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_MapMaker::CanCutNodes)
	);

	MapEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FAssetEditor_MapMaker::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_MapMaker::CanPasteNodes)
	);

	MapEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FAssetEditor_MapMaker::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_MapMaker::CanDuplicateNodes)
	);

	MapEditorCommands->MapAction(FGenericCommands::Get().Rename,
		FExecuteAction::CreateSP(this, &FAssetEditor_MapMaker::OnRenameNode),
		FCanExecuteAction::CreateSP(this, &FAssetEditor_MapMaker::CanRenameNodes)
	);
}

TSharedPtr<SGraphEditor> FAssetEditor_MapMaker::GetCurrGraphEditor() const
{
	return ViewportWidget;
}

FGraphPanelSelectionSet FAssetEditor_MapMaker::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	TSharedPtr<SGraphEditor> FocusedGraphEd = GetCurrGraphEditor();
	if (FocusedGraphEd.IsValid())
	{
		CurrentSelection = FocusedGraphEd->GetSelectedNodes();
	}

	return CurrentSelection;
}

void FAssetEditor_MapMaker::RebuildMapMaker()
{
	if (EditingGraph == nullptr)
	{
		LOG_WARNING(TEXT("FMapMakerAssetEditor::RebuildMapMaker EditingGraph is nullptr"));
		return;
	}

	UEdGraph_MapMaker* EdGraph = Cast<UEdGraph_MapMaker>(EditingGraph->EdGraph);
	check(EdGraph != nullptr);

	EdGraph->RebuildMapMaker();
}

void FAssetEditor_MapMaker::SelectAllNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (CurrentGraphEditor.IsValid())
	{
		CurrentGraphEditor->SelectAllNodes();
	}
}

bool FAssetEditor_MapMaker::CanSelectAllNodes()
{
	return true;
}

void FAssetEditor_MapMaker::DeleteSelectedNodes()
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

		if (UEdNode_MapMakerNode* EdNode_Node = Cast<UEdNode_MapMakerNode>(EdNode))
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

bool FAssetEditor_MapMaker::CanDeleteNodes()
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

void FAssetEditor_MapMaker::DeleteSelectedDuplicatableNodes()
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

void FAssetEditor_MapMaker::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedDuplicatableNodes();
}

bool FAssetEditor_MapMaker::CanCutNodes()
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FAssetEditor_MapMaker::CopySelectedNodes()
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

		if (UEdNode_MapMakerEdge* EdNode_Edge = Cast<UEdNode_MapMakerEdge>(*SelectedIter))
		{
			UEdNode_MapMakerNode* StartNode = EdNode_Edge->GetStartNode();
			UEdNode_MapMakerNode* EndNode = EdNode_Edge->GetEndNode();

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

bool FAssetEditor_MapMaker::CanCopyNodes()
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

void FAssetEditor_MapMaker::PasteNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (CurrentGraphEditor.IsValid())
	{
		PasteNodesHere(CurrentGraphEditor->GetPasteLocation());
	}
}

void FAssetEditor_MapMaker::PasteNodesHere(const FVector2D& Location)
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

bool FAssetEditor_MapMaker::CanPasteNodes()
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

void FAssetEditor_MapMaker::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FAssetEditor_MapMaker::CanDuplicateNodes()
{
	return CanCopyNodes();
}

void FAssetEditor_MapMaker::OnRenameNode()
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

bool FAssetEditor_MapMaker::CanRenameNodes() const
{
	UEdGraph_MapMaker* EdGraph = Cast<UEdGraph_MapMaker>(EditingGraph->EdGraph);
	check(EdGraph != nullptr);

	UMapMaker* Graph = EdGraph->GetMapMaker();
	check(Graph != nullptr)

		return Graph->bCanRenameNode && GetSelectedNodes().Num() == 1;
}

void FAssetEditor_MapMaker::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
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
}

void FAssetEditor_MapMaker::OnNodeDoubleClicked(UEdGraphNode* Node)
{

}

void FAssetEditor_MapMaker::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (EditingGraph == nullptr)
		return;

	EditingGraph->EdGraph->GetSchema()->ForceVisualizationCacheClear();
}

TSharedRef<SDockTab> FAssetEditor_MapMaker::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FMapMakerAssetEditorTabs::ViewportID);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("ViewportTab_Title", "Viewport"));

	if (ViewportWidget.IsValid())
	{
		SpawnedTab->SetContent(ViewportWidget.ToSharedRef());
	}

	return SpawnedTab;
}

TSharedRef<SDockTab> FAssetEditor_MapMaker::SpawnTab_Preview(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FMapMakerAssetEditorTabs::PreviewID);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("PreviewTab_Title", "Preview"));

	if (PreviewWidget.IsValid())
	{
		SpawnedTab->SetContent(PreviewWidget.ToSharedRef());
	}

	return SpawnedTab;
}

TSharedRef<SDockTab> FAssetEditor_MapMaker::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FMapMakerAssetEditorTabs::MapMakerPropertyID);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("Details_Title", "Property"));

	if (PropertyWidget.IsValid())
	{
		SpawnedTab->SetContent(PropertyWidget.ToSharedRef());
	}

	return SpawnedTab;
}
TSharedRef<SDockTab> FAssetEditor_MapMaker::SpawnTab_Assets(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FMapMakerAssetEditorTabs::MapMakerAssetsID);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("Details_Title", "Assets"));

	if (AssetsWidget.IsValid())
	{
		SpawnedTab->SetContent(AssetsWidget.ToSharedRef());
	}

	return SpawnedTab;
}
void FAssetEditor_MapMaker::OnPackageSavedWithContext(const FString& PackageFileName, UPackage* Package, FObjectPostSaveContext ObjectSaveContext)
{
	RebuildMapMaker();
}



#undef LOCTEXT_NAMESPACE

