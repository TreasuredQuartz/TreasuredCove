// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "CharacterClassMakerAssetEditor/AssetEditor_CharacterClassMaker.h"
#include "CharacterClassMakerAssetEditor/SCharacterClassEditor.h"
#include "CharacterClass.h"


#define LOCTEXT_NAMESPACE "AssetEditor_CharacterClassMaker"

const FName CharacterClassMakerEditorAppName = FName(TEXT("CharacterClassMakerEditorApp"));

struct FCharacterClassMakerAssetEditorTabs
{
	// Tab identifiers
	static const FName ClassTraitsEditorID;
	static const FName ClassFeaturesEditorID;
	static const FName ClassPropertyID;
};

//////////////////////////////////////////////////////////////////////////

const FName FCharacterClassMakerAssetEditorTabs::ClassTraitsEditorID(TEXT("ClassTraitsEditor"));
const FName FCharacterClassMakerAssetEditorTabs::ClassFeaturesEditorID(TEXT("ClassFeaturesEditor"));
const FName FCharacterClassMakerAssetEditorTabs::ClassPropertyID(TEXT("ClassMakerProperty"));

//////////////////////////////////////////////////////////////////////////

FAssetEditor_CharacterClassMaker::FAssetEditor_CharacterClassMaker()
{
	EditingClass = nullptr;

	OnPackageSavedDelegateHandle = UPackage::PackageSavedWithContextEvent.AddRaw(this, &FAssetEditor_CharacterClassMaker::OnPackageSavedWithContext);
}

FAssetEditor_CharacterClassMaker::~FAssetEditor_CharacterClassMaker()
{
	UPackage::PackageSavedWithContextEvent.Remove(OnPackageSavedDelegateHandle);
}

void FAssetEditor_CharacterClassMaker::InitCharacterClassMakerAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UCharacterClass* CharacterClassMaker)
{
	EditingClass = CharacterClassMaker;
	// EditingDatabase->Initialize();

	CreateInternalWidgets();

	// Layout
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_CharacterClassMakerEditor_Layout_v1")
		->AddArea(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(0.9f)
				->Split(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.25f)
					->AddTab(FCharacterClassMakerAssetEditorTabs::ClassPropertyID, ETabState::OpenedTab)->SetHideTabWell(true)
				)
				->Split(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.75f)
					->AddTab(FCharacterClassMakerAssetEditorTabs::ClassTraitsEditorID, ETabState::OpenedTab)->SetHideTabWell(true)
				)
			)
			->Split(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.25f)
				->AddTab(FCharacterClassMakerAssetEditorTabs::ClassFeaturesEditorID, ETabState::OpenedTab)->SetHideTabWell(true)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, CharacterClassMakerEditorAppName, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, EditingClass, false);
}

void FAssetEditor_CharacterClassMaker::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_CharacterClassMakerEditor", "Character Class Maker Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FCharacterClassMakerAssetEditorTabs::ClassTraitsEditorID, FOnSpawnTab::CreateSP(this, &FAssetEditor_CharacterClassMaker::SpawnTab_ClassEditor))
		.SetDisplayName(LOCTEXT("ClassEditorTab", "Character Class Editor"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner(FCharacterClassMakerAssetEditorTabs::ClassFeaturesEditorID, FOnSpawnTab::CreateSP(this, &FAssetEditor_CharacterClassMaker::SpawnTab_ClassBrowser))
		.SetDisplayName(LOCTEXT("ClassBrowserTab", "Class Browser"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FCharacterClassMakerAssetEditorTabs::ClassPropertyID, FOnSpawnTab::CreateSP(this, &FAssetEditor_CharacterClassMaker::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("DetailsTab", "Property"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FAssetEditor_CharacterClassMaker::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(FCharacterClassMakerAssetEditorTabs::ClassTraitsEditorID);
	InTabManager->UnregisterTabSpawner(FCharacterClassMakerAssetEditorTabs::ClassFeaturesEditorID);
	InTabManager->UnregisterTabSpawner(FCharacterClassMakerAssetEditorTabs::ClassPropertyID);
}

FName FAssetEditor_CharacterClassMaker::GetToolkitFName() const
{
	return FName("FCharacterClassMakerEditor");
}

FText FAssetEditor_CharacterClassMaker::GetBaseToolkitName() const
{
	return LOCTEXT("CharacterClassMakerEditorAppLabel", "Character Class Maker Editor");
}

FText FAssetEditor_CharacterClassMaker::GetToolkitName() const
{
	const bool bDirtyState = EditingClass->GetOutermost()->IsDirty();

	FFormatNamedArguments Args;
	Args.Add(TEXT("CharacterClassMakerName"), FText::FromString(EditingClass->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());
	return FText::Format(LOCTEXT("CharacterClassMakerEditorToolkitName", "{CharacterClassMakerName}{DirtyState}"), Args);
}

FText FAssetEditor_CharacterClassMaker::GetToolkitToolTipText() const
{
	return FAssetEditorToolkit::GetToolTipTextForObject(EditingClass);
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
	Collector.AddReferencedObject(EditingClass);
}

TSharedRef<SDockTab> FAssetEditor_CharacterClassMaker::SpawnTab_ClassEditor(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FCharacterClassMakerAssetEditorTabs::ClassTraitsEditorID);

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
	check(Args.GetTabId() == FCharacterClassMakerAssetEditorTabs::ClassFeaturesEditorID);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("ClassBrowserTab_Title", "Class Browser"));

	if (ClassBrowserWidget.IsValid())
	{
		SpawnedTab->SetContent(ClassBrowserWidget.ToSharedRef());
	}

	return SpawnedTab;
}

TSharedRef<SDockTab> FAssetEditor_CharacterClassMaker::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FCharacterClassMakerAssetEditorTabs::ClassPropertyID);

	return SNew(SDockTab)
#if ENGINE_MAJOR_VERSION < 5
		.Icon(FAppStyle::GetBrush("LevelEditor.Tabs.Details"))
#endif // #if ENGINE_MAJOR_VERSION < 5
		.Label(LOCTEXT("Details_Title", "Property"))
		[
			PropertyWidget.ToSharedRef()
		];
}

void FAssetEditor_CharacterClassMaker::CreateInternalWidgets()
{
	ClassEditorWidget = CreateClassEditorWidget();
	ClassBrowserWidget = CreateClassBrowserWidget();

	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.NotifyHook = this;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyWidget = PropertyModule.CreateDetailView(Args);
	PropertyWidget->SetObject(EditingClass);
	PropertyWidget->OnFinishedChangingProperties().AddSP(this, &FAssetEditor_CharacterClassMaker::OnFinishedChangingProperties);

	// FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FDatatableEditorModule>("DatatableEditor");
}

TSharedRef<SCompoundWidget> FAssetEditor_CharacterClassMaker::CreateClassEditorWidget()
{
	return SNew(SCharacterClassEditor, EditingClass);
}

TSharedRef<SCompoundWidget> FAssetEditor_CharacterClassMaker::CreateClassBrowserWidget()
{
	return SNew(SButton); // SNew(SClassBrowser, EditorModeManager.Get(), EditingDatabase);
}

void FAssetEditor_CharacterClassMaker::RebuildCharacterClassMaker()
{
	// EditingDatabase->RebuildDatabase();
}

void FAssetEditor_CharacterClassMaker::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
}

void FAssetEditor_CharacterClassMaker::OnPackageSavedWithContext(const FString& PackageFileName, UPackage* Package, FObjectPostSaveContext ObjectSaveContext)
{
	RebuildCharacterClassMaker();
}

#undef LOCTEXT_NAMESPACE