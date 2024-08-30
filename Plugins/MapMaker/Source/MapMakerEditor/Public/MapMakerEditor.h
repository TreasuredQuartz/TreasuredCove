// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include <IAssetTools.h>
#include <EdGraphUtilities.h>

DECLARE_LOG_CATEGORY_EXTERN(MapMakerEditor, Log, All);

class FMapMakerEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action);

private:
	TArray< TSharedPtr<IAssetTypeActions> > CreatedAssetTypeActions;

	EAssetTypeCategories::Type MapMakerAssetCategoryBit;

	TSharedPtr<FGraphPanelNodeFactory> GraphPanelNodeFactory_MapMaker;
};
