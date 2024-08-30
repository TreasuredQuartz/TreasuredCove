// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerAssetEditor/SMapMakerViewportToolbar.h"
#include "MapMakerAssetEditor/SMapMakerPreviewViewport.h"

void SMapMakerViewportToolbar::Construct(const FArguments& InArgs, TSharedPtr<class SMapMakerPreviewViewport> InRealViewport)
{
	SCommonEditorViewportToolbarBase::Construct(SCommonEditorViewportToolbarBase::FArguments(), InRealViewport);
}
