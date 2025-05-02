// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/HexGrid/HexGridEditor.h"
#include "Global/HexGrid/HexGridManager.h"
#include "Global/HexGrid/HexTileSelector.h"

/*************************************/ 
//                                   //
/*************************************/

UHexGridEditor::UHexGridEditor()
	: bIsEditing(false)
	, bInitialized(false)
	, bHasBegunPlay(false)
	, TileSelectorClass(UHexTileSelector::StaticClass())
	, HexManager(nullptr)
	, TileSelector(nullptr)
{
}

void UHexGridEditor::BeginPlay()
{
	bHasBegunPlay = true;
}

void UHexGridEditor::Initialize()
{
	bInitialized = true;
}

void UHexGridEditor::Tick(float DeltaTime)
{
	//
	if (!bHasBegunPlay && GetWorld() && GetWorld()->HasBegunPlay())
	{
		BeginPlay();
	}

	// Only tick while editing...
	if (!bIsEditing) return;

	if (TileSelector && TileSelector->GetClass() == TileSelectorClass)
	{
		TileSelector->TileSelectionCheck();
	}
	else if (TileSelectorClass)
	{
		TileSelector = NewObject<UHexTileSelector>(GetOuter(), TileSelectorClass);
	}
}

bool UHexGridEditor::IsTickable() const
{
	return true;
}

bool UHexGridEditor::IsTickableInEditor() const
{
	return false;
}

bool UHexGridEditor::IsTickableWhenPaused() const
{
	return false;
}

TStatId UHexGridEditor::GetStatId() const
{
	return TStatId();
}

void UHexGridEditor::StartEditing()
{
	bIsEditing = true;

	if (!bInitialized)
	{
		Initialize();
	}
}

void UHexGridEditor::StopEditing()
{
	bIsEditing = false;
}
