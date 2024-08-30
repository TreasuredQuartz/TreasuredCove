// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MapMakerNode.generated.h"

class UMapMaker;
class UMapMakerEdge;

/**
 * 
 */
UCLASS()
class MAPMAKERRUNTIME_API UMapMakerNode : public UObject
{
	GENERATED_BODY()
	
public:
	UMapMakerNode();
	virtual ~UMapMakerNode();

	UPROPERTY(EditAnywhere, Category = "MapMakerNode")
	FVector2D Size;

	UPROPERTY(VisibleDefaultsOnly, Category = "MapMakerNode")
	UMapMaker* Graph;

	UPROPERTY(BlueprintReadOnly, Category = "MapMakerNode")
	TArray<UMapMakerNode*> ParentNodes;

	UPROPERTY(BlueprintReadOnly, Category = "MapMakerNode")
	TArray<UMapMakerNode*> ChildrenNodes;

	UPROPERTY(BlueprintReadOnly, Category = "MapMakerNode")
	TMap<UMapMakerNode*, UMapMakerEdge*> Edges;

	UFUNCTION(BlueprintCallable, Category = "MapMakerNode")
	FVector2D GetSize() const;

	UFUNCTION(BlueprintCallable, Category = "MapMakerNode")
	virtual UMapMakerEdge* GetEdge(UMapMakerNode* ChildNode);

	UFUNCTION(BlueprintCallable, Category = "MapMakerNode")
	bool IsLeafNode() const;

	UFUNCTION(BlueprintCallable, Category = "MapMakerNode")
	UMapMaker* GetGraph() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MapMakerNode")
	FText GetDescription() const;
	virtual FText GetDescription_Implementation() const;

	//////////////////////////////////////////////////////////////////////////
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "MapMakerNode_Editor")
	FText NodeTitle;

	UPROPERTY(VisibleDefaultsOnly, Category = "MapMakerNode_Editor")
	TSubclassOf<UMapMaker> CompatibleGraphType;

	UPROPERTY(EditDefaultsOnly, Category = "MapMakerNode_Editor")
	FLinearColor BackgroundColor;

	UPROPERTY(EditDefaultsOnly, Category = "MapMakerNode_Editor")
	FText ContextMenuName;
#endif

#if WITH_EDITOR
	virtual bool IsNameEditable() const;

	virtual FLinearColor GetBackgroundColor() const;

	virtual FText GetNodeTitle() const;

	virtual void SetNodeTitle(const FText& NewTitle);

	virtual void SetNodeSize(const FVector2D& Delta);

	virtual bool CanCreateConnection(UMapMakerNode* Other, FText& ErrorMessage);

	virtual bool CanCreateConnectionTo(UMapMakerNode* Other, int32 NumberOfChildrenNodes, FText& ErrorMessage);
	virtual bool CanCreateConnectionFrom(UMapMakerNode* Other, int32 NumberOfParentNodes, FText& ErrorMessage);
#endif


};
