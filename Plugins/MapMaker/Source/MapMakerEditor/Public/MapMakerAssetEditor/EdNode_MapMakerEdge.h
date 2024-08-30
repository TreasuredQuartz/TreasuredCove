// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "EdNode_MapMakerEdge.generated.h"

class UMapMakerEdge;
class UEdNode_MapMakerNode;

/**
 * 
 */
UCLASS()
class MAPMAKEREDITOR_API UEdNode_MapMakerEdge : public UEdGraphNode
{
	GENERATED_BODY()
	
public:
	UEdNode_MapMakerEdge();

	UPROPERTY()
	class UEdGraph* Graph;

	UPROPERTY(VisibleAnywhere, Instanced, Category = "MapMaker")
	UMapMakerEdge* MapMakerEdge;

	void SetEdge(UMapMakerEdge* Edge);

	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

	virtual void PrepareForCopying() override;

	virtual UEdGraphPin* GetInputPin() const { return Pins[0]; }
	virtual UEdGraphPin* GetOutputPin() const { return Pins[1]; }

	void CreateConnections(UEdNode_MapMakerNode* Start, UEdNode_MapMakerNode* End);

	UEdNode_MapMakerNode* GetStartNode();
	UEdNode_MapMakerNode* GetEndNode();
};
