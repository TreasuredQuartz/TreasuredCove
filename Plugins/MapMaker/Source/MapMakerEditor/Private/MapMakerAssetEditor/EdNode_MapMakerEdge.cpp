// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerAssetEditor/EdNode_MapMakerEdge.h"
#include "MapMakerEdge.h"
#include "MapMakerAssetEditor/EdNode_MapMakerNode.h"

#define LOCTEXT_NAMESPACE "EdNode_MapMakerEdge"

UEdNode_MapMakerEdge::UEdNode_MapMakerEdge()
{
	bCanRenameNode = true;
}

void UEdNode_MapMakerEdge::SetEdge(UMapMakerEdge* Edge)
{
	MapMakerEdge = Edge;
}

void UEdNode_MapMakerEdge::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, TEXT("Edge"), FName(), TEXT("In"));
	Inputs->bHidden = true;
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, TEXT("Edge"), FName(), TEXT("Out"));
	Outputs->bHidden = true;
}

FText UEdNode_MapMakerEdge::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (MapMakerEdge)
	{
		return MapMakerEdge->GetNodeTitle();
	}
	return FText();
}

void UEdNode_MapMakerEdge::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin->LinkedTo.Num() == 0)
	{
		// Commit suicide; transitions must always have an input and output connection
		Modify();

		// Our parent graph will have our graph in SubGraphs so needs to be modified to record that.
		if (UEdGraph* ParentGraph = GetGraph())
		{
			ParentGraph->Modify();
		}

		DestroyNode();
	}
}

void UEdNode_MapMakerEdge::PrepareForCopying()
{
	MapMakerEdge->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
}

void UEdNode_MapMakerEdge::CreateConnections(UEdNode_MapMakerNode* Start, UEdNode_MapMakerNode* End)
{
	Pins[0]->Modify();
	Pins[0]->LinkedTo.Empty();

	Start->GetOutputPin()->Modify();
	Pins[0]->MakeLinkTo(Start->GetOutputPin());

	// This to next
	Pins[1]->Modify();
	Pins[1]->LinkedTo.Empty();

	End->GetInputPin()->Modify();
	Pins[1]->MakeLinkTo(End->GetInputPin());
}

UEdNode_MapMakerNode* UEdNode_MapMakerEdge::GetStartNode()
{
	if (Pins[0]->LinkedTo.Num() > 0)
	{
		return Cast<UEdNode_MapMakerNode>(Pins[0]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return nullptr;
	}
}

UEdNode_MapMakerNode* UEdNode_MapMakerEdge::GetEndNode()
{
	if (Pins[1]->LinkedTo.Num() > 0)
	{
		return Cast<UEdNode_MapMakerNode>(Pins[1]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return nullptr;
	}
}

#undef LOCTEXT_NAMESPACE