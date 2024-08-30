// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MAPMAKEREDITOR_API FEditorCommands_MapMaker : public TCommands<FEditorCommands_MapMaker>
{
public:
	/** Constructor */
	FEditorCommands_MapMaker()
		: TCommands<FEditorCommands_MapMaker>("MapMakerEditor", NSLOCTEXT("Contexts", "MapMakerEditor", "Map Maker Editor"), NAME_None, FAppStyle::GetAppStyleSetName())
	{
	}

	TSharedPtr<FUICommandInfo> GraphSettings;
	TSharedPtr<FUICommandInfo> AutoArrange;

	virtual void RegisterCommands() override;
};
