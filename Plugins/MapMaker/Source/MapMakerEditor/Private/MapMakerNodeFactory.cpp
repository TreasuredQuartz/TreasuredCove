// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerNodeFactory.h"
#include "MapMakerAssetEditor/EdNode_MapMakerNode.h"
#include "MapMakerAssetEditor/EdNode_MapMakerEdge.h"

TSharedPtr<class SGraphNode> FGraphPanelNodeFactory_MapMaker::CreateNode(UEdGraphNode* Node) const
{
	if (UEdNode_MapMakerNode* EdNode_GraphNode = Cast<UEdNode_MapMakerNode>(Node))
	{
		return SNew(SEdNode_MapMakerNode, EdNode_GraphNode);
	}
	else if (UEdNode_MapMakerEdge* EdNode_Edge = Cast<UEdNode_MapMakerEdge>(Node))
	{
		return SNew(SEdNode_MapMakerEdge, EdNode_Edge);
	}
	return nullptr;
}