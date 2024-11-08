// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerAssetEditor/EdNode_MapMakerNode.h"
#include "MapMakerEditorPCH.h"
#include "EdGraph_MapMaker.h"

#define LOCTEXT_NAMESPACE "EdNode_MapMaker"

UEdNode_MapMakerNode::UEdNode_MapMakerNode()
{
	bCanRenameNode = true;
}

UEdNode_MapMakerNode::~UEdNode_MapMakerNode()
{

}

void UEdNode_MapMakerNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, "MultipleNodes", FName(), TEXT("In"));
	CreatePin(EGPD_Output, "MultipleNodes", FName(), TEXT("Out"));
}

UEdGraph_MapMaker* UEdNode_MapMakerNode::GetMapMakerEdGraph()
{
	return Cast<UEdGraph_MapMaker>(GetGraph());
}

FText UEdNode_MapMakerNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (MapMakerNode == nullptr)
	{
		return Super::GetNodeTitle(TitleType);
	}
	else
	{
		return MapMakerNode->GetNodeTitle();
	}
}

void UEdNode_MapMakerNode::PrepareForCopying()
{
	MapMakerNode->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
}

void UEdNode_MapMakerNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	if (FromPin != nullptr)
	{
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}
}

void UEdNode_MapMakerNode::SetMapMakerNode(UMapMakerNode* InNode)
{
	MapMakerNode = InNode;
}

FLinearColor UEdNode_MapMakerNode::GetBackgroundColor() const
{
	return MapMakerNode == nullptr ? FLinearColor::Black : MapMakerNode->GetBackgroundColor();
}

FVector2D UEdNode_MapMakerNode::GetSize() const
{
	return MapMakerNode == nullptr ? FVector2D(100,100) : MapMakerNode->GetSize();
}

UEdGraphPin* UEdNode_MapMakerNode::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* UEdNode_MapMakerNode::GetOutputPin() const
{
	return Pins[1];
}

void UEdNode_MapMakerNode::PostEditUndo()
{
	UEdGraphNode::PostEditUndo();
}

#undef LOCTEXT_NAMESPACE