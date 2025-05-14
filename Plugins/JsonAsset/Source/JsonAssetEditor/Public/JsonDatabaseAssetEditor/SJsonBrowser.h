// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UJsonAsset;
class UJsonDatabaseAsset;
template <typename ItemType>
class STileView;

struct JSONASSETEDITOR_API FJsonBrowserTile
{
public:
	FJsonBrowserTile() : 
		TileIndex(-1),
		TileAsset(nullptr)
	{};

	FJsonBrowserTile(int32 InTileIndex, UJsonAsset* InTileAsset) :
		TileIndex(InTileIndex),
		TileAsset(InTileAsset)
	{};

	int32 TileIndex;
	UJsonAsset* TileAsset;
};

/**
 * 
 */
class JSONASSETEDITOR_API SJsonBrowser : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SJsonBrowser) {}

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, FEditorModeTools* Tools, UJsonDatabaseAsset* InDatabase);

	void RebuildBrowser();

	/* Adds a new textbox with the string to the list */
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FJsonBrowserTile> Item, const TSharedRef<STableViewBase>& OwnerTable);

	FReply OnKeyDown(const FGeometry& Geometry, const FKeyEvent& KeyEvent) override;
	FReply OnBrowserClicked(const FGeometry& Geometry, const FPointerEvent& Event);
	void OnItemClicked(TSharedPtr<FJsonBrowserTile> ClickedAsset);
	void OnItemDoubleClicked(TSharedPtr<FJsonBrowserTile> ClickedAsset);

	FSlateBrush GetAddButtonTileBrush() const;

private:
	UJsonDatabaseAsset* BrowsingDatabase;

	/* The actual UI list. */
	TSharedPtr<STileView<TSharedPtr<FJsonBrowserTile>>> TileView;

	/* The Items array that the list updates from. */
	TArray<TSharedPtr<FJsonBrowserTile>> JsonFiles;

	/* The current UI Item that has been selected in the list. */
	TSharedPtr<FJsonBrowserTile> SelectedAsset;
};
