// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class UEdNode_MapMakerNode;
class UMapMakerAsset;
class SMapMakerAsset;

/**
 * 
 */
class MAPMAKEREDITOR_API SEdNode_MapMakerNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SEdNode_MapMakerNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdNode_MapMakerNode* InNode);

	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual bool IsNameReadOnly() const override;

	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);
	// void OnResized(const FVector2D& Delta);
	FReply OnClearButtonClicked();
	// FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	// FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	// FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual FSlateColor GetBorderBackgroundColor() const;
	virtual FSlateColor GetBackgroundColor() const;

	virtual EVisibility GetDragOverMarkerVisibility() const;

	virtual const FSlateBrush* GetNameIcon() const;
	virtual const FButtonStyle* GetClearButtonStyle() const;
	// virtual FVector2D GetSize() const;
	// virtual const FSlateBrush* GetRoomBrush() const;
	// UMapMakerAsset* GetCurrentAsset() const;


protected:
	/** Image for center of node to represent a room. */
	TSharedPtr<SImage> Image;

	/** Collection of images that represent internal assets. */
	TSharedPtr<class SCanvas> AssetCanvas;

	/**  */
	bool bBrushActivated = false;
};
