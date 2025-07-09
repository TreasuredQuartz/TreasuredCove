// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerAssetEditor/SRoomPreview_AssetEditor.h"
#include "MapMakerAssetEditor/SResizeableBox.h"
#include "MapMakerAssetEditor/SMapMakerAsset.h"
#include "MapMakerAssetEditor/EdNode_MapMakerNode.h"
#include "MapMakerAssetEditor/EdGraph_MapMaker.h"
#include "MapMakerEditorPCH.h"

#include "Widgets/SCanvas.h"

#define LOCTEXT_NAMESPACE "RoomPreview_AssetEditor"

void SRoomPreview_AssetEditor::Construct(const FArguments& InArgs, UEdGraphNode* InNode)
{
	GraphNode = InNode;

	UpdateGraphNode();
}

void SRoomPreview_AssetEditor::UpdateGraphNode()
{
	ChildSlot
		[
			SNew(SResizeableBox)
				.IsEnabled(true)
				.OnResized(this, &SRoomPreview_AssetEditor::OnResized)
				.Size(this, &SRoomPreview_AssetEditor::GetSize)
				.Visibility(EVisibility::Visible)
				[
					SAssignNew(AssetCanvas, SCanvas)
						.Visibility(EVisibility::Visible)

						+ SCanvas::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SImage)
								.Image(this, &SRoomPreview_AssetEditor::GetRoomBrush)
								.ColorAndOpacity(FSlateColor(FLinearColor(0.2, 0.2, 0.2, 0.2)))
								.Cursor(EMouseCursor::GrabHand)
								.Visibility(EVisibility::Visible)
								.OnMouseButtonDown(this, &SRoomPreview_AssetEditor::OnMouseButtonDown)
						]
				]
		];
	
	CreateAssetWidgets();
}

void SRoomPreview_AssetEditor::CreateAssetWidgets()
{
	UEdNode_MapMakerNode* StateNode = CastChecked<UEdNode_MapMakerNode>(GraphNode);

	for (int32 AssetIdx = 0; AssetIdx < StateNode->MapMakerNode->Assets.Num(); AssetIdx++)
	{
		UMapMakerAsset* MyAsset = StateNode->MapMakerNode->Assets[AssetIdx];
		if (MyAsset)
		{
			TSharedPtr<SMapMakerAsset> NewAsset = SNew(SMapMakerAsset, MyAsset);

			AddAsset(NewAsset.ToSharedRef());
		}
	}
}

void SRoomPreview_AssetEditor::AddAsset(const TSharedRef<SMapMakerAsset>& AssetToAdd)
{
	if (AssetCanvas)
	{
		FVector AssetLocation = FVector::ZeroVector;
		FVector2D AssetSize = FVector2D::ZeroVector;
		if (AssetToAdd->Asset)
		{
			AssetLocation = AssetToAdd->Asset->AssetTransform.GetLocation();
			FVector Scale3D = AssetToAdd->Asset->AssetTransform.GetScale3D();
			AssetSize = FVector2D(Scale3D.X, Scale3D.Y);
		}

		SCanvas::FSlot::FSlotArguments& TMP_SLOT = AssetCanvas->AddSlot()
			.Size(AssetSize)
			.Position(FVector2D(AssetLocation.X, AssetLocation.Y))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				(AssetToAdd)
			];
	}
}

void SRoomPreview_AssetEditor::OnResized(const FVector2D& Delta)
{
	UEdNode_MapMakerNode* MyNode = CastChecked<UEdNode_MapMakerNode>(GraphNode);

	if (MyNode != nullptr && MyNode->MapMakerNode != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("MapMakerEditorResized", "Map Maker Editor: Resized Node"));
		MyNode->Modify();
		MyNode->MapMakerNode->Modify();
		MyNode->MapMakerNode->SetNodeSize(Delta);

		UpdateGraphNode();
	}
}

FReply SRoomPreview_AssetEditor::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FKey MouseButton = MouseEvent.GetEffectingButton();
	if (MouseButton == EKeys::LeftMouseButton)
	{
		bBrushActivated = true;
		return FReply::Handled();
	}


	return FReply::Unhandled();
}

FReply SRoomPreview_AssetEditor::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bBrushActivated = false;

	return FReply::Handled();
}

FReply SRoomPreview_AssetEditor::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UEdNode_MapMakerNode* MyEdNode = CastChecked<UEdNode_MapMakerNode>(GraphNode);
	if (!MyEdNode || !MyEdNode->MapMakerNode) return FReply::Unhandled();

	if (bBrushActivated)
	{
		if (UMapMakerAsset* BrushAsset = GetCurrentAsset())
		{
			UMapMakerNode* MyNode = MyEdNode->MapMakerNode;
			UMapMakerAsset* Asset = NewObject<UMapMakerAsset>(MyNode, FName(), RF_NoFlags, BrushAsset);

			float GridCellSize = MyNode->GridCellSize;
			const FVector2D NodeLocation = FVector2D(MyEdNode->NodePosX, MyEdNode->NodePosY);
			const FVector2D MouseLocation = MouseEvent.GetScreenSpacePosition();
			const FVector2D AssetGridLocation = MyGeometry.AbsoluteToLocal(MouseLocation);
			const float AssetOffset = (GridCellSize / 2);

			if (AssetGridLocation.ComponentwiseAllGreaterThan(FVector2D(GridCellSize, GridCellSize)) && AssetGridLocation.ComponentwiseAllLessThan(GetSize()))
			{
				auto SnapToGrid = [](double value, double size) { return FMath::Floor(value / size) * size; };
				const FVector AssetLocation = FVector(SnapToGrid(AssetGridLocation.X, GridCellSize), SnapToGrid(AssetGridLocation.Y, GridCellSize), 0);
				Asset->AssetTransform.SetLocation(AssetLocation);
				Asset->AssetTransform.SetScale3D(FVector(GridCellSize, GridCellSize, 0));

				if (MyNode->CanAddAsset(Asset))
				{
					const FScopedTransaction Transaction(LOCTEXT("MapMakerEditorAddAsset", "Map Maker Editor: Add Room Asset"));
					MyEdNode->Modify();
					MyNode->Modify();
					MyNode->AddAsset(Asset);
					LOG_INFO(TEXT("Asset added to node!"));
					UpdateGraphNode();
					return FReply::Handled();
				}
			}
		}
	}

	return FReply::Unhandled();
}

FVector2D SRoomPreview_AssetEditor::GetSize() const
{
	UEdNode_MapMakerNode* MyNode = CastChecked<UEdNode_MapMakerNode>(GraphNode);
	return MyNode ? MyNode->GetSize() : FVector2D(100, 100);
}

const FSlateBrush* SRoomPreview_AssetEditor::GetRoomBrush() const
{
	FSlateBrush* ImageBrush = new FSlateBrush();
	// ImageBrush->SetImageSize(GetSize());

	return ImageBrush;
}

UMapMakerAsset* SRoomPreview_AssetEditor::GetCurrentAsset() const
{
	UEdNode_MapMakerNode* MyNode = CastChecked<UEdNode_MapMakerNode>(GraphNode);
	return MyNode ? MyNode->GetMapMakerEdGraph()->GetMapMaker()->CurrentAsset : nullptr;
}

#undef LOCTEXT_NAMESPACE