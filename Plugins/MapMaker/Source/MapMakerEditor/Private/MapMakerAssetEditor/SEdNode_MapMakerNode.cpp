// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerAssetEditor/SEdNode_MapMakerNode.h"
#include "MapMakerAssetEditor/EdNode_MapMakerNode.h"
#include "MapMakerAssetEditor/EdGraph_MapMaker.h"
#include "MapMakerAssetEditor/MapMakerDragConnection.h"
#include "MapMakerAssetEditor/SResizeableBox.h"
#include "MapMakerStyle.h"

#include "MapMakerEditorPCH.h"
#include "GraphEditorSettings.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Widgets/SCanvas.h"
#include "SCommentBubble.h"
#include "SlateOptMacros.h"
#include "SGraphPin.h"
#include "SGraphPanel.h"
#include "SMapMakerAsset.h"


#define LOCTEXT_NAMESPACE "EdNode_MapMaker"

//////////////////////////////////////////////////////////////////////////
class SMapMakerPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SMapMakerPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		this->SetCursor(EMouseCursor::Default);

		bShowLabel = true;

		GraphPinObj = InPin;
		check(GraphPinObj != nullptr);

		const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
		check(Schema);

		SBorder::Construct(SBorder::FArguments()
			.BorderImage(this, &SMapMakerPin::GetPinBorder)
			.BorderBackgroundColor(this, &SMapMakerPin::GetPinColor)
			.OnMouseButtonDown(this, &SMapMakerPin::OnPinMouseDown)
			.Cursor(this, &SMapMakerPin::GetPinCursor)
			.Padding(FMargin(5.0f))
		);
	}

protected:
	virtual FSlateColor GetPinColor() const override
	{
		return FLinearColor(0.02f, 0.02f, 0.02f);
	}

	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override
	{
		return SNew(STextBlock);
	}

	const FSlateBrush* GetPinBorder() const
	{
		return FAppStyle::GetBrush(TEXT("Graph.StateNode.Body"));
	}

	virtual TSharedRef<FDragDropOperation> SpawnPinDragEvent(const TSharedRef<class SGraphPanel>& InGraphPanel, const TArray< TSharedRef<SGraphPin> >& InStartingPins) override
	{
		FMapMakerDragConnection::FDraggedPinTable PinHandles;
		PinHandles.Reserve(InStartingPins.Num());
		// since the graph can be refreshed and pins can be reconstructed/replaced 
		// behind the scenes, the DragDropOperation holds onto FGraphPinHandles 
		// instead of direct widgets/graph-pins
		for (const TSharedRef<SGraphPin>& PinWidget : InStartingPins)
		{
			PinHandles.Add(PinWidget->GetPinObj());
		}

		return FMapMakerDragConnection::New(InGraphPanel, PinHandles);
	}

};


