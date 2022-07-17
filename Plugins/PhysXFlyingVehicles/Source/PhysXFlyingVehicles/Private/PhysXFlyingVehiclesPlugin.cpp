// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "UObject/UObjectIterator.h"

#include "IPhysXFlyingVehiclesPlugin.h"

class FPhysXFlyingVehiclesPlugin : public IPhysXFlyingVehiclesPlugin
{
private:

public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{
	}

	virtual void ShutdownModule() override
	{
	}
};

IMPLEMENT_MODULE(FPhysXFlyingVehiclesPlugin, PhysXFlyingVehicles )







