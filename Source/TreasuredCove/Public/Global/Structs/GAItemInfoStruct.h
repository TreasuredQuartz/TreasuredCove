// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemUIInfoStruct.h"
#include "AbilityInfoStruct.h"
#include "GAItemInfoStruct.generated.h"

class UItemEditor;
class UCommandMenu;
class AActor;

USTRUCT(BlueprintType)
struct FGAItemInfo
{
	GENERATED_BODY()

	FGAItemInfo();

	FGAItemInfo(FName InName, UTexture2D* inTexture, 
		TSubclassOf<UCommandMenu> InItemWidgetActiveMenuClass, TSubclassOf<UItemEditor> InItemWidgetEditMenuClss);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* Texture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<UCommandMenu> ActiveMenuClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<UItemEditor> EditMenuClass;
};