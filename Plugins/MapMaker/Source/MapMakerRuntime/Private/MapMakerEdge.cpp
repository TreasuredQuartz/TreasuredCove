// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerEdge.h"

UMapMakerEdge::UMapMakerEdge()
{
	
}

UMapMakerEdge::~UMapMakerEdge()
{

}

UMapMaker* UMapMakerEdge::GetGraph() const
{
	return Graph;
}

#if WITH_EDITOR
void UMapMakerEdge::SetNodeTitle(const FText& NewTitle)
{
	NodeTitle = NewTitle;
}
#endif // #if WITH_EDITOR