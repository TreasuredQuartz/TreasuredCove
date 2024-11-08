// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerNode.h"
#include "MapMaker.h"
#include "MapMakerAsset.h"

#define LOCTEXT_NAMESPACE "MapMakerNode"

UMapMakerNode::UMapMakerNode()
{
	Size = FVector2D(1000, 1000);

#if WITH_EDITORONLY_DATA
	CompatibleGraphType = UMapMaker::StaticClass();

	BackgroundColor = FLinearColor::Black;
#endif
}

UMapMakerNode::~UMapMakerNode()
{
}

FVector2D UMapMakerNode::GetSize() const
{
	return Size;
}

UMapMakerEdge* UMapMakerNode::GetEdge(UMapMakerNode* ChildNode)
{
	return Edges.Contains(ChildNode) ? Edges.FindChecked(ChildNode) : nullptr;
}

FText UMapMakerNode::GetDescription_Implementation() const
{
	return LOCTEXT("NodeDesc", "Generic Graph Node");
}

#if WITH_EDITOR
bool UMapMakerNode::IsNameEditable() const
{
	return true;
}

FLinearColor UMapMakerNode::GetBackgroundColor() const
{
	return BackgroundColor;
}

FText UMapMakerNode::GetNodeTitle() const
{
	return NodeTitle.IsEmpty() ? GetDescription() : NodeTitle;
}

void UMapMakerNode::SetNodeTitle(const FText& NewTitle)
{
	NodeTitle = NewTitle;
}

void UMapMakerNode::SetNodeSize(const FVector2D& NewSize)
{
	Size = NewSize;
	UE_LOG(LogTemp, Warning, TEXT("UMapMakerNode: Size Updated '%s'"), *Size.ToString());
}

bool UMapMakerNode::CanAddAsset(UMapMakerAsset* NewAsset) const
{
	if (!NewAsset) return false;
	const FVector Location = NewAsset->AssetTransform.GetLocation();

	for (UMapMakerAsset* Asset : Assets) {
		if (Asset && Asset->AssetTransform.GetLocation() == Location) {
			return false;
		}
	}

	return true;
}

TArray<UMapMakerAsset*> UMapMakerNode::GetAssets() const
{
	return Assets;
}

void UMapMakerNode::AddAsset(UMapMakerAsset* NewAsset)
{
	Assets.Add(NewAsset);
}

void UMapMakerNode::ClearAssets()
{
	Assets.Empty();
}

bool UMapMakerNode::CanCreateConnection(UMapMakerNode* Other, FText& ErrorMessage)
{
	return true;
}

bool UMapMakerNode::CanCreateConnectionTo(UMapMakerNode* Other, int32 NumberOfChildrenNodes, FText& ErrorMessage)
{
	return CanCreateConnection(Other, ErrorMessage);
}

bool UMapMakerNode::CanCreateConnectionFrom(UMapMakerNode* Other, int32 NumberOfParentNodes, FText& ErrorMessage)
{
	return true;
}


#endif

bool UMapMakerNode::IsLeafNode() const
{
	return ChildrenNodes.Num() == 0;
}

UMapMaker* UMapMakerNode::GetGraph() const
{
	return Graph;
}

#undef LOCTEXT_NAMESPACE
