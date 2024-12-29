// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/Interfaces/InteractionInterface.h"

void IInteractionInterface::InteractedWith_Implementation(AActor* OtherActor)
{

}

void IInteractionInterface::NotifyCanInteract_Implementation(FName InteractibleName, bool CanInteract)
{

}

void IInteractionInterface::AddItemToInventory_Implementation(AGAActor* Item)
{

}

void IInteractionInterface::AddItemToRepository_Implementation(const FString& Category, const FItemKey& Item)
{

}