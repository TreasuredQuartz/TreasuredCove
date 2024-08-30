// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph_MapMaker.generated.h"

class UMapMaker;
class UMapMakerNode;
class UMapMakerEdge;
class UEdNode_MapMakerNode;
class UEdNode_MapMakerEdge;

/**
 * 
 */
UCLASS()
class MAPMAKEREDITOR_API UEdGraph_MapMaker : public UEdGraph
{
	GENERATED_BODY()
	
public:
	UEdGraph_MapMaker();
	virtual ~UEdGraph_MapMaker();

	virtual void RebuildMapMaker();

	UMapMaker* GetMapMaker() const;

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	virtual void PostEditUndo() override;

	UPROPERTY(Transient)
	TMap<UMapMakerNode*, UEdNode_MapMakerNode*> NodeMap;

	UPROPERTY(Transient)
	TMap<UMapMakerEdge*, UEdNode_MapMakerEdge*> EdgeMap;

protected:
	void Clear();

	void SortNodes(UMapMakerNode* RootNode);
};
