// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class UEdNode_MapMakerNode;

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
	void OnResized(const FVector2D& Delta);

	virtual FSlateColor GetBorderBackgroundColor() const;
	virtual FSlateColor GetBackgroundColor() const;

	virtual EVisibility GetDragOverMarkerVisibility() const;

	virtual const FSlateBrush* GetNameIcon() const;
	virtual FVector2D GetSize() const;
	virtual const FSlateBrush* GetRoomBrush() const;


protected:
	/** Image for center of node to represent a room. */
	TSharedPtr<SImage> Image;
};
