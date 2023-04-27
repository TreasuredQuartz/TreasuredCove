// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Actors/Items/ItemAttributeData.h"

void UItemAttributeData::GetAttributeMap(TMap<FString, FText>& OutAttributeMap)
{
	OutAttributeMap = AttributeMap;
}
