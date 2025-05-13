// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Components/Items/CommandMenuComponent.h"

// Sets default values for this component's properties
UCommandMenuComponent::UCommandMenuComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// */
//////////////////////////////////////////////////////
/////			CHARACTER COMBAT MENU			//////
//////////////////////////////////////////////////////

void UCommandMenuComponent::MenuUp()
{
	if (CurrentSubMenuSlot == 0)
		DetermineActiveMenuSelection(0, 1);
	else if (MaxSubMenuSlots.Num() > CurrentSubMenuSlot)
		DetermineActiveMenuSelection(MaxActiveMenuSlots[CurrentSubMenuSlot], 1);

	OnCommandMenuChanged.Broadcast();
}

void UCommandMenuComponent::MenuDown()
{
	if (CurrentSubMenuSlot == 0)
		DetermineActiveMenuSelection(0, -1);
	else if (MaxSubMenuSlots.Num() > CurrentSubMenuSlot)
		DetermineActiveMenuSelection(MaxActiveMenuSlots[CurrentSubMenuSlot], -1);

	OnCommandMenuChanged.Broadcast();
}

void UCommandMenuComponent::MenuRight()
{
	if (CurrentActiveMenuSlot == 0)
		DetermineSubMenuSelection(0, 1);
	else if (MaxSubMenuSlots[CurrentActiveMenuSlot])
		DetermineSubMenuSelection(MaxSubMenuSlots[CurrentActiveMenuSlot], 1);

	OnCommandMenuChanged.Broadcast();
}

void UCommandMenuComponent::MenuLeft()
{
	if (CurrentActiveMenuSlot == 0)
		DetermineSubMenuSelection(0, -1);
	else if (MaxSubMenuSlots[CurrentActiveMenuSlot])
		DetermineSubMenuSelection(MaxSubMenuSlots[CurrentActiveMenuSlot], -1);

	OnCommandMenuChanged.Broadcast();
}

uint8 UCommandMenuComponent::GetCurrentActiveMenuSlot() const
{
	return CurrentActiveMenuSlot;
}

uint8 UCommandMenuComponent::GetCurrentSubMenuSlot() const
{
	return CurrentSubMenuSlot;
}

void UCommandMenuComponent::ResetActiveMenuSelection()
{
	CurrentActiveMenuSlot = 1;
}

void UCommandMenuComponent::ResetSubMenuSelection()
{
	CurrentSubMenuSlot = 1;
}

int UCommandMenuComponent::DetermineActiveMenuSelection(int MaxMenuCount, int Direction)
{
	int Value = -1;

	if (MaxMenuCount == 0)
	{
	}

	if (Direction > 0)
	{
		CurrentActiveMenuSlot += 1;

		if (CurrentActiveMenuSlot <= MaxMenuCount)
		{
			Value = CurrentActiveMenuSlot;
		}
		else if (CurrentActiveMenuSlot > MaxMenuCount)
		{
			Value = 1;
			CurrentActiveMenuSlot = 1;
		}
	}
	else if (Direction < 0)
	{
		CurrentActiveMenuSlot -= 1;

		if (CurrentActiveMenuSlot >= 1)
		{
			Value = CurrentActiveMenuSlot;
		}
		else if (CurrentActiveMenuSlot < 1)
		{
			Value = MaxMenuCount;
			CurrentActiveMenuSlot = MaxMenuCount;
		}
	}

	// ResetSubMenuSelection();

	return Value;
}

int UCommandMenuComponent::DetermineSubMenuSelection(int MaxMenuCount, int Direction)
{
	// PreviousActiveMenuIndex = CurrentActiveMenuSlot;
	int Value = -1;

	if (MaxMenuCount == 0)
	{
	}

	if (Direction > 0)
	{
		CurrentSubMenuSlot += 1;

		if (CurrentSubMenuSlot <= MaxMenuCount)
		{
			Value = CurrentSubMenuSlot;
		}
		else if (CurrentSubMenuSlot > MaxMenuCount)
		{
			Value = 1;
			CurrentSubMenuSlot = 1;
		}
	}
	else if (Direction < 0)
	{
		CurrentSubMenuSlot -= 1;

		if (CurrentSubMenuSlot >= 1)
		{
			Value = CurrentSubMenuSlot;
		}
		else if (CurrentSubMenuSlot < 1)
		{
			Value = MaxMenuCount;
			CurrentSubMenuSlot = MaxMenuCount;
		}
	}

	ResetActiveMenuSelection();

	return Value;
}
