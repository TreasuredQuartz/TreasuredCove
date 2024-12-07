// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "JsonDatabaseAssetEditor/AssetEditor_JsonDatabase.h"
#include "JsonDatabaseAsset.h"

#include "SJsonEditor.h"
#include "SJsonBrowser.h"

#define LOCTEXT_NAMESPACE "AssetEditor_JsonDatabase"

const FName JsonDatabaseEditorAppName = FName(TEXT("JsonDatabaseEditorApp"));

struct FJsonDatabaseAssetEditorTabs
{
	// Tab identifiers
	static const FName JsonEditorID;
	static const FName JsonBrowserID;
};

//////////////////////////////////////////////////////////////////////////

const FName FJsonDatabaseAssetEditorTabs::JsonEditorID(TEXT("JsonEditor"));
const FName FJsonDatabaseAssetEditorTabs::JsonBrowserID(TEXT("JsonBrowser"));

//////////////////////////////////////////////////////////////////////////

FAssetEditor_JsonDatabase::FAssetEditor_JsonDatabase()
{
	EditingDatabase = nullptr;

	OnPackageSavedDelegateHandle = UPackage::PackageSavedWithContextEvent.AddRaw(this, &FAssetEditor_JsonDatabase::OnPackageSavedWithContext);
}

FAssetEditor_JsonDatabase::~FAssetEditor_JsonDatabase()
{
	UPackage::PackageSavedWithContextEvent.Remove(OnPackageSavedDelegateHandle);
}

void FAssetEditor_JsonDatabase::InitJsonDatabaseAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UJsonDatabaseAsset* JsonDatabase)
{
	EditingDatabase = JsonDatabase;
	EditingDatabase->Initialize();

	CreateInternalWidgets();

	// Layout
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_JsonDatabaseEditor_Layout_v1")
		->AddArea(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Horizontal)
			->Split(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.25f)
				->AddTab(FJsonDatabaseAssetEditorTabs::JsonBrowserID, ETabState::OpenedTab)->SetHideTabWell(true)
			)
			->Split(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.75f)
				->AddTab(FJsonDatabaseAssetEditorTabs::JsonEditorID, ETabState::OpenedTab)->SetHideTabWell(true)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, JsonDatabaseEditorAppName, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, EditingDatabase, false);
}

void FAssetEditor_JsonDatabase::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_JsonDatabaseEditor", "Json Database Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FJsonDatabaseAssetEditorTabs::JsonEditorID, FOnSpawnTab::CreateSP(this, &FAssetEditor_JsonDatabase::SpawnTab_JsonEditor))
		.SetDisplayName(LOCTEXT("JsonEditorTab", "Json Editor"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner(FJsonDatabaseAssetEditorTabs::JsonBrowserID, FOnSpawnTab::CreateSP(this, &FAssetEditor_JsonDatabase::SpawnTab_JsonBrowser))
		.SetDisplayName(LOCTEXT("JsonBrowserTab", "Json Browser"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FAssetEditor_JsonDatabase::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(FJsonDatabaseAssetEditorTabs::JsonEditorID);
	InTabManager->UnregisterTabSpawner(FJsonDatabaseAssetEditorTabs::JsonBrowserID);
}

FName FAssetEditor_JsonDatabase::GetToolkitFName() const
{
	return FName("FJsonDatabaseEditor");
}

FText FAssetEditor_JsonDatabase::GetBaseToolkitName() const
{
	return LOCTEXT("JsonDatabaseEditorAppLabel", "Json Database Editor");
}

FText FAssetEditor_JsonDatabase::GetToolkitName() const
{
	const bool bDirtyState = EditingDatabase->GetOutermost()->IsDirty();

	FFormatNamedArguments Args;
	Args.Add(TEXT("JsonDatabaseName"), FText::FromString(EditingDatabase->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());
	return FText::Format(LOCTEXT("JsonDatabaseEditorToolkitName", "{JsonDatabaseName}{DirtyState}"), Args);
}

FText FAssetEditor_JsonDatabase::GetToolkitToolTipText() const
{
	return FAssetEditorToolkit::GetToolTipTextForObject(EditingDatabase);
}

FLinearColor FAssetEditor_JsonDatabase::GetWorldCentricTabColorScale() const
{
	return FLinearColor();
}

FString FAssetEditor_JsonDatabase::GetWorldCentricTabPrefix() const
{
	return FString();
}

FString FAssetEditor_JsonDatabase::GetDocumentationLink() const
{
	return FString();
}

void FAssetEditor_JsonDatabase::SaveAsset_Execute()
{
}

void FAssetEditor_JsonDatabase::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EditingDatabase);
}

TSharedRef<SDockTab> FAssetEditor_JsonDatabase::SpawnTab_JsonEditor(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FJsonDatabaseAssetEditorTabs::JsonEditorID);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("JsonEditorTab_Title", "Json Editor"));

	if (JsonEditorWidget.IsValid())
	{
		SpawnedTab->SetContent(JsonEditorWidget.ToSharedRef());
	}

	return SpawnedTab;
}

TSharedRef<SDockTab> FAssetEditor_JsonDatabase::SpawnTab_JsonBrowser(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FJsonDatabaseAssetEditorTabs::JsonBrowserID);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("JsonBrowserTab_Title", "Json Browser"));

	if (JsonBrowserWidget.IsValid())
	{
		SpawnedTab->SetContent(JsonBrowserWidget.ToSharedRef());
	}

	return SpawnedTab;
}

void FAssetEditor_JsonDatabase::CreateInternalWidgets()
{
	JsonEditorWidget = CreateJsonEditorWidget();
	JsonBrowserWidget = CreateJsonBrowserWidget();
}

TSharedRef<SCompoundWidget> FAssetEditor_JsonDatabase::CreateJsonEditorWidget()
{
	return SNew(SJsonEditor, EditorModeManager.Get(), EditingDatabase);
}

TSharedRef<SCompoundWidget> FAssetEditor_JsonDatabase::CreateJsonBrowserWidget()
{
	return SNew(SJsonBrowser, EditorModeManager.Get(), EditingDatabase);
}

void FAssetEditor_JsonDatabase::RebuildJsonDatabase()
{
	EditingDatabase->RebuildDatabase();
}

void FAssetEditor_JsonDatabase::OnPackageSavedWithContext(const FString& PackageFileName, UPackage* Package, FObjectPostSaveContext ObjectSaveContext)
{
	RebuildJsonDatabase();
}

#undef LOCTEXT_NAMESPACE
