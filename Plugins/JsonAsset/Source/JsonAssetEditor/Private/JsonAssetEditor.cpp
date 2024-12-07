// Copyright Epic Games, Inc. All Rights Reserved.

#include "JsonAssetEditor.h"
#include <IAssetTools.h>
#include "AssetTypeActions_JsonDatabase.h"
// #include "JsonDatabaseNodeFactory.h"
// #include "JsonDatabaseStyle.h"

#define LOCTEXT_NAMESPACE "FJsonAssetEditorModule"

void FJsonAssetEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	// FJsonDatabaseStyle::Initialize();

	// GraphPanelNodeFactory_JsonDatabase = MakeShareable(new FGraphPanelNodeFactory_JsonDatabase());
	// FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory_JsonDatabase);

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	JsonDatabaseAssetCategoryBit = EAssetTypeCategories::Gameplay;// AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("JsonDatabase")), LOCTEXT("JsonDatabaseAssetCategory", "JsonDatabase"));

	RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeActions_JsonDatabase(JsonDatabaseAssetCategoryBit)));
}

void FJsonAssetEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FJsonAssetEditorModule::RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
	AssetTools.RegisterAssetTypeActions(Action);
	CreatedAssetTypeActions.Add(Action);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FJsonAssetEditorModule, JsonAssetEditor)