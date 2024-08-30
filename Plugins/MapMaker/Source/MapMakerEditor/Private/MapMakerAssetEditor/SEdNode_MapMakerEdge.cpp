// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerAssetEditor/SEdNode_MapMakerEdge.h"
#include "MapMakerAssetEditor/EdNode_MapMakerEdge.h"
#include "MapMakerAssetEditor/EdNode_MapMakerNode.h"
#include "MapMakerEditorPCH.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "MapMakerAssetEditor/ConnectionDrawingPolicy_MapMaker.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#define LOCTEXT_NAMESPACE "SMapMakerEdge"

void SEdNode_MapMakerEdge::Construct(const FArguments& InArgs, UEdNode_MapMakerEdge* InNode)
{
	this->GraphNode = InNode;
	this->UpdateGraphNode();
}

bool SEdNode_MapMakerEdge::RequiresSecondPassLayout() const
{
	return true;
}

void SEdNode_MapMakerEdge::PerformSecondPassLayout(const TMap< UObject*, TSharedRef<SNode> >& NodeToWidgetLookup) const
{
	UEdNode_MapMakerEdge* EdgeNode = CastChecked<UEdNode_MapMakerEdge>(GraphNode);

	FGeometry StartGeom;
	FGeometry EndGeom;

	UEdNode_MapMakerNode* Start = EdgeNode->GetStartNode();
	UEdNode_MapMakerNode* End = EdgeNode->GetEndNode();
	if (Start != nullptr && End != nullptr)
	{
		const TSharedRef<SNode>* pFromWidget = NodeToWidgetLookup.Find(Start);
		const TSharedRef<SNode>* pToWidget = NodeToWidgetLookup.Find(End);
		if (pFromWidget != nullptr && pToWidget != nullptr)
		{
			const TSharedRef<SNode>& FromWidget = *pFromWidget;
			const TSharedRef<SNode>& ToWidget = *pToWidget;

			StartGeom = FGeometry(FVector2D(Start->NodePosX, Start->NodePosY), FVector2D::ZeroVector, FromWidget->GetDesiredSize(), 1.0f);
			EndGeom = FGeometry(FVector2D(End->NodePosX, End->NodePosY), FVector2D::ZeroVector, ToWidget->GetDesiredSize(), 1.0f);
		}
	}

	PositionBetweenTwoNodesWithOffset(StartGeom, EndGeom, 0, 1);
}

void SEdNode_MapMakerEdge::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	SGraphNode::OnNameTextCommited(InText, CommitInfo);

	UEdNode_MapMakerEdge* MyNode = CastChecked<UEdNode_MapMakerEdge>(GraphNode);

	if (MyNode != nullptr && MyNode->MapMakerEdge != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("MapMakerEditorRenameEdge", "Map Maker Editor: Rename Edge"));
		MyNode->Modify();
		MyNode->MapMakerEdge->SetNodeTitle(InText);
		UpdateGraphNode();
	}
}

void SEdNode_MapMakerEdge::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SOverlay)
				+ SOverlay::Slot()
				[
					SNew(SImage)
						.Image(FAppStyle::GetBrush("Graph.TransitionNode.ColorSpill"))
						.ColorAndOpacity(this, &SEdNode_MapMakerEdge::GetEdgeColor)
						.RenderTransform(FSlateRenderTransform(FQuat2f(0.8), FVector2D(64,-32)))
				]
				+ SOverlay::Slot()
				[
					SAssignNew(ImageBoxWidget, SHorizontalBox)
				]

				+ SOverlay::Slot()
				.Padding(FMargin(4.0f, 4.0f, 4.0f, 4.0f))
				[
					SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Center)
						.AutoHeight()
						[
							SAssignNew(InlineEditableText, SInlineEditableTextBlock)
								.ColorAndOpacity(FLinearColor::Black)
								.Visibility(this, &SEdNode_MapMakerEdge::GetEdgeTitleVisbility)
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
								.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
								.OnTextCommitted(this, &SEdNode_MapMakerEdge::OnNameTextCommited)
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							NodeTitle.ToSharedRef()
						]

				]
		];

	InitializeImages();
}