//////////////////////////////////////////////////////////////////////////
void SEdNode_MapMakerNode::Construct(const FArguments& InArgs, UEdNode_MapMakerNode* InNode)
{
	GraphNode = InNode;
	UpdateGraphNode();
	InNode->SEdNode = this;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SEdNode_MapMakerNode::UpdateGraphNode()
{
	const FMargin NodePadding = FMargin(5);
	const FMargin NamePadding = FMargin(2);

	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	const FSlateBrush* NodeTypeIcon = GetNameIcon();
	const FButtonStyle* ClearButtonStyle = GetClearButtonStyle();
	
	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);
	TSharedPtr<SErrorText> ErrorText;
	TSharedPtr<SVerticalBox> NodeBody;
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
				.Padding(0.0f)
				.BorderBackgroundColor(this, &SEdNode_MapMakerNode::GetBorderBackgroundColor)
				[
					SNew(SOverlay)

						+ SOverlay::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SVerticalBox)

								// Input Pin Area
								+ SVerticalBox::Slot()
								.FillHeight(1)
								[
									SAssignNew(LeftNodeBox, SVerticalBox)
								]

								// Output Pin Area	
								+ SVerticalBox::Slot()
								.FillHeight(1)
								[
									SAssignNew(RightNodeBox, SVerticalBox)
								]
						]

						+ SOverlay::Slot()
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.Padding(8.0f)
						[
							SNew(SBorder)
								.BorderImage(FAppStyle::GetBrush("Graph.StateNode.ColorSpill"))
								.BorderBackgroundColor(TitleShadowColor)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.Visibility(EVisibility::SelfHitTestInvisible)
								.Padding(0.0f)
								[
									SAssignNew(NodeBody, SVerticalBox)

										// Title
										+ SVerticalBox::Slot()
										.AutoHeight()
										[
											SNew(SHorizontalBox)

												// Error message
												+ SHorizontalBox::Slot()
												.AutoWidth()
												[
													SAssignNew(ErrorText, SErrorText)
														.BackgroundColor(this, &SEdNode_MapMakerNode::GetErrorColor)
														.ToolTipText(this, &SEdNode_MapMakerNode::GetErrorMsgToolTip)
												]

												// Icon
												+ SHorizontalBox::Slot()
												.AutoWidth()
												.VAlign(VAlign_Center)
												[
													SNew(SImage)
														.Image(NodeTypeIcon)
												]

												// Node Title
												+ SHorizontalBox::Slot()
												.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
												.HAlign(HAlign_Fill)
												[
													SNew(SVerticalBox)
														+ SVerticalBox::Slot()
														.AutoHeight()
														[
															SAssignNew(InlineEditableText, SInlineEditableTextBlock)
																.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
																.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
																.OnVerifyTextChanged(this, &SEdNode_MapMakerNode::OnVerifyNameTextChanged)
																.OnTextCommitted(this, &SEdNode_MapMakerNode::OnNameTextCommited)
																.IsReadOnly(this, &SEdNode_MapMakerNode::IsNameReadOnly)
																.IsSelected(this, &SEdNode_MapMakerNode::IsSelectedExclusively)
														]
														/*+ SVerticalBox::Slot()
														.AutoHeight()
														[
															NodeTitle.ToSharedRef()
														]*/

												] // */

												+ SHorizontalBox::Slot()
												.AutoWidth()
												.VAlign(VAlign_Center)
												[
													SNew(SButton)
														.OnClicked(this, &SEdNode_MapMakerNode::OnClearButtonClicked)
														.ButtonStyle(ClearButtonStyle)
												]
										]

										// Room
										+ SVerticalBox::Slot()
										.AutoHeight()
										[
											SNew(SResizeableBox)
												.IsEnabled(true)
												.OnResized(this, &SEdNode_MapMakerNode::OnResized)
												.Size(this, &SEdNode_MapMakerNode::GetSize)
												.Visibility(EVisibility::Visible)
												[
													SAssignNew(AssetCanvas, SCanvas)
														.Visibility(EVisibility::Visible)

														+ SCanvas::Slot()
														.HAlign(HAlign_Fill)
														.VAlign(VAlign_Fill)
														[
															SNew(SImage)
																.Image(this, &SEdNode_MapMakerNode::GetRoomBrush)
																.ColorAndOpacity(FSlateColor(FLinearColor(0.2, 0.2, 0.2, 0.2)))
																.Cursor(EMouseCursor::GrabHand)
																.Visibility(EVisibility::Visible)
																.OnMouseButtonDown(this, &SEdNode_MapMakerNode::OnMouseButtonDown)
														]
												]
										]
								]
						]
				]
		];

	// Create comment bubble
	TSharedPtr<SCommentBubble> CommentBubble;
	const FSlateColor CommentColor = GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;

	SAssignNew(CommentBubble, SCommentBubble)
		.GraphNode(GraphNode)
		.Text(this, &SGraphNode::GetNodeComment)
		.OnTextCommitted(this, &SGraphNode::OnCommentTextCommitted)
		.ColorAndOpacity(CommentColor)
		.AllowPinning(true)
		.EnableTitleBarBubble(true)
		.EnableBubbleCtrls(true)
		.GraphLOD(this, &SGraphNode::GetCurrentLOD)
		.IsGraphNodeHovered(this, &SGraphNode::IsHovered);

	GetOrAddSlot(ENodeZone::TopCenter)
		.SlotOffset(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetOffset))
		.SlotSize(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetSize))
		.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
		.VAlign(VAlign_Top)
		[
			CommentBubble.ToSharedRef()
		];

	ErrorReporting = ErrorText;
	ErrorReporting->SetError(ErrorMsg);
	CreateAssetWidgets();
	CreatePinWidgets();
}

