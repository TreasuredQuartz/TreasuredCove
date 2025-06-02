// Includes

#include "GenericGraphNode.h"
#include "GenericGraph.h"
#include "GenericGraphNodeComponent.h"


#define LOCTEXT_NAMESPACE "GenericGraphNode"

UGenericGraphNode::UGenericGraphNode()
{
#if WITH_EDITORONLY_DATA
	CompatibleGraphType = UGenericGraph::StaticClass();

	BackgroundColor = FLinearColor::Black;
#endif
}

UGenericGraphNode::~UGenericGraphNode()
{
}

UGenericGraphEdge* UGenericGraphNode::GetEdge(UGenericGraphNode* ChildNode)
{
	return Edges.Contains(ChildNode) ? Edges.FindChecked(ChildNode) : nullptr;
}

FText UGenericGraphNode::GetDescription_Implementation() const
{
	return LOCTEXT("NodeDesc", "Generic Graph Node");
}

UGenericGraphNodeComponent* UGenericGraphNode::GetComponentByClass(TSubclassOf<UGenericGraphNodeComponent> ComponentClass) const
{
	return FindComponentByClass(ComponentClass);
}

UGenericGraphNodeComponent* UGenericGraphNode::FindComponentByClass(const TSubclassOf<UGenericGraphNodeComponent> ComponentClass) const
{
	UGenericGraphNodeComponent* FoundComponent = nullptr;

	if (UClass* TargetClass = ComponentClass.Get())
	{
		for (UGenericGraphNodeComponent* Component : OwnedComponents)
		{
			if (Component && Component->IsA(TargetClass))
			{
				FoundComponent = Component;
				break;
			}
		}
	}

	return FoundComponent;
}

#if WITH_EDITOR
bool UGenericGraphNode::IsNameEditable() const
{
	return true;
}

FLinearColor UGenericGraphNode::GetBackgroundColor() const
{
	return BackgroundColor;
}

FText UGenericGraphNode::GetNodeTitle() const
{
	return NodeTitle.IsEmpty() ? GetDescription() : NodeTitle;
}

void UGenericGraphNode::SetNodeTitle(const FText& NewTitle)
{
	NodeTitle = NewTitle;
}

bool UGenericGraphNode::CanCreateConnection(UGenericGraphNode* Other, FText& ErrorMessage)
{	
	return true;
}

bool UGenericGraphNode::CanCreateConnectionTo(UGenericGraphNode* Other, int32 NumberOfChildrenNodes, FText& ErrorMessage)
{
	if (ChildrenLimitType == ENodeLimit::Limited && NumberOfChildrenNodes >= ChildrenLimit)
	{
		ErrorMessage = FText::FromString("Children limit exceeded");
		return false;
	}

	return CanCreateConnection(Other, ErrorMessage);
}

bool UGenericGraphNode::CanCreateConnectionFrom(UGenericGraphNode* Other, int32 NumberOfParentNodes, FText& ErrorMessage)
{
	if (ParentLimitType == ENodeLimit::Limited && NumberOfParentNodes >= ParentLimit)
	{
		ErrorMessage = FText::FromString("Parent limit exceeded");
		return false;
	}

	return true;
}


#endif

bool UGenericGraphNode::IsLeafNode() const
{
	return ChildrenNodes.Num() == 0;
}

UGenericGraph* UGenericGraphNode::GetGraph() const
{
	return Graph;
}

#undef LOCTEXT_NAMESPACE
