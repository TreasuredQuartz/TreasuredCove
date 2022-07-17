// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ProceduralPlanetPluginEditor.h"

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "Modules/ModuleManager.h"
#include "AssetData.h"
#include "AssetRegistryModule.h"
#include "Engine/Blueprint.h"

#include "Customization/PlanetSettingsCustomization.h"
#include "AssetTypeActions_Planet.h"
#include "PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "ProceduralPlanetEditor"

class FProceduralPlanetEditorModule : public IPlanetEditorModule
{

public:
	FProceduralPlanetEditorModule()
	{
	}

	void StartupModule()
	{
		/*FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		PropertyModule.RegisterCustomPropertyTypeLayout(
			FPlanetSettings::StaticStruct()->GetFName(),
			);*/

			// Register
		IAssetTools& AssetToolsModule =
			FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

		// AssetCategory

		// Repeat for each IAssetTypeActions class.
		TSharedPtr<IAssetTypeActions> AssetActions =
			MakeShareable(new FAssetTypeActions_Planet());
		AssetToolsModule.RegisterAssetTypeActions(AssetActions.ToSharedRef());
	}

	void ShutdownModule()
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			// unregister properties when the module is shutdown
			// IAssetTools& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
			// AssetToolsModule.UnregisterAssetTypeActions();
		}
	}

	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 *
	static inline FProceduralPlanetEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked< FProceduralPlanetEditorModule >("ProceduralPlanetPluginEditor");
	}*/

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 *
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ProceduralPlanetPluginEditor");
	}*/
};

IMPLEMENT_MODULE(FProceduralPlanetEditorModule, ProceduralPlanetPluginEditor)

#undef LOCTEXT_NAMESPACE