void SEdNode_MapMakerNode::CreatePinWidgets()
{
	UEdNode_MapMakerNode* StateNode = CastChecked<UEdNode_MapMakerNode>(GraphNode);

	for (int32 PinIdx = 0; PinIdx < StateNode->Pins.Num(); PinIdx++)
	{
		UEdGraphPin* MyPin = StateNode->Pins[PinIdx];
		if (!MyPin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SMapMakerPin, MyPin);

			AddPin(NewPin.ToSharedRef());
		}
	}
}

void SEdNode_MapMakerNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
	}

	TSharedPtr<SVerticalBox> PinBox;
	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		PinBox = LeftNodeBox;
		InputPins.Add(PinToAdd);
	}
	else // Direction == EEdGraphPinDirection::EGPD_Output
	{
		PinBox = RightNodeBox;
		OutputPins.Add(PinToAdd);
	}

	if (PinBox)
	{
		PinBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.0f)
			//.Padding(6.0f, 0.0f)
			[
				PinToAdd
			];
	}
}

void SEdNode_MapMakerNode::CreateAssetWidgets()
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

void SEdNode_MapMakerNode::AddAsset(const TSharedRef<SMapMakerAsset>& AssetToAdd)
{
	// AssetToAdd->SetOwner(AsShared(this));

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
			[
				(AssetToAdd)
			];
	}
}

bool SEdNode_MapMakerNode::IsNameReadOnly() const
{
	UEdNode_MapMakerNode* EdNode_Node = Cast<UEdNode_MapMakerNode>(GraphNode);
	check(EdNode_Node != nullptr);

	UMapMaker* MapMaker = EdNode_Node->MapMakerNode->Graph;
	check(MapMaker != nullptr);

	return (!MapMaker->bCanRenameNode || !EdNode_Node->MapMakerNode->IsNameEditable()) || SGraphNode::IsNameReadOnly();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SEdNode_MapMakerNode::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	SGraphNode::OnNameTextCommited(InText, CommitInfo);

	UEdNode_MapMakerNode* MyNode = CastChecked<UEdNode_MapMakerNode>(GraphNode);

	if (MyNode != nullptr && MyNode->MapMakerNode != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("MapMakerEditorRenameNode", "Map Maker Editor: Rename Node"));
		MyNode->Modify();
		MyNode->MapMakerNode->Modify();
		MyNode->MapMakerNode->SetNodeTitle(InText);
		UpdateGraphNode();
	}
}

void SEdNode_MapMakerNode::OnResized(const FVector2D& Delta)
{
	UEdNode_MapMakerNode* MyNode = CastChecked<UEdNode_MapMakerNode>(GraphNode);

	if (MyNode != nullptr && MyNode->MapMakerNode != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("MapMakerEditorResized", "Map Maker Editor: Resized Node"));
		MyNode->Modify();
		MyNode->MapMakerNode->Modify();
		TSharedPtr<SGraphPanel> OwnerCanvas = OwnerGraphPanelPtr.Pin();
		if (!OwnerCanvas.IsValid()) return;

		//@TODO: NodeSpaceCoordinate != PanelCoordinate
		FVector2D GraphCoordToPanelCoord = (GetPosition() - OwnerCanvas->GetViewOffset()) * OwnerCanvas->GetZoomAmount();
		
		MyNode->MapMakerNode->SetNodeSize(Delta);

		UpdateGraphNode();
	}
}

FReply SEdNode_MapMakerNode::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FKey MouseButton = MouseEvent.GetEffectingButton();
	if (MouseButton == EKeys::LeftMouseButton)
	{
		bBrushActivated = true;
		return FReply::Handled();
	}
	

	return FReply::Unhandled();
}

