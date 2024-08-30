// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MAPMAKEREDITOR_API FEditorViewportClient_MapMaker : public FEditorViewportClient
{
public:
	FEditorViewportClient_MapMaker(FEditorModeTools* InModeTools, FPreviewScene* InPreviewScene)
		: FEditorViewportClient(InModeTools, InPreviewScene)
	{};

public:
	virtual void Draw(FViewport* InViewport, FCanvas* Canvas) override;
	virtual void Tick(float DeltaSeconds) override;

};
