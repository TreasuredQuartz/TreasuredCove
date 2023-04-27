// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDatabase.generated.h"

class UItemPopupData;

USTRUCT(BlueprintType)
struct FRow
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FItemDatabaseRow
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UDataTable* SpawnTable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UDataTable* InventoryUITable;
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "UI")
	UItemPopupData* PopupData;
};

/**
 * 
 */
UCLASS(BlueprintType)
class TREASUREDCOVE_API UItemDatabase : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FName, FItemDatabaseRow> ItemDatabase;
};
