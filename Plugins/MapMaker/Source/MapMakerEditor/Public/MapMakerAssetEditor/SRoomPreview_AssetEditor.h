// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UEdGraphNode;
class UMapMakerAsset;
class SMapMakerAsset;

/**
 * 
 */
class MAPMAKEREDITOR_API SRoomPreview_AssetEditor : public SCompoundWidget
{
private:
	uint8 bBrushActivated : 1;
	UEdGraphNode* GraphNode;

public:
	SLATE_BEGIN_ARGS(SRoomPreview_AssetEditor)
		{}
		/** Slot for this button's content (optional) */
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, class UEdGraphNode* InNode);

	void UpdateGraphNode();
	void CreateAssetWidgets();
	void AddAsset(const TSharedRef<SMapMakerAsset>& AssetToAdd);

	FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
	void OnResized(const FVector2D& Delta);
	FVector2D GetSize() const;
	const FSlateBrush* GetRoomBrush() const;
	UMapMakerAsset* GetCurrentAsset() const;

private:
	TSharedPtr<class SCanvas> AssetCanvas;
};
