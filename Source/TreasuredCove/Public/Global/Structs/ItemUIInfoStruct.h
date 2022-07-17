// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Datatable.h"
#include "ItemUIInfoStruct.generated.h"

USTRUCT(BlueprintType)
struct TREASUREDCOVE_API FItemUIInfoRow :
	public FTableRowBase
{
	GENERATED_BODY()

	FItemUIInfoRow() :
		Name(FString("Potion")),
		Category(FString("Item")),
		Description(FString("Heals")),
		Max(1),
		Value(0),
		Texture(nullptr)
	{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString Category;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int Max;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int Value;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* Texture;
};
