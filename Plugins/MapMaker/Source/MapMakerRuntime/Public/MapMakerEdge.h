// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "MapMakerEdge.generated.h"

class UMapMaker;
class UMapMakerNode;

/**
 * 
 */
UCLASS()
class MAPMAKERRUNTIME_API UMapMakerEdge : public UObject
{
	GENERATED_BODY()
	
public:
	UMapMakerEdge();
	virtual ~UMapMakerEdge();

	UPROPERTY(VisibleAnywhere, Category = "MapMakerNode")
	UMapMaker* Graph;

	UPROPERTY(BlueprintReadOnly, Category = "MapMakerEdge")
	UMapMakerNode* StartNode;

	UPROPERTY(BlueprintReadOnly, Category = "MapMakerEdge")
	UMapMakerNode* EndNode;

	UFUNCTION(BlueprintPure, Category = "MapMakerEdge")
	UMapMaker* GetGraph() const;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "MapMakerNode_Editor")
	bool bShouldDrawTitle = false;

	UPROPERTY(EditDefaultsOnly, Category = "MapMakerNode_Editor")
	FText NodeTitle;

	UPROPERTY(EditDefaultsOnly, Category = "MapMakerNode_Editor")
	FGameplayTagContainer EdgeTags;

	UPROPERTY(EditDefaultsOnly, Category = "MapMakerEdge")
	FLinearColor EdgeColour = FLinearColor(0.9f, 0.9f, 0.9f, 1.0f);
#endif

#if WITH_EDITOR
	virtual FText GetNodeTitle() const { return NodeTitle; }
	FLinearColor GetEdgeColour() const { return EdgeColour; }
	const FGameplayTagContainer& GetEdgeTags() const { return EdgeTags; }

	virtual void SetNodeTitle(const FText& NewTitle);
#endif
};
