// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BlockMeshData.generated.h"

class UGameplayTileData;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UBlockMeshData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<UGameplayTileData*> BlockMeshData;
};
