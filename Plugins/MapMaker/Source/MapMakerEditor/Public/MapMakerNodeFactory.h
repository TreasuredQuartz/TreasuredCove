// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include <EdGraphUtilities.h>
#include <EdGraph/EdGraphNode.h>

/**
 * 
 */
class MAPMAKEREDITOR_API FGraphPanelNodeFactory_MapMaker : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override;
};
