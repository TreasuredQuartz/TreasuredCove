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
	UDataTable* LootTable;
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "UI")
	UItemPopupData* PopupData;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FString Category;
};

/**
 * 
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API UItemDatabase : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Database")
	TMap<FName, FItemDatabaseRow> ItemDatabase;
	TArray<FString> Categories;

public:
	int32 GetTotalItemCount() const { return ItemDatabase.Num(); };


};
