// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LoadoutItemInfo.generated.h"

USTRUCT(BlueprintType)
struct TREASUREDCOVE_API FLoadoutItemInfo
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer|Loadout")
	uint8 ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer|Loadout")
	uint8 LoadoutCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer|Loadout")
	uint8 DataTableSource;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer|Loadout")
	FName DataRowName;

public:	
	// Sets default values
	FLoadoutItemInfo() { InValidate(); };

public:
	// Returns if this valid
	bool IsValid() {
		return DataRowName.IsNone();
	}

	// Sets all values to a zero based value
	void InValidate() {
		ItemID = 0;
		LoadoutCost = 0;
		DataTableSource = 0;
		DataRowName = FName();
	}

public:
	bool operator ==(const FLoadoutItemInfo& Other) const
	{
		return this->ItemID == Other.ItemID &&
			this->DataTableSource == Other.DataTableSource &&
			this->DataRowName == Other.DataRowName;
	}

};