void SEdNode_MapMakerEdge::InitializeImages()
{
	UEdNode_MapMakerEdge* EdgeNode = CastChecked<UEdNode_MapMakerEdge>(GraphNode);
	if (EdgeNode != nullptr && EdgeNode->MapMakerEdge != nullptr && EdgeNode->MapMakerEdge->Graph && ImageBoxWidget.IsValid())
	{
		const FGameplayTagContainer& OurTags = EdgeNode->MapMakerEdge->GetEdgeTags();
		const TMap<FGameplayTag, FSlateBrush>& TagIcons = EdgeNode->MapMakerEdge->Graph->TagIcons;
		TArray<FGameplayTag> Keys;
		TagIcons.GetKeys(Keys);
		
		ImageBoxWidget.Get()->ClearChildren();
		ImageBrushes.Empty(OurTags.Num());
		for (FGameplayTag OurTag : OurTags)
		{
			if (Keys.Contains(OurTag))
			{
				const FSlateBrush* NewImageBrush = &(TagIcons[OurTag]);
				ImageBrushes.Add(NewImageBrush);

				ImageBoxWidget->AddSlot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(SImage)
							.Image(NewImageBrush)
							.ColorAndOpacity(FSlateColor(FLinearColor::Black))
					]; 
			}
		}
	}
}

void SEdNode_MapMakerEdge::PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const
{
	// Get a reasonable seed point (halfway between the boxes)
	const FVector2D StartCenter = FGeometryHelper::CenterOf(StartGeom);
	const FVector2D EndCenter = FGeometryHelper::CenterOf(EndGeom);
	const FVector2D SeedPoint = (StartCenter + EndCenter) * 0.5f;

	// Find the (approximate) closest points between the two boxes
	const FVector2D StartAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(StartGeom, SeedPoint);
	const FVector2D EndAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(EndGeom, SeedPoint);

	// Position ourselves halfway along the connecting line between the nodes, elevated away perpendicular to the direction of the line
	const float Height = 30.0f;

	const FVector2D DesiredNodeSize = GetDesiredSize();

	FVector2D DeltaPos(EndAnchorPoint - StartAnchorPoint);

	if (DeltaPos.IsNearlyZero())
	{
		DeltaPos = FVector2D(10.0f, 0.0f);
	}

	const FVector2D Normal = FVector2D(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	const FVector2D NewCenter = StartAnchorPoint + (0.5f * DeltaPos) + (Height * Normal);

	FVector2D DeltaNormal = DeltaPos.GetSafeNormal();

	// Calculate node offset in the case of multiple transitions between the same two nodes
	// MultiNodeOffset: the offset where 0 is the centre of the transition, -1 is 1 <size of node>
	// towards the PrevStateNode and +1 is 1 <size of node> towards the NextStateNode.

	const float MutliNodeSpace = 0.2f; // Space between multiple transition nodes (in units of <size of node> )
	const float MultiNodeStep = (1.f + MutliNodeSpace); //Step between node centres (Size of node + size of node spacer)

	const float MultiNodeStart = -((MaxNodes - 1) * MultiNodeStep) / 2.f;
	const float MultiNodeOffset = MultiNodeStart + (NodeIndex * MultiNodeStep);

	// Now we need to adjust the new center by the node size, zoom factor and multi node offset
	const FVector2D NewCorner = NewCenter - (0.5f * DesiredNodeSize) + (DeltaNormal * MultiNodeOffset * DesiredNodeSize.Size());

	GraphNode->NodePosX = NewCorner.X;
	GraphNode->NodePosY = NewCorner.Y;
}

FSlateColor SEdNode_MapMakerEdge::GetEdgeColor() const
{
	UEdNode_MapMakerEdge* EdgeNode = CastChecked<UEdNode_MapMakerEdge>(GraphNode);
	if (EdgeNode != nullptr && EdgeNode->MapMakerEdge != nullptr)
	{
		return EdgeNode->MapMakerEdge->GetEdgeColour();
	}
	return FLinearColor(0.9f, 0.9f, 0.9f, 1.0f);
}

const FSlateBrush* SEdNode_MapMakerEdge::GetEdgeImage() const
{
	return new FSlateBrush();
}

EVisibility SEdNode_MapMakerEdge::GetEdgeImageVisibility() const
{
	UEdNode_MapMakerEdge* EdgeNode = CastChecked<UEdNode_MapMakerEdge>(GraphNode);
	if (EdgeNode && EdgeNode->MapMakerEdge && EdgeNode->MapMakerEdge->bShouldDrawTitle)
		return EVisibility::Hidden;

	return EVisibility::Visible;
}

EVisibility SEdNode_MapMakerEdge::GetEdgeTitleVisbility() const
{
	UEdNode_MapMakerEdge* EdgeNode = CastChecked<UEdNode_MapMakerEdge>(GraphNode);
	if (EdgeNode && EdgeNode->MapMakerEdge && EdgeNode->MapMakerEdge->bShouldDrawTitle)
		return EVisibility::Visible;

	return EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE