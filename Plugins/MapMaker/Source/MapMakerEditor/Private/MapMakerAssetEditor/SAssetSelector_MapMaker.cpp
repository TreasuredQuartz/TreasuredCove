// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerAssetEditor/SAssetSelector_MapMaker.h"
#include "MapMakerAssetEditor/EdGraph_MapMaker.h"
#include "MapMaker.h"
#include "MapMakerAssetDatabase.h"
#include "MapMakerAsset.h"

void SAssetSelector_MapMaker::Construct(const FArguments& InArgs)
{
	EditingGraph = Cast<UEdGraph_MapMaker>(InArgs._GraphToEdit);
	if (EditingGraph == nullptr) return;

	UMapMakerAssetDatabase* Database = InArgs._AssetDatabase;
	FSlateBrush Brush = Database->Asset2DRepresentation;
	TArray<TSharedPtr<SButton>> AssetButtons;
	this->OnAssetClicked = InArgs._OnAssetClicked;

	ChildSlot
	[
		SAssignNew(TileView, STileView<UMapMakerAsset*>)
			.ListItemsSource(&Database->Assets)
			.OnGenerateTile(this, &SAssetSelector_MapMaker::OnGenerateRowForList)
			.OnMouseButtonClick(this, &SAssetSelector_MapMaker::OnItemClicked)
	];
}

TSharedRef<ITableRow> SAssetSelector_MapMaker::OnGenerateRowForList(UMapMakerAsset* Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	//Create the row
	return SNew(STableRow< UMapMakerAsset* >, OwnerTable)
		.Padding(2.0f)
		[
			SNew(SImage)
				.Image(&Item->Brush)
		];
}

void SAssetSelector_MapMaker::OnItemClicked(UMapMakerAsset* ClickedAsset)
{
	if (EditingGraph) {
		EditingGraph->GetMapMaker()->CurrentAsset = ClickedAsset;
	}

	if (OnAssetClicked.IsBound())
	{
		OnAssetClicked.Execute(ClickedAsset);
	}
}

