// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MapMakerAssetDatabase.generated.h"

class UMapMakerAsset;

/**
 * 
 */
UCLASS()
class MAPMAKERRUNTIME_API UMapMakerAssetDatabase : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Instanced, EditDefaultsOnly, Category="MapMaker")
	TArray<UMapMakerAsset*> Assets;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditdefaultsOnly, Category="MapMakerEditor")
	FSlateBrush Asset2DRepresentation;
#endif
};
