// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SMapMakerPreviewViewport;
class FPreviewScene_MapMaker;

/**
 * 
 */
class MAPMAKEREDITOR_API SMapMakerViewportTabBody : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SMapMakerViewportTabBody) {}

	SLATE_END_ARGS()
public:

	void Construct(const FArguments& InArgs, FEditorModeTools* Tools, FPreviewScene_MapMaker& Scene);

private:
	TSharedPtr<FEditorViewportClient> m_LevelViewportClient;
	TSharedPtr<SMapMakerPreviewViewport> m_ViewportWidget;
};
