// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SCommonEditorViewportToolbarBase.h"

/**
 * 
 */
class MAPMAKEREDITOR_API SMapMakerViewportToolbar : public SCommonEditorViewportToolbarBase
{
public:
	SLATE_BEGIN_ARGS(SMapMakerViewportToolbar) { }

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedPtr<class SMapMakerPreviewViewport> InRealViewport);
};
