// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Texture2D.h"
#include "DraggedItemSlot.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UDraggedItemSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (ExposeOnSpawn = "true"))
	UTexture2D* ItemImage;
};
