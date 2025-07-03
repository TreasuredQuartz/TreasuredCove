#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "GenericGraphAssetEditor/SGenericComponentList.h"
#include "EdGraph_GenericGraph.generated.h"

class UGenericGraph;
class UGenericGraphNode;
class UGenericGraphEdge;
class UEdNode_GenericGraphNode;
class UEdNode_GenericGraphEdge;
class FGenericComponentListNode;

UCLASS()
class GENERICGRAPHEDITOR_API UEdGraph_GenericGraph : public UEdGraph
{
	GENERATED_BODY()

public:

	DECLARE_DELEGATE_OneParam(FOnSelectionUpdated, const TArray<FGenericComponentListNodePtrType>& SelectedNodes)

public:
	UEdGraph_GenericGraph();
	virtual ~UEdGraph_GenericGraph();

	virtual void RebuildGenericGraph();

	UGenericGraph* GetGenericGraph() const;

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	virtual void PostEditUndo() override;

	UPROPERTY(Transient)
	TMap<UGenericGraphNode*, UEdNode_GenericGraphNode*> NodeMap;

	UPROPERTY(Transient)
	TMap<UGenericGraphEdge*, UEdNode_GenericGraphEdge*> EdgeMap;

protected:
	void Clear();

	void SortNodes(UGenericGraphNode* RootNode);
};
