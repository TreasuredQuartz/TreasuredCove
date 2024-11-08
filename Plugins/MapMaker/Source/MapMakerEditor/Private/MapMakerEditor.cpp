// Copyright Epic Games, Inc. All Rights Reserved.

#include "MapMakerEditor.h"
#include "AssetTypeActions_MapMaker.h"
#include "MapMakerNodeFactory.h"
#include "MapMakerStyle.h"

DEFINE_LOG_CATEGORY(MapMakerEditor)

#define LOCTEXT_NAMESPACE "FMapMakerEditorModule"

void FMapMakerEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FMapMakerStyle::Initialize();

	GraphPanelNodeFactory_MapMaker = MakeShareable(new FGraphPanelNodeFactory_MapMaker());
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory_MapMaker);

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	MapMakerAssetCategoryBit = EAssetTypeCategories::Gameplay;// AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("MapMaker")), LOCTEXT("MapMakerAssetCategory", "MapMaker"));

	RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeActions_MapMaker(MapMakerAssetCategoryBit)));
}

void FMapMakerEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FMapMakerStyle::Shutdown();
	
	// Unregister all the asset types that we registered
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (int32 Index = 0; Index < CreatedAssetTypeActions.Num(); ++Index)
		{
			AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[Index].ToSharedRef());
		}
	}

	if (GraphPanelNodeFactory_MapMaker.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(GraphPanelNodeFactory_MapMaker);
		GraphPanelNodeFactory_MapMaker.Reset();
	}
}

void FMapMakerEditorModule::RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
	AssetTools.RegisterAssetTypeActions(Action);
	CreatedAssetTypeActions.Add(Action);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMapMakerEditorModule, MapMakerEditor)