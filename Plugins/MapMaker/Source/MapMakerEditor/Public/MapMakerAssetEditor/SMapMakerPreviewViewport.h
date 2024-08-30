// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SEditorViewport.h"
#include "SCommonEditorViewportToolbarBase.h"

/**
 * 
 */
class MAPMAKEREDITOR_API SMapMakerPreviewViewport : public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SMapMakerPreviewViewport) {}
		SLATE_ARGUMENT(TSharedPtr<FEditorViewportClient>, LevelEditorViewportClient)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	//Toolbar interface
	virtual TSharedRef<class SEditorViewport> GetViewportWidget() override;
	virtual TSharedPtr<FExtender> GetExtenders() const override;
	virtual void OnFloatingButtonClicked() override;

private:
	/** SWidget Interface */
	virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	/** End of SWidget interface */

	bool HandleDragObjects(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);
	bool HandlePlaceDraggedObjects(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent, bool bCreateDropPreview);
	void GetAssetsFromDrag(const FDragDropEvent& DragDropEvent, TArray<FAssetData>& AssetDataArray);

protected:
	// Create viewport client and toolbar
	// SEditorViewport interface
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	virtual TSharedPtr<SWidget> MakeViewportToolbar() override;
	// End of SEditorViewport interface

	TSharedRef<SWidget> BuildViewportDragDropContextMenu();
	void SetKeyboardFocusToThisViewport();

private:
	FEditorModeTools* Tools;
	class FPreviewScene_MapMaker* Scene;
	TSharedPtr<FEditorViewportClient> LevelViewportClient;
	TWeakPtr<class FAssetEditor_MapMaker> ParentLevelEditor;

	TArray<UObject*> DroppedObjects;
	FVector2D CachedOnDropLocalMousePos;
};
