// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerAssetEditor/SMapMakerAsset.h"
#include "MapMakerAsset.h"

void SMapMakerAsset::Construct(const FArguments& InArgs, UMapMakerAsset* InAsset)
{
	Asset = InAsset;

	ChildSlot
	[
		SNew(SImage)
			.Image(this, &SMapMakerAsset::GetAssetBrush)
	];
}

const FSlateBrush* SMapMakerAsset::GetAssetBrush() const
{
	FSlateBrush* Brush = new FSlateBrush();
	Brush->TintColor = FSlateColor(FColor::White);

	if (Asset)
	{
		Brush = &Asset->Brush;
	}
	else {
		Brush->TintColor = FSlateColor(FColor::Black);
		Brush->SetImageSize(FVector2D(128, 128));
	}

	return Brush;
}
