// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "JsonDatabaseAssetEditor/SJsonBrowser.h"
#include "JsonDatabaseAsset.h"
#include "JsonAsset.h"
#include "SJsonBrowserAsset.h"

#include "Widgets/Views/STileView.h"

void SJsonBrowser::Construct(const FArguments& InArgs, FEditorModeTools* Tools, UJsonDatabaseAsset* InDatabase)
{
	BrowsingDatabase = InDatabase;

	RebuildBrowser();
}

void SJsonBrowser::RebuildBrowser()
{
	TileView = nullptr;
	TArray<UJsonAsset*> Assets = BrowsingDatabase->GetAssets();

	int32 i = 0;
	JsonFiles.Empty(Assets.Num());
	for (UJsonAsset* Asset : Assets)
	{
		TSharedPtr<FJsonBrowserTile> Tile = MakeShareable(new FJsonBrowserTile(i, Asset));
		JsonFiles.Add(Tile);
		++i;
	}

	this->ChildSlot
		[
			SNew(SBorder)
				.OnMouseButtonDown(this, &SJsonBrowser::OnBrowserClicked)
				[
					SNew(SScrollBox)
						+ SScrollBox::Slot()
						[
							SAssignNew(TileView, STileView<TSharedPtr<FJsonBrowserTile>>)
								.ListItemsSource(&JsonFiles)
								.OnGenerateTile(this, &SJsonBrowser::OnGenerateRowForList)
								.OnMouseButtonDoubleClick(this, &SJsonBrowser::OnItemDoubleClicked)
								.OnMouseButtonClick(this, &SJsonBrowser::OnItemClicked)
						]
				]
		];
}

TSharedRef<ITableRow> SJsonBrowser::OnGenerateRowForList(TSharedPtr<FJsonBrowserTile> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	// Create the row
	return 
		SNew(STableRow< TSharedPtr<FJsonBrowserTile> >, OwnerTable)
		.Padding(2.0f)
		[
			SNew(SJsonBrowserAsset, Item->TileAsset)
		];
}

FReply SJsonBrowser::OnBrowserClicked(const FGeometry& Geometry, const FPointerEvent& Event)
{
	return FReply::Handled();
}

void SJsonBrowser::OnItemClicked(TSharedPtr<FJsonBrowserTile> ClickedAsset)
{
	// IFileManager::Get().Move();
}

void SJsonBrowser::OnItemDoubleClicked(TSharedPtr<FJsonBrowserTile> ClickedAsset)
{
	if (BrowsingDatabase) {
		int32 Index = ClickedAsset->TileIndex;
		if (Index == -1)
		{
			BrowsingDatabase->CreateNewAsset();
			return;
		}

		BrowsingDatabase->SetCurrentAsset(Index);
	}
}

FSlateBrush SJsonBrowser::GetAddButtonTileBrush() const
{
	return FSlateBrush();
}

