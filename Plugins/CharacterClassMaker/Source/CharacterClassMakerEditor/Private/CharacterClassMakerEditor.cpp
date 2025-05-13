// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterClassMakerEditor.h"
#include <IAssetTools.h>
#include "AssetTypeActions_CharacterClass.h"

#define LOCTEXT_NAMESPACE "FCharacterClassMakerEditorModule"

void FCharacterClassMakerEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	CharacterClassMakerAssetCategoryBit = EAssetTypeCategories::Gameplay;// AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("JsonDatabase")), LOCTEXT("JsonDatabaseAssetCategory", "JsonDatabase"));

	RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeActions_CharacterClass(CharacterClassMakerAssetCategoryBit)));
}

void FCharacterClassMakerEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FCharacterClassMakerEditorModule::RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
	AssetTools.RegisterAssetTypeActions(Action);
	CreatedAssetTypeActions.Add(Action);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCharacterClassMakerEditorModule, CharacterClassMakerEditor)