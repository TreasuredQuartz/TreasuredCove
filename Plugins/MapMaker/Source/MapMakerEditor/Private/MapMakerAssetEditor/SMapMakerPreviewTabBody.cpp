// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerAssetEditor/SMapMakerPreviewTabBody.h"
#include "MapMakerAssetEditor/SMapMakerPreviewViewport.h"
#include "MapMakerAssetEditor/FPreviewScene_MapMaker.h"
#include "MapMakerAssetEditor/FEditorViewportClient_MapMaker.h"
#include "EditorModeManager.h"

void SMapMakerViewportTabBody::Construct(const FArguments& InArgs, FEditorModeTools* Tools, FPreviewScene_MapMaker& Scene)
{
	m_ViewportWidget = 
		SNew(SMapMakerPreviewViewport)
		.LevelEditorViewportClient(MakeShareable(new FEditorViewportClient_MapMaker(Tools, &Scene)));
	m_LevelViewportClient = m_ViewportWidget->GetViewportClient();
	TSharedPtr<SVerticalBox> ViewportContainer = nullptr;
	this->ChildSlot
		[
			SAssignNew(ViewportContainer, SVerticalBox)

				+ SVerticalBox::Slot()
				.VAlign(EVerticalAlignment::VAlign_Fill)
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				.FillHeight(1)
				[
					SNew(SOverlay)
						// The viewport
						+ SOverlay::Slot()
						[
							m_ViewportWidget.ToSharedRef()
						]
				]
		];
}
