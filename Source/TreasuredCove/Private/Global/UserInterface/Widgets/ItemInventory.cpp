// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemInventory.h"
#include "ItemSlot.h"
#include "PauseMenu.h"

//
void UItemInventory::NativeConstruct()
{
	Super::NativeConstruct();

	PauseMenu = Cast<UPauseMenu>(GetParent());
}

//
void UItemInventory::NativeOnMoveItem(uint8 FromSlot, uint8 ToSlot)
{
	PauseMenu->NativeOnMoveItem(FromSlot, ToSlot);
}

//
void UItemInventory::NativeOnAddItem()
{
	
}

//
void UItemInventory::NativeOnDropItem()
{

}

//
void UItemInventory::NativeOnEquipItem()
{

}
