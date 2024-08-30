// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowTabFactory.h"

/**
 * 
 */
struct MAPMAKEREDITOR_API FMapMakerPreviewSummoner : public FWorkflowTabFactory
{
public:
	// FMapMakerPreviewSummoner(TSharedPtr<class FAssetEditor_MapMaker> InMapMakerEditorPtr);

	// virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	// virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

protected:
	TWeakPtr<class FAssetEditor_MapMaker> MapMakerEditorPtr;
};
