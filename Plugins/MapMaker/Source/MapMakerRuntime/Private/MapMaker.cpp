// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMaker.h"
#include "MapMakerNode.h"
#include "MapMakerEdge.h"
#include "MapMakerAssetDatabase.h"

// Sets default values
UMapMaker::UMapMaker()
{
	NodeType = UMapMakerNode::StaticClass();
	EdgeType = UMapMakerEdge::StaticClass();
	AssetDatabase = NewObject<UMapMakerAssetDatabase>();

	bEdgeEnabled = true;

#if WITH_EDITORONLY_DATA
	EdGraph = nullptr;

	bCanRenameNode = true;
#endif
}

void UMapMaker::ClearGraph()
{
	for (int i = 0; i < AllNodes.Num(); ++i)
	{
		UMapMakerNode* Node = AllNodes[i];
		if (Node)
		{
			Node->ParentNodes.Empty();
			Node->ChildrenNodes.Empty();
			Node->Edges.Empty();
		}
	}

	AllNodes.Empty();
	RootNodes.Empty();
}


