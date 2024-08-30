// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class SToolTip;
class UEdNode_MapMakerEdge;

/**
 * 
 */
class MAPMAKEREDITOR_API SEdNode_MapMakerEdge : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SEdNode_MapMakerEdge) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdNode_MapMakerEdge* InNode);

	virtual bool RequiresSecondPassLayout() const override;
	virtual void PerformSecondPassLayout(const TMap< UObject*, TSharedRef<SNode> >& NodeToWidgetLookup) const override;

	virtual void UpdateGraphNode() override;

	// Calculate position for multiple nodes to be placed between a start and end point, by providing this nodes index and max expected nodes 
	void PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const;

	void InitializeImages();

	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);

protected:
	FSlateColor GetEdgeColor() const;

	const FSlateBrush* GetEdgeImage() const;

	EVisibility GetEdgeImageVisibility() const;
	EVisibility GetEdgeTitleVisbility() const;
private:
	TSharedPtr<STextEntryPopup> TextEntryWidget;
	TSharedPtr<SHorizontalBox> ImageBoxWidget;
	TArray<const FSlateBrush*> ImageBrushes;
};
