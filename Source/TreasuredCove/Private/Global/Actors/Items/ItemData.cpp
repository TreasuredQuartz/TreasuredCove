// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Actors/Items/ItemData.h"
#include "Global/Actors/Items/ItemAttributeData.h"

UItemPopupData::UItemPopupData()
{

}

void UItemPopupData::GetAttributeMap(TMap<FString, FText>& OutAttributes)
{
	OutAttributes = ItemAttributeData;
}
