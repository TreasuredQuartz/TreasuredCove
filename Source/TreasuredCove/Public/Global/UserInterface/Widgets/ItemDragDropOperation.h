// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "ItemDragDropOperation.generated.h"

class UItemSlot;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	bool IsFromInventory;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	uint8 Slot;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	UTexture2D* Image;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	UItemSlot* Source;
};