FReply SEdNode_MapMakerNode::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) 
{
	bBrushActivated = false;

	return FReply::Handled();
}

FReply SEdNode_MapMakerNode::OnClearButtonClicked()
{
	UEdNode_MapMakerNode* MyNode = CastChecked<UEdNode_MapMakerNode>(GraphNode);

	if (MyNode != nullptr && MyNode->MapMakerNode != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("MapMakerEditorClearNode", "Map Maker Editor: Clear Node"));
		MyNode->Modify();
		MyNode->MapMakerNode->Modify();
		MyNode->MapMakerNode->ClearAssets();
		UpdateGraphNode();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FSlateColor SEdNode_MapMakerNode::GetBorderBackgroundColor() const
{
	UEdNode_MapMakerNode* MyNode = CastChecked<UEdNode_MapMakerNode>(GraphNode);
	return MyNode ? MyNode->GetBackgroundColor() : FLinearColor(0.0f, 0.22f, 0.4f);
}

FSlateColor SEdNode_MapMakerNode::GetBackgroundColor() const
{
	return FLinearColor(0.1f, 0.1f, 0.1f);;
}

EVisibility SEdNode_MapMakerNode::GetDragOverMarkerVisibility() const
{
	return EVisibility::Visible;
}

FReply SEdNode_MapMakerNode::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UEdNode_MapMakerNode* MyEdNode = CastChecked<UEdNode_MapMakerNode>(GraphNode);
	if (!MyEdNode || !MyEdNode->MapMakerNode) return FReply::Unhandled();

	if (bBrushActivated) {
		if (UMapMakerAsset* BrushAsset = GetCurrentAsset())
		{
			UMapMakerNode* MyNode = MyEdNode->MapMakerNode;
			UMapMakerAsset* Asset = NewObject<UMapMakerAsset>(MyNode, FName(), RF_NoFlags, BrushAsset);

			float GridCellSize = MyNode->GridCellSize;
			const FVector2D NodeLocation = FVector2D(MyEdNode->NodePosX, MyEdNode->NodePosY);
			const FVector2D MouseLocation = MouseEvent.GetScreenSpacePosition();
			const FVector2D AssetGridLocation = MyGeometry.AbsoluteToLocal(MouseLocation);
			auto SnapToGrid = [](double value, double size) {return std::floor(value / size) * size; };
			const FVector AssetLocation = FVector(SnapToGrid(AssetGridLocation.X - GridCellSize, GridCellSize / 2), SnapToGrid(AssetGridLocation.Y - GridCellSize, GridCellSize / 2), 0);
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

	return FReply::Unhandled();
}

const FSlateBrush* SEdNode_MapMakerNode::GetNameIcon() const
{
	return FAppStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Icon"));
}

const FButtonStyle* SEdNode_MapMakerNode::GetClearButtonStyle() const
{
	return &FMapMakerStyle::Get()->GetWidgetStyle<FButtonStyle>(TEXT("MMEditor.Graph.MMNode.Clear"));
}

FVector2D SEdNode_MapMakerNode::GetSize() const
{
	UEdNode_MapMakerNode* MyNode = CastChecked<UEdNode_MapMakerNode>(GraphNode);
	return MyNode ? MyNode->GetSize() : FVector2D(100,100);
}

const FSlateBrush* SEdNode_MapMakerNode::GetRoomBrush() const
{
	FSlateBrush* ImageBrush = new FSlateBrush();
	// ImageBrush->SetImageSize(GetSize());

	return ImageBrush;
}

UMapMakerAsset* SEdNode_MapMakerNode::GetCurrentAsset() const
{
	UEdNode_MapMakerNode* MyNode = CastChecked<UEdNode_MapMakerNode>(GraphNode);
	return MyNode ? MyNode->GetMapMakerEdGraph()->GetMapMaker()->CurrentAsset : nullptr;
}


#undef LOCTEXT_NAMESPACE