// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Blueprint.h"

#include "IPhysXFlyingVehiclesEditorPlugin.h"

class FPhysXFlyingVehiclesEditorPlugin : public IPhysXFlyingVehiclesEditorPlugin
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.NotifyCustomizationModuleChanged();
	}


	virtual void ShutdownModule() override
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.NotifyCustomizationModuleChanged();
	}
};

IMPLEMENT_MODULE(FPhysXFlyingVehiclesEditorPlugin, PhysXFlyingVehiclesEditor)

