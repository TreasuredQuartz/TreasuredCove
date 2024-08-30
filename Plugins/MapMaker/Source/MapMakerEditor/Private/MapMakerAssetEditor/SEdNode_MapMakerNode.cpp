// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerAssetEditor/SEdNode_MapMakerNode.h"
#include "MapMakerAssetEditor/EdNode_MapMakerNode.h"
#include "MapMakerAssetEditor/MapMakerDragConnection.h"
#include "MapMakerAssetEditor/SResizeableBox.h"
#include "MapMakerEditorPCH.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "SCommentBubble.h"
#include "SlateOptMacros.h"
#include "SGraphPin.h"
#include "GraphEditorSettings.h"
#include "SGraphPanel.h"


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
													 SNew(SImage)
														 .Image(this, &SEdNode_MapMakerNode::GetRoomBrush)
														 .ColorAndOpacity(FSlateColor(FLinearColor(0.2, 0.2, 0.2, 0.2)))
														 .Cursor(EMouseCursor::Default)
														 .Visibility(EVisibility::Visible)
														 .OnMouseButtonDown_Lambda([](const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)->FReply { return FReply::Handled(); })
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
		const FScopedTransaction Transaction(LOCTEXT("MapMakerEditorRenameNode", "Map Maker Editor: Rename Node"));
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

const FSlateBrush* SEdNode_MapMakerNode::GetNameIcon() const
{
	return FAppStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Icon"));
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

#undef LOCTEXT_NAMESPACE