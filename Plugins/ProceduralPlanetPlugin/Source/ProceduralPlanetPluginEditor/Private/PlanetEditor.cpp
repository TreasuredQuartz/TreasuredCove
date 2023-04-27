// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetEditor.h"
#include "PlanetGraph.h"
#include "SPlanetEditorViewport.h"

#include "ProceduralPlanet.h"

#include "SlateOptMacros.h"
#include "Styling/AppStyle.h"

#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "BlueprintEditorUtils.h"

#include "Widgets/Docking/SDockTab.h"


#define LOCTEXT_NAMESPACE "PlanetEditor"
DEFINE_LOG_CATEGORY_STATIC(LogPlanetEditor, Log, All);

const FName FPlanetEditor::PreviewTabId(TEXT("PlanetEditor_Preview"));
const FName FPlanetEditor::GraphCanvasTabId(TEXT("PlanetEditor_GraphCanvas"));
const FName FPlanetEditor::PropertiesTabId(TEXT("PlanetEditor_Properties"));
const FName FPlanetEditor::AppIdentifier(TEXT("PlanetEditorApp"));

FText FPlanetEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Planet Editor");
}

FName FPlanetEditor::GetToolkitFName() const
{
	return FName("PlanetEditor");
}

FText FPlanetEditor::GetToolkitName() const
{
	UObject* EditObject = GetEditingObjects()[0];
	return GetLabelForObject(EditObject);
}

FText FPlanetEditor::GetToolkitToolTipText() const
{
	return GetToolTipTextForObject(GetEditingObjects()[0]);
}

FString FPlanetEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Planet").ToString();
}

FLinearColor FPlanetEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.2f, 0.4f, 0.8f, 0.5f);
}

FText FPlanetEditor::GetOriginalObjectName() const
{
	return FText::FromString(GetEditingObjects()[0]->GetName());
}


/** Planet Editor Initialization */
void FPlanetEditor::Init(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost,
	UObject* ObjectToEdit)
{
	Planet = Cast<AProceduralPlanet>(ObjectToEdit);

	// Support undo/redo
	Planet->SetFlags(RF_Transactional);
	// GEditor->RegisterForUndo(this);

	CreateInternalWidgets();

	// create tab layout
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("Standalone_TextAssetEditor")
	->AddArea
	(
		FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewStack()
			->SetSizeCoefficient(0.1f)
			->SetHideTabWell(true)
			->AddTab(FName()/*GetToolbarTabId()*/, ETabState::OpenedTab)
		)
		->Split
		(
			FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(0.9f)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)->SetSizeCoefficient(0.2f)
				->Split
				(
					FTabManager::NewStack()
					->SetHideTabWell(true)
					->AddTab(PreviewTabId, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->AddTab(PropertiesTabId, ETabState::OpenedTab)
					/*->AddTab(PreviewSettingsTabId, ETabState::ClosedTab)
					->AddTab(ParameterDefaultsTabId, ETabState::OpenedTab)
					->AddTab(CustomPrimitiveTabId, ETabState::ClosedTab)
					->AddTab(LayerPropertiesTabId, ETabState::ClosedTab)*/
					->SetForegroundTab(PropertiesTabId)
				)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
				->SetSizeCoefficient(0.80f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.8f)
					->SetHideTabWell(true)
					->AddTab(GraphCanvasTabId, ETabState::OpenedTab)
				)
			)
		)
	);

	FAssetEditorToolkit::InitAssetEditor(
		Mode,
		InitToolkitHost,
		AppIdentifier,
		Layout,
		true /*bCreateDefaultStandaloneMenu*/,
		true /*bCreateDefaultToolbar*/,
		ObjectToEdit
	);

	RegenerateMenusAndToolbars();
}



void FPlanetEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& inTabManager)
{
	WorkspaceMenuCategory = inTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_PlanetEditor", "Planet Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(inTabManager);

	inTabManager->RegisterTabSpawner(PreviewTabId, FOnSpawnTab::CreateSP(this, &FPlanetEditor::SpawnTab_Preview))
		.SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Viewports"));

	inTabManager->RegisterTabSpawner(GraphCanvasTabId, FOnSpawnTab::CreateSP(this, &FPlanetEditor::SpawnTab_GraphCanvas))
		.SetDisplayName(LOCTEXT("GraphCanvasTab", "Graph"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.EventGraph_16x"));

	inTabManager->RegisterTabSpawner(PropertiesTabId, FOnSpawnTab::CreateSP(this, &FPlanetEditor::SpawnTab_Properties))
		.SetDisplayName(LOCTEXT("PropertiesTab", "Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));


	OnRegisterTabSpawners().Broadcast(inTabManager);
}

void FPlanetEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& inTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(inTabManager);

	inTabManager->UnregisterTabSpawner(PreviewTabId);
	inTabManager->UnregisterTabSpawner(GraphCanvasTabId);
	inTabManager->UnregisterTabSpawner(PropertiesTabId);


	OnUnregisterTabSpawners().Broadcast(inTabManager);
}



TSharedRef<SDockTab> FPlanetEditor::SpawnTab_Preview(const FSpawnTabArgs& Args)
{
	TSharedRef<SDockTab> SpawnedTab =
		SNew(SDockTab)
		.Label(LOCTEXT("ViewportTabTitle", "Viewport"))
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
		[
			PreviewViewport.ToSharedRef()
		]
	//  + SOverlay::Slot()
	//	[
	//		PreviewUIViewport.ToSharedRef()
	//	]
		];

	//PreviewViewport->OnAddedToTab(SpawnedTab);

	return SpawnedTab;
}

TSharedRef<SDockTab> FPlanetEditor::SpawnTab_GraphCanvas(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == GraphCanvasTabId);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("GraphCanvasTitle", "Graph"))
		[
			GraphEditor.ToSharedRef()
		];

	return SpawnedTab;
}

TSharedRef<SDockTab> FPlanetEditor::SpawnTab_Properties(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == PropertiesTabId);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("PlanetPropertiesTitle", "Details"))
		[
			DetailsView.ToSharedRef()
		];

	// UE5 Changed from setting on construction to set by function
	SpawnedTab->SetTabIcon(FAppStyle::GetBrush("LevelEditor.Tabs.Details"));

	return SpawnedTab;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FPlanetEditor::CreateInternalWidgets()
{
	PreviewViewport = SNew(SPlanetEditor3DPreviewViewport)
		.PlanetEditor(SharedThis(this));

	GraphEditor = 
		CreateGraphEditorWidget();

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	// UE5 updated the constructor to prefer manual setting of properties
	// const FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, true, this);
	FDetailsViewArgs DetailsViewArgs = FDetailsViewArgs();
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsViewArgs.NotifyHook = this;

	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION


TSharedRef<SGraphEditor> FPlanetEditor::CreateGraphEditorWidget()
{
	EdGraph = Cast<UPlanetGraph>(
		FBlueprintEditorUtils::CreateNewGraph(Planet, FName("Planet Graph"), UPlanetGraph::StaticClass(), UEdGraphSchema_Planet::StaticClass())
		);
	// TSharedPtr<FDocumentTracker> DocTracker = MakeShareable(new FDocumentTracker);
	TSharedPtr<FUICommandList> GraphEditorCommands = MakeShareable(new FUICommandList);
	SGraphEditor::FGraphEditorEvents InEvents;

	// Create the title bar widget
	TSharedPtr<SWidget> TitleBarWidget = 
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush(TEXT("Graph.TitleBackground")))
		.HAlign(HAlign_Fill)
		[
			SNew(SVerticalBox)
			// Title text/icon
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.Padding(10)
			.AutoHeight()
			[
				SNew(STextBlock)
				.TextStyle(FAppStyle::Get(), TEXT("GraphBreadcrumbButtonText"))
				.Text(this, &FPlanetEditor::GetOriginalObjectName)
			]
			+ SVerticalBox::Slot()
				.VAlign(VAlign_Top)
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.FillWidth(1.f)
				.Padding(5, 0)
			]
		];

	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.TitleBar(TitleBarWidget)
		.Appearance(this, &FPlanetEditor::GetGraphAppearance)
		.GraphToEdit(EdGraph)
		.GraphEvents(InEvents)
		.ShowGraphStateOverlay(false)
		.AssetEditorToolkit(this->AsShared());
}

FGraphAppearanceInfo FPlanetEditor::GetGraphAppearance() const
{
	return FGraphAppearanceInfo();
}



#undef LOCTEXT_NAMESPACE
