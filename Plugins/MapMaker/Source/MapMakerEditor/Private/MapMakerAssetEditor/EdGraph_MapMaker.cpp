// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerAssetEditor/EdGraph_MapMaker.h"
#include "MapMakerAssetEditor/EdNode_MapMakerNode.h"
#include "MapMakerAssetEditor/EdNode_MapMakerEdge.h"
#include "MapMakerAssetEditor/SEdNode_MapMakerNode.h"
#include "MapMakerEditorPCH.h"

UEdGraph_MapMaker::UEdGraph_MapMaker()
{

}

UEdGraph_MapMaker::~UEdGraph_MapMaker()
{

}

void UEdGraph_MapMaker::RebuildMapMaker()
{
	LOG_INFO(TEXT("UMapMakerEdGraph::RebuildMapMaker has been called"));

	UMapMaker* Graph = GetMapMaker();

	Clear();

	for (int i = 0; i < Nodes.Num(); ++i)
	{
		if (UEdNode_MapMakerNode* EdNode = Cast<UEdNode_MapMakerNode>(Nodes[i]))
		{
			if (EdNode->MapMakerNode == nullptr)
				continue;

			UMapMakerNode* MapMakerNode = EdNode->MapMakerNode;

			NodeMap.Add(MapMakerNode, EdNode);

			Graph->AllNodes.Add(MapMakerNode);

			for (int PinIdx = 0; PinIdx < EdNode->Pins.Num(); ++PinIdx)
			{
				UEdGraphPin* Pin = EdNode->Pins[PinIdx];

				if (Pin->Direction != EEdGraphPinDirection::EGPD_Output)
					continue;

				for (int LinkToIdx = 0; LinkToIdx < Pin->LinkedTo.Num(); ++LinkToIdx)
				{
					UMapMakerNode* ChildNode = nullptr;
					if (UEdNode_MapMakerNode* EdNode_Child = Cast<UEdNode_MapMakerNode>(Pin->LinkedTo[LinkToIdx]->GetOwningNode()))
					{
						ChildNode = EdNode_Child->MapMakerNode;
					}
					else if (UEdNode_MapMakerEdge* EdNode_Edge = Cast<UEdNode_MapMakerEdge>(Pin->LinkedTo[LinkToIdx]->GetOwningNode()))
					{
						UEdNode_MapMakerNode* Child = EdNode_Edge->GetEndNode();;
						if (Child != nullptr)
						{
							ChildNode = Child->MapMakerNode;
						}
					}

					if (ChildNode != nullptr)
					{
						MapMakerNode->ChildrenNodes.Add(ChildNode);

						ChildNode->ParentNodes.Add(MapMakerNode);
					}
					else
					{
						LOG_ERROR(TEXT("UEdGraph_MapMaker::RebuildMapMaker can't find child node"));
					}
				}
			}

			if (EdNode->SEdNode) EdNode->SEdNode->UpdateGraphNode();
		}
		else if (UEdNode_MapMakerEdge* EdgeNode = Cast<UEdNode_MapMakerEdge>(Nodes[i]))
		{
			UEdNode_MapMakerNode* StartNode = EdgeNode->GetStartNode();
			UEdNode_MapMakerNode* EndNode = EdgeNode->GetEndNode();
			UMapMakerEdge* Edge = EdgeNode->MapMakerEdge;

			if (StartNode == nullptr || EndNode == nullptr || Edge == nullptr)
			{
				LOG_ERROR(TEXT("UEdGraph_MapMaker::RebuildMapMaker add edge failed."));
				continue;
			}

			EdgeMap.Add(Edge, EdgeNode);

			Edge->Graph = Graph;
			Edge->Rename(nullptr, Graph, REN_DontCreateRedirectors | REN_DoNotDirty);
			Edge->StartNode = StartNode->MapMakerNode;
			Edge->EndNode = EndNode->MapMakerNode;
			Edge->StartNode->Edges.Add(Edge->EndNode, Edge);
		}
	}

	for (int i = 0; i < Graph->AllNodes.Num(); ++i)
	{
		UMapMakerNode* Node = Graph->AllNodes[i];
		if (Node->ParentNodes.Num() == 0)
		{
			Graph->RootNodes.Add(Node);

			SortNodes(Node);
		}

		Node->Graph = Graph;
		Node->Rename(nullptr, Graph, REN_DontCreateRedirectors | REN_DoNotDirty);
	}

	Graph->RootNodes.Sort([&](const UMapMakerNode& L, const UMapMakerNode& R)
	{
		UEdNode_MapMakerNode* EdNode_LNode = NodeMap[&L];
		UEdNode_MapMakerNode* EdNode_RNode = NodeMap[&R];
		return EdNode_LNode->NodePosX < EdNode_RNode->NodePosX;
	});
}

UMapMaker* UEdGraph_MapMaker::GetMapMaker() const
{
	return CastChecked<UMapMaker>(GetOuter());
}

bool UEdGraph_MapMaker::Modify(bool bAlwaysMarkDirty /*= true*/)
{
	bool Rtn = Super::Modify(bAlwaysMarkDirty);

	GetMapMaker()->Modify();

	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		Nodes[i]->Modify();
	}

	return Rtn;
}

void UEdGraph_MapMaker::Clear()
{
	UMapMaker* Graph = GetMapMaker();

	Graph->ClearGraph();
	NodeMap.Reset();
	EdgeMap.Reset();

	for (int i = 0; i < Nodes.Num(); ++i)
	{
		if (UEdNode_MapMakerNode* EdNode = Cast<UEdNode_MapMakerNode>(Nodes[i]))
		{
			UMapMakerNode* MapMakerNode = EdNode->MapMakerNode;
			if (MapMakerNode)
			{
				MapMakerNode->ParentNodes.Reset();
				MapMakerNode->ChildrenNodes.Reset();
				MapMakerNode->Edges.Reset();
			}
		}
	}
}

void UEdGraph_MapMaker::SortNodes(UMapMakerNode* RootNode)
{
	int Level = 0;
	TArray<UMapMakerNode*> CurrLevelNodes = { RootNode };
	TArray<UMapMakerNode*> NextLevelNodes;
	TSet<UMapMakerNode*> Visited;

	while (CurrLevelNodes.Num() != 0)
	{
		int32 LevelWidth = 0;
		for (int i = 0; i < CurrLevelNodes.Num(); ++i)
		{
			UMapMakerNode* Node = CurrLevelNodes[i];
			Visited.Add(Node);

			auto Comp = [&](const UMapMakerNode& L, const UMapMakerNode& R)
			{
				UEdNode_MapMakerNode* EdNode_LNode = NodeMap[&L];
				UEdNode_MapMakerNode* EdNode_RNode = NodeMap[&R];
				return EdNode_LNode->NodePosX < EdNode_RNode->NodePosX;
			};

			Node->ChildrenNodes.Sort(Comp);
			Node->ParentNodes.Sort(Comp);

			for (int j = 0; j < Node->ChildrenNodes.Num(); ++j)
			{
				UMapMakerNode* ChildNode = Node->ChildrenNodes[j];
				if (!Visited.Contains(ChildNode))
					NextLevelNodes.Add(Node->ChildrenNodes[j]);
			}
		}

		CurrLevelNodes = NextLevelNodes;
		NextLevelNodes.Reset();
		++Level;
	}
}

void UEdGraph_MapMaker::PostEditUndo()
{
	Super::PostEditUndo();

	NotifyGraphChanged();
}