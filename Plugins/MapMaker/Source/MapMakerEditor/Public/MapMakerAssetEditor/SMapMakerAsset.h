// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SCompoundWidget.h"

/**
 * 
 */
class MAPMAKEREDITOR_API SMapMakerAsset : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SMapMakerAsset) {}

	SLATE_END_ARGS()
public:
	void Construct(const FArguments& InArgs, class UMapMakerAsset* InAsset);

	UMapMakerAsset* Asset;

	const FSlateBrush* GetAssetBrush() const;
private:

};
