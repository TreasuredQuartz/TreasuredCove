// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDatabase.generated.h"

class UItemData;
class AGAWeapon;

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
	FItemDatabaseRow();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FName ItemName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<AGAWeapon> Class;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TArray<UItemData*> ItemData;
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "UI")
	AGAWeapon* ItemInstance;

public:
	UItemData* FindItemDataByClass(TSubclassOf<UItemData> ItemDataClass) const;

	template <class T>
	T* GetItemDataByClass() const
	{
		return FindItemDataByClass<T>()
	};

	template<class T>
	T* FindItemDataByClass() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, const UItemData>::Value, "'T' template parameter to FindItemDataByClass must be derived from UActorComponent");

		return (T*)FindItemDataByClass(T::StaticClass());
	}
};

/**
 * 
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API UItemDatabase : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Database", meta = (TitleProperty="ItemName"))
	TArray<FItemDatabaseRow> ItemDatabase;
	UPROPERTY()
	TArray<FString> Categories;

public:
	int32 GetTotalItemCount() const { return ItemDatabase.Num(); };
};
