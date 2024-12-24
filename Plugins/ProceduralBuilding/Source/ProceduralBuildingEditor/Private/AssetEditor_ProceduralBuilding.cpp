// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetEditor_ProceduralBuilding.h"
#include "ProceduralBuilding.h"
#include "HAL/PlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "AssetEditor_ProceduralBuilding"

const FName ProceduralBuildingEditorAppName = FName(TEXT("ProceduralBuildingEditorApp"));

struct FProceduralBuildingAssetEditorTabs
{
	// Tab identifiers
	static const FName ProceduralBuildingViewportID;
	static const FName ProceduralBuildingAssetsID;
};

//////////////////////////////////////////////////////////////////////////

const FName FProceduralBuildingAssetEditorTabs::ProceduralBuildingViewportID(TEXT("ProceduralBuildingViewport"));
const FName FProceduralBuildingAssetEditorTabs::ProceduralBuildingAssetsID(TEXT("ProceduralBuildingAssets"));

//////////////////////////////////////////////////////////////////////////

FAssetEditor_ProceduralBuilding::FAssetEditor_ProceduralBuilding()
{
}

FAssetEditor_ProceduralBuilding::~FAssetEditor_ProceduralBuilding()
{
}

void FAssetEditor_ProceduralBuilding::InitProceduralBuildingAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UProceduralBuilding* Asset)
{
	EditingBuilding = Asset;
	// CreateEdAssets();

	/*FGenericCommands::Register();
	FGraphEditorCommands::Register();
	FEditorCommands_ProceduralBuilding::Register();

	BindCommands();*/
	CreateInternalWidgets();

	// Layout
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_ProceduralBuildingEditor_Layout_v1")
		->AddArea(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split(
				FTabManager::NewStack()
				->SetSizeCoefficient(1.f)
				->AddTab(FProceduralBuildingAssetEditorTabs::ProceduralBuildingViewportID, ETabState::OpenedTab)->SetHideTabWell(true)
			)
			->Split(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.3f)
				->AddTab(FProceduralBuildingAssetEditorTabs::ProceduralBuildingAssetsID, ETabState::OpenedTab)->SetHideTabWell(true)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, ProceduralBuildingEditorAppName, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, EditingBuilding, false);

	RegenerateMenusAndToolbars();
}

void FAssetEditor_ProceduralBuilding::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
}

void FAssetEditor_ProceduralBuilding::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
}

FName FAssetEditor_ProceduralBuilding::GetToolkitFName() const
{
	return FName("FProceduralBuildingEditor");
}

FText FAssetEditor_ProceduralBuilding::GetBaseToolkitName() const
{
	return LOCTEXT("ProceduralBuildingEditorAppLabel", "Procedural Building Editor");
}

FText FAssetEditor_ProceduralBuilding::GetToolkitName() const
{
	const bool bDirtyState = EditingBuilding->GetOutermost()->IsDirty();

	FFormatNamedArguments Args;
	Args.Add(TEXT("ProceduralBuildingName"), FText::FromString(EditingBuilding->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());
	return FText::Format(LOCTEXT("ProceduralBuildingEditorToolkitName", "{ProceduralBuildingName}{DirtyState}"), Args);
}

FText FAssetEditor_ProceduralBuilding::GetToolkitToolTipText() const
{
	return FAssetEditorToolkit::GetToolTipTextForObject(EditingBuilding);
}

FLinearColor FAssetEditor_ProceduralBuilding::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

FString FAssetEditor_ProceduralBuilding::GetWorldCentricTabPrefix() const
{
	return TEXT("ProceduralBuildingEditor");
}

FString FAssetEditor_ProceduralBuilding::GetDocumentationLink() const
{
	return TEXT("");
}

void FAssetEditor_ProceduralBuilding::SaveAsset_Execute()
{
}

void FAssetEditor_ProceduralBuilding::AddReferencedObjects(FReferenceCollector& Collector)
{
}

void FAssetEditor_ProceduralBuilding::CreateInternalWidgets()
{
}

#undef LOCTEXT_NAMESPACE