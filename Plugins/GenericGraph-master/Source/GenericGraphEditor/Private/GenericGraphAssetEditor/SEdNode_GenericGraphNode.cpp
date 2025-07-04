#include "GenericGraphAssetEditor/SEdNode_GenericGraphNode.h"
#include "GenericGraphAssetEditor/Colors_GenericGraph.h"
#include "GenericGraphAssetEditor/EdGraph_GenericGraph.h"
#include "GenericGraphAssetEditor/EdNode_GenericGraphNode.h"
#include "GenericGraphAssetEditor/GenericGraphDragConnection.h"
#include "GenericGraphAssetEditor/SGenericComponentList.h"
#include "GenericSubobjectEditor/SGenericSubobjectEditor.h"
#include "GenericGraphEditorPCH.h"

#include "Widgets/Text/SInlineEditableTextBlock.h"
// #include "SLevelOfDetailBranchNode.h"
#include "SCommentBubble.h"
#include "SlateOptMacros.h"
#include "SGraphPin.h"
#include "GraphEditorSettings.h"

#define LOCTEXT_NAMESPACE "EdNode_GenericGraph"

//////////////////////////////////////////////////////////////////////////
class SGenericGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SGenericGraphPin) {}
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
			.BorderImage(this, &SGenericGraphPin::GetPinBorder)
			.BorderBackgroundColor(this, &SGenericGraphPin::GetPinColor)
			.OnMouseButtonDown(this, &SGenericGraphPin::OnPinMouseDown)
			.Cursor(this, &SGenericGraphPin::GetPinCursor)
			.Padding(FMargin(5.0f))
		);
	}

protected:
	virtual FSlateColor GetPinColor() const override
	{
		return GenericGraphColors::Pin::Default;
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
		FGenericGraphDragConnection::FDraggedPinTable PinHandles;
		PinHandles.Reserve(InStartingPins.Num());
		// since the graph can be refreshed and pins can be reconstructed/replaced 
		// behind the scenes, the DragDropOperation holds onto FGraphPinHandles 
		// instead of direct widgets/graph-pins
		for (const TSharedRef<SGraphPin>& PinWidget : InStartingPins)
		{
			PinHandles.Add(PinWidget->GetPinObj());
		}

		return FGenericGraphDragConnection::New(InGraphPanel, PinHandles);
	}

};

//////////////////////////////////////////////////////////////////////////
void SEdNode_GenericGraphNode::Construct(const FArguments& InArgs, UEdNode_GenericGraphNode* InNode)
{
	GraphNode = InNode;

	UpdateGraphNode();

	InNode->SEdNode = this;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SEdNode_GenericGraphNode::UpdateGraphNode()
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
	UEdNode_GenericGraphNode* EdNode = Cast<UEdNode_GenericGraphNode>(GraphNode);
	UGenericGraphNode* GenericGraphNode = EdNode->GenericGraphNode ? EdNode->GenericGraphNode : nullptr;

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0.0f)
			.BorderBackgroundColor(this, &SEdNode_GenericGraphNode::GetBorderBackgroundColor)
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
					.Padding(6.0f)
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
								.BackgroundColor(this, &SEdNode_GenericGraphNode::GetErrorColor)
								.ToolTipText(this, &SEdNode_GenericGraphNode::GetErrorMsgToolTip)
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
							.FillWidth(1.0)
							[
								SNew(SVerticalBox)
								+ SVerticalBox::Slot()
								.AutoHeight()
								[
									SAssignNew(InlineEditableText, SInlineEditableTextBlock)
									.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
									.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
									.OnVerifyTextChanged(this, &SEdNode_GenericGraphNode::OnVerifyNameTextChanged)
									.OnTextCommitted(this, &SEdNode_GenericGraphNode::OnNameTextCommited)
									.IsReadOnly(this, &SEdNode_GenericGraphNode::IsNameReadOnly)
									.IsSelected(this, &SEdNode_GenericGraphNode::IsSelectedExclusively)
								]
								+ SVerticalBox::Slot()
								.AutoHeight()
								[
									NodeTitle.ToSharedRef()
								]
							]
						]

						// Body
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SGenericComponentList, GenericGraphNode)
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

void SEdNode_GenericGraphNode::CreatePinWidgets()
{
	UEdNode_GenericGraphNode* StateNode = CastChecked<UEdNode_GenericGraphNode>(GraphNode);

	for (int32 PinIdx = 0; PinIdx < StateNode->Pins.Num(); PinIdx++)
	{
		UEdGraphPin* MyPin = StateNode->Pins[PinIdx];
		if (!MyPin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SGenericGraphPin, MyPin);

			AddPin(NewPin.ToSharedRef());
		}
	}
}

void SEdNode_GenericGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility( TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced) );
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

TSharedRef<SWidget> SEdNode_GenericGraphNode::GenerateComponentPicker()
{
	// FOnClassPicked OnPicked(FOnClassPicked::CreateSP(this, &SEdNode_GenericGraphNode::OnComponentPicked));

	return SNew(SBox);
		/* .WidthOverride(280.0f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.MaxHeight(500.0f)
			[
				FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer").CreateClassViewer(ComponentClassViewerOptions, OnPicked)
			]
		]; // */
}

void SEdNode_GenericGraphNode::SendToObjects(const FString& NewValue)
{
	if (PropertyEditor.IsValid())
	{
		// const TSharedRef<IPropertyHandle> PropertyHandle = PropertyEditor->GetPropertyHandle();
		// PropertyHandle->SetValueFromFormattedString(NewValue);
	}
	else if (!NewValue.IsEmpty() && NewValue != TEXT("None"))
	{
		auto FindOrLoadClass = [](const FString& ClassName) {
			UClass* Class = UClass::TryFindTypeSlow<UClass>(ClassName, EFindFirstObjectOptions::EnsureIfAmbiguous);

			if (!Class)
			{
				Class = LoadObject<UClass>(nullptr, *ClassName);
			}

			return Class;
		};

		const UClass* NewClass = FindOrLoadClass(NewValue);
		OnSetClass.Execute(NewClass);
	}
	else
	{
		OnSetClass.Execute(nullptr);
	}
}

void SEdNode_GenericGraphNode::CreateClassFilterObtions()
{
	/*ComponentClassViewerOptions.bShowBackgroundBorder = false;
	ComponentClassViewerOptions.bShowUnloadedBlueprints = true;

	const bool bAllowNone = false;
	const bool bIsBlueprintBaseOnly = false;
	const bool bAllowOnlyPlaceable = false;
	const bool bShowDisplayNames = false;
	const bool bShowTree = false;
	const bool bShowViewOptions = false;
	TArray<TSharedRef<IClassViewerFilter>> InClassFilters;
	InClassFilters.Add(MakeShared<FClassViewerFilter>(FClassViewerInitializationOptions()));

	ComponentClassViewerOptions.bShowNoneOption = bAllowNone;
	ComponentClassViewerOptions.bIsBlueprintBaseOnly = bIsBlueprintBaseOnly;
	ComponentClassViewerOptions.bIsPlaceableOnly = bAllowOnlyPlaceable;
	ComponentClassViewerOptions.NameTypeToDisplay = (bShowDisplayNames ? EClassViewerNameTypeToDisplay::DisplayName : EClassViewerNameTypeToDisplay::ClassName);
	ComponentClassViewerOptions.DisplayMode = bShowTree ? EClassViewerDisplayMode::TreeView : EClassViewerDisplayMode::ListView;
	ComponentClassViewerOptions.bAllowViewOptions = bShowViewOptions;
	ComponentClassViewerOptions.ClassFilters.Append(InClassFilters);*/

	/*TSharedRef<FPropertyEditorClassFilter> PropEdClassFilter = MakeShared<FPropertyEditorClassFilter>();
	PropEdClassFilter->ClassPropertyMetaClass = MetaClass;
	PropEdClassFilter->InterfaceThatMustBeImplemented = RequiredInterface;
	PropEdClassFilter->bAllowAbstract = bAllowAbstract;
	PropEdClassFilter->AllowedClassFilters = AllowedClassFilters;
	PropEdClassFilter->DisallowedClassFilters = DisallowedClassFilters;

	ComponentClassViewerOptions.ClassFilters.Add(PropEdClassFilter);*/
}

TSharedRef<ITableRow> SEdNode_GenericGraphNode::OnGenerateRowForList(TSharedPtr<FText> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	// Create the row
	if (!Item.IsValid()) return SNew(STableRow< TSharedPtr<FText> >, OwnerTable);

	return
		SNew(STableRow< TSharedPtr<FText> >, OwnerTable)
		.Padding(2.0f)
		[
			SNew(STextBlock)
			.Text(*Item.Get())
		];
}

bool SEdNode_GenericGraphNode::IsNameReadOnly() const
{
	UEdNode_GenericGraphNode* EdNode_Node = Cast<UEdNode_GenericGraphNode>(GraphNode);
	check(EdNode_Node != nullptr);

	UGenericGraph* GenericGraph = EdNode_Node->GenericGraphNode->Graph;
	check(GenericGraph != nullptr);

	return (!GenericGraph->bCanRenameNode || !EdNode_Node->GenericGraphNode->IsNameEditable()) || SGraphNode::IsNameReadOnly();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SEdNode_GenericGraphNode::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	SGraphNode::OnNameTextCommited(InText, CommitInfo);

	UEdNode_GenericGraphNode* MyNode = CastChecked<UEdNode_GenericGraphNode>(GraphNode);

	if (MyNode != nullptr && MyNode->GenericGraphNode != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("GenericGraphEditorRenameNode", "Generic Graph Editor: Rename Node"));
		MyNode->Modify();
		MyNode->GenericGraphNode->Modify();
		MyNode->GenericGraphNode->SetNodeTitle(InText);
		UpdateGraphNode();
	}
}

FSlateColor SEdNode_GenericGraphNode::GetBorderBackgroundColor() const
{
	UEdNode_GenericGraphNode* MyNode = CastChecked<UEdNode_GenericGraphNode>(GraphNode);
	return MyNode ? MyNode->GetBackgroundColor() : GenericGraphColors::NodeBorder::HighlightAbortRange0;
}

FSlateColor SEdNode_GenericGraphNode::GetBackgroundColor() const
{
	return GenericGraphColors::NodeBody::Default;
}

EVisibility SEdNode_GenericGraphNode::GetDragOverMarkerVisibility() const
{
	return EVisibility::Visible;
}

const FSlateBrush* SEdNode_GenericGraphNode::GetNameIcon() const
{
	return FAppStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Icon"));
}

const TArray<TSharedPtr<FText>>* SEdNode_GenericGraphNode::GetComponentItems()
{
	UEdNode_GenericGraphNode* MyNode = CastChecked<UEdNode_GenericGraphNode>(GraphNode);
	if (MyNode != nullptr && MyNode->GenericGraphNode != nullptr)
	{
		TArray<UGenericGraphNodeComponent*> Components = MyNode->GenericGraphNode->OwnedComponents;
		for (UGenericGraphNodeComponent* Component : Components)
		{
			FString StringName;
			Component->GetName(StringName);
			ListItems.Add(MakeShared<FText>(FText::FromString(StringName)));
		}
	}

	return &ListItems;
}

#undef LOCTEXT_NAMESPACE
