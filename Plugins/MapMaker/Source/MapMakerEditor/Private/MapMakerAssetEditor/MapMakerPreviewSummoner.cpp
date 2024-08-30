// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerAssetEditor/MapMakerPreviewSummoner.h"
#include "MapMakerAssetEditor/AssetEditor_MapMaker.h"

//FMapMakerPreviewSummoner::FMapMakerPreviewSummoner(TSharedPtr<class FAssetEditor_MapMaker> InMapMakerEditorPtr)
//	// : FWorkflowTabFactory(FMapMakerAssetEditorTabs::PreviewID, InMapMakerEditorPtr)
//	// , MapMakerEditorPtr(InMapMakerEditorPtr)
//{
//	/*TabLabel = LOCTEXT("MapMakerPreviewLabel", "Preview");
//	TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Components");
//	bIsSingleton = true;
//	ViewMenuDescription = LOCTEXT("MapMakerPreview", "Priview");
//	ViewMenuTooltip = LOCTEXT("MapMakerPreview_ToolTip", "Show the preview tab");*/
//}
//
//TSharedRef<SWidget> FMapMakerPreviewSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
//{
//	return MapMakerEditorPtr.Pin()->SpawnPreview();
//}
//
//FText FMapMakerPreviewSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
//{
//	return LOCTEXT("MapMakerPreviewTabTooltip", "The preview tab allows displaying models");
//}
