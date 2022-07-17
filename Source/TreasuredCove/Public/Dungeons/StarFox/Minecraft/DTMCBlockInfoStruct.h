// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Datatable.h"
#include "DTMCBlockInfoStruct.generated.h"

USTRUCT(BlueprintType)
struct TREASUREDCOVE_API FDTMCBlockInfo :
	public FTableRowBase
{
	GENERATED_BODY()

public:
	FDTMCBlockInfo()
	{
		bIsTransparent = true;
		BlockType = 0;
		Material = nullptr;
		UITexture = nullptr;
	};

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTransparent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BlockType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Material;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* UITexture;
};
