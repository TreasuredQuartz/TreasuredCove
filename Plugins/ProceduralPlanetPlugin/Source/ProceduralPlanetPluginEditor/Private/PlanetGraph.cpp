#include "PlanetGraph.h"

void UEdGraphSchema_Planet::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
}

void UEdGraphSchema_Planet::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
}

void UEdGraphSchema_Planet::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
}

const FPinConnectionResponse UEdGraphSchema_Planet::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	return FPinConnectionResponse();
}

const FPinConnectionResponse UEdGraphSchema_Planet::CanMergeNodes(const UEdGraphNode* A, const UEdGraphNode* B) const
{
	return FPinConnectionResponse();
}

FLinearColor UEdGraphSchema_Planet::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor();
}

int32 UEdGraphSchema_Planet::GetNodeSelectionCount(const UEdGraph* Graph) const
{
	return int32();
}

bool UEdGraphSchema_Planet::IsCacheVisualizationOutOfDate(int32 InVisualizationCacheID) const
{
	return false;
}

int32 UEdGraphSchema_Planet::GetCurrentVisualizationCacheID() const
{
	return int32();
}

void UEdGraphSchema_Planet::ForceVisualizationCacheClear() const
{
}

TSharedPtr<FEdGraphSchemaAction> UEdGraphSchema_Planet::GetCreateCommentAction() const
{
	return TSharedPtr<FEdGraphSchemaAction>();
}
