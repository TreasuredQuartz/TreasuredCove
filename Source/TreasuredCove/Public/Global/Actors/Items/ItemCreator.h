// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemCreator.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UItemCreator : public UObject
{
	GENERATED_BODY()

public:
	ItemCreator();

public:
	TArray<UObject*> Parts;
};
