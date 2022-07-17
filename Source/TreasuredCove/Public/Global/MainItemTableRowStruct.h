// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Datatable.h"
#include "MainItemTableRowStruct.generated.h"

USTRUCT(BlueprintType)
struct TREASUREDCOVE_API FMainItemTableRow : 
	public FTableRowBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values
	FMainItemTableRow() : ItemName(FName()), DataTable(nullptr) {};

	UPROPERTY()
	FName ItemName;
	UPROPERTY()
	UDataTable* DataTable;


};
