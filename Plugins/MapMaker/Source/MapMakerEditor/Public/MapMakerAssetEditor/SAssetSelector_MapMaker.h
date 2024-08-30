// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SCompoundWidget.h"
#include "Widgets/Views/STileView.h"

class UMapMakerAsset;
class UMapMakerAssetDatabase;

/**
 * 
 */
class MAPMAKEREDITOR_API SAssetSelector_MapMaker : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAssetSelector_MapMaker) 
		: _AssetDatabase()
		{}
		SLATE_ARGUMENT(UMapMakerAssetDatabase*, AssetDatabase)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	/* Adds a new textbox with the string to the list */
	TSharedRef<ITableRow> OnGenerateRowForList(UMapMakerAsset* Item, const TSharedRef<STableViewBase>& OwnerTable);

	/* The list of strings */
	// TArray<TSharedPtr<FString>> Items;

	void OnItemClicked(UMapMakerAsset* ClickedItem);

	/* The actual UI list */
	TSharedPtr<STileView<UMapMakerAsset*>> TileView;
};
