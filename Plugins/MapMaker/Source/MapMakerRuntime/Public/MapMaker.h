// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MapMakerAsset.h"
#include "GameplayTagContainer.h"
#include "MapMaker.generated.h"

class UMapMakerNode;
class UMapMakerEdge;
class UMapMakerAssetDatabase;

UCLASS()
class MAPMAKERRUNTIME_API UMapMaker : public UObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UMapMaker();

	UPROPERTY(BlueprintReadOnly, Category = "MapMaker")
	FString MapName;

	UPROPERTY(EditDefaultsOnly, Category = "MapMaker")
	TSubclassOf<UMapMakerNode> NodeType;

	UPROPERTY(EditDefaultsOnly, Category = "MapMaker")
	TSubclassOf<UMapMakerEdge> EdgeType;

	UPROPERTY(EditDefaultsOnly, Category = "MapMaker")
	TMap<FGameplayTag, FSlateBrush> TagIcons;

	UPROPERTY(EditDefaultsOnly, Category = "MapMaker")
	UMapMakerAssetDatabase* AssetDatabase;

	UPROPERTY(EditDefaultsOnly, Category = "MapMaker")
	UMapMakerAsset* CurrentAsset;

	UPROPERTY(BlueprintReadOnly, Category = "MapMaker")
	TArray<UMapMakerNode*> RootNodes;

	UPROPERTY(BlueprintReadOnly, Category = "MapMaker")
	TArray<UMapMakerNode*> AllNodes;

	UPROPERTY(BlueprintReadOnly, Category = "MapMaker")
	TArray<FMatrix> MapDimensions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MapMaker")
	bool bEdgeEnabled;

	void ClearGraph();

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<class UEdGraph> EdGraph;

	UPROPERTY(EditDefaultsOnly, Category = "MapMaker_Editor")
	bool bCanRenameNode;

	UPROPERTY(EditDefaultsOnly, Category = "MapMaker_Editor")
	bool bCanBeCyclical;
#endif
};
