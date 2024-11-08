// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "EdNode_MapMakerNode.generated.h"

class SCanvas;
class UMapMakerNode;
class UMapMakerAsset;
class UEdGraph_MapMaker;
class SEdNode_MapMakerNode;

/**
 * 
 */
UCLASS()
class MAPMAKEREDITOR_API UEdNode_MapMakerNode : public UEdGraphNode
{
	GENERATED_BODY()
	
public:
	UEdNode_MapMakerNode();
	virtual ~UEdNode_MapMakerNode();

	UPROPERTY(VisibleAnywhere, Instanced, Category = "MapMaker")
	UMapMakerNode* MapMakerNode;

	void SetMapMakerNode(UMapMakerNode* InNode);
	UEdGraph_MapMaker* GetMapMakerEdGraph();

	SEdNode_MapMakerNode* SEdNode;

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PrepareForCopying() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

	virtual FLinearColor GetBackgroundColor() const;
	virtual FVector2D GetSize() const;
	virtual UEdGraphPin* GetInputPin() const;
	virtual UEdGraphPin* GetOutputPin() const;

#if WITH_EDITOR
	virtual void PostEditUndo() override;
#endif
};
