// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UCharacterClass;

/**
 * 
 */
class CHARACTERCLASSMAKEREDITOR_API SCharacterClassEditor : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SCharacterClassEditor) {}

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, UCharacterClass* InRepresentingAsset);
	void RebuildEditor();
	
private:
	TObjectPtr<UCharacterClass> EditingClassAsset;
};
