// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slate.h"
#include "Widgets/SCompoundWidget.h"

/**
*
*/
class TREASUREDCOVE_API SBaseWidget :
	public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBaseWidget)
	{}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments &InArgs);
public:
	uint8 NavIndex;
};

