// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "CharacterClassMakerAssetEditor/AssetEditor_CharacterClassMaker.h"
#include "CharacterClass.h"


#define LOCTEXT_NAMESPACE "AssetEditor_CharacterClassMaker"

const FName CharacterClassMakerEditorAppName = FName(TEXT("CharacterClassMakerEditorApp"));

struct FCharacterClassMakerAssetEditorTabs
{
	// Tab identifiers
	static const FName ClassEditorID;
	static const FName ClassBrowserID;
};

//////////////////////////////////////////////////////////////////////////

const FName FCharacterClassMakerAssetEditorTabs::ClassEditorID(TEXT("ClassEditor"));
const FName FCharacterClassMakerAssetEditorTabs::ClassBrowserID(TEXT("ClassBrowser"));

//////////////////////////////////////////////////////////////////////////

FAssetEditor_CharacterClassMaker::FAssetEditor_CharacterClassMaker()
{
	EditingDatabase = nullptr;

	OnPackageSavedDelegateHandle = UPackage::PackageSavedWithContextEvent.AddRaw(this, &FAssetEditor_CharacterClassMaker::OnPackageSavedWithContext);
}

FAssetEditor_CharacterClassMaker::~FAssetEditor_CharacterClassMaker()
{
	UPackage::PackageSavedWithContextEvent.Remove(OnPackageSavedDelegateHandle);
}

void FAssetEditor_CharacterClassMaker::InitCharacterClassMakerAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UCharacterClass* CharacterClassMaker)
{
	EditingDatabase = CharacterClassMaker;
	// EditingDatabase->Initialize();

	CreateInternalWidgets();

	// Layout
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_CharacterClassMakerEditor_Layout_v1")
		->AddArea(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Horizontal)
			->Split(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.25f)
				->AddTab(FCharacterClassMakerAssetEditorTabs::ClassBrowserID, ETabState::OpenedTab)->SetHideTabWell(true)
			)
			->Split(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.75f)
				->AddTab(FCharacterClassMakerAssetEditorTabs::ClassEditorID, ETabState::OpenedTab)->SetHideTabWell(true)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, CharacterClassMakerEditorAppName, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, EditingDatabase, false);
}

void FAssetEditor_CharacterClassMaker::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_CharacterClassMakerEditor", "Character Class Maker Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FCharacterClassMakerAssetEditorTabs::ClassEditorID, FOnSpawnTab::CreateSP(this, &FAssetEditor_CharacterClassMaker::SpawnTab_ClassEditor))
		.SetDisplayName(LOCTEXT("ClassEditorTab", "Character Class Editor"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner(FCharacterClassMakerAssetEditorTabs::ClassBrowserID, FOnSpawnTab::CreateSP(this, &FAssetEditor_CharacterClassMaker::SpawnTab_ClassBrowser))
		.SetDisplayName(LOCTEXT("ClassBrowserTab", "Class Browser"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FAssetEditor_CharacterClassMaker::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(FCharacterClassMakerAssetEditorTabs::ClassEditorID);
	InTabManager->UnregisterTabSpawner(FCharacterClassMakerAssetEditorTabs::ClassBrowserID);
}

FName FAssetEditor_CharacterClassMaker::GetToolkitFName() const
{
	return FName("FCharacterClassMakerEditor");
}

FText FAssetEditor_CharacterClassMaker::GetBaseToolkitName() const
{
	return LOCTEXT("CharacterClassMakerEditorAppLabel", "Json Database Editor");
}

FText FAssetEditor_CharacterClassMaker::GetToolkitName() const
{
	const bool bDirtyState = EditingDatabase->GetOutermost()->IsDirty();

	FFormatNamedArguments Args;
	Args.Add(TEXT("CharacterClassMakerName"), FText::FromString(EditingDatabase->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());
	return FText::Format(LOCTEXT("CharacterClassMakerEditorToolkitName", "{CharacterClassMakerName}{DirtyState}"), Args);
}

FText FAssetEditor_CharacterClassMaker::GetToolkitToolTipText() const
{
	return FAssetEditorToolkit::GetToolTipTextForObject(EditingDatabase);
}

FLinearColor FAssetEditor_CharacterClassMaker::GetWorldCentricTabColorScale() const
{
	return FLinearColor();
}

FString FAssetEditor_CharacterClassMaker::GetWorldCentricTabPrefix() const
{
	return FString();
}

FString FAssetEditor_CharacterClassMaker::GetDocumentationLink() const
{
	return FString();
}

void FAssetEditor_CharacterClassMaker::SaveAsset_Execute()
{
}

void FAssetEditor_CharacterClassMaker::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EditingDatabase);
}

TSharedRef<SDockTab> FAssetEditor_CharacterClassMaker::SpawnTab_ClassEditor(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FCharacterClassMakerAssetEditorTabs::ClassEditorID);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("ClassEditorTab_Title", "Class Editor"));

	if (ClassEditorWidget.IsValid())
	{
		SpawnedTab->SetContent(ClassEditorWidget.ToSharedRef());
	}

	return SpawnedTab;
}

TSharedRef<SDockTab> FAssetEditor_CharacterClassMaker::SpawnTab_ClassBrowser(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FCharacterClassMakerAssetEditorTabs::ClassBrowserID);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("ClassBrowserTab_Title", "Class Browser"));

	if (ClassBrowserWidget.IsValid())
	{
		SpawnedTab->SetContent(ClassBrowserWidget.ToSharedRef());
	}

	return SpawnedTab;
}

void FAssetEditor_CharacterClassMaker::CreateInternalWidgets()
{
	ClassEditorWidget = CreateClassEditorWidget();
	ClassBrowserWidget = CreateClassBrowserWidget();
}

TSharedRef<SCompoundWidget> FAssetEditor_CharacterClassMaker::CreateClassEditorWidget()
{
	return SNew(SButton); // SNew(SClassEditor, EditorModeManager.Get(), EditingDatabase);
}

TSharedRef<SCompoundWidget> FAssetEditor_CharacterClassMaker::CreateClassBrowserWidget()
{
	return SNew(SButton); // SNew(SClassBrowser, EditorModeManager.Get(), EditingDatabase);
}

void FAssetEditor_CharacterClassMaker::RebuildCharacterClassMaker()
{
	// EditingDatabase->RebuildDatabase();
}

void FAssetEditor_CharacterClassMaker::OnPackageSavedWithContext(const FString& PackageFileName, UPackage* Package, FObjectPostSaveContext ObjectSaveContext)
{
	RebuildCharacterClassMaker();
}

#undef LOCTEXT_NAMESPACE