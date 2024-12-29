// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/UserInterface/Widgets/PauseMenu.h"
#include "Global/Framework/GAHUD.h"

void UPauseMenu::NativeConstruct()
{
	Super::NativeConstruct();

	GetOwningPlayer();
	
}

void UPauseMenu::NativeOnMoveItem(uint8 FromSlot, uint8 ToSlot)
{
	BP_OnMoveItem(FromSlot, ToSlot);
}
