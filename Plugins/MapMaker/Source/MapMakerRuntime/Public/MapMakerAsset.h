// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MapMakerAsset.generated.h"

class AActor;

UCLASS(EditInlineNew)
class MAPMAKERRUNTIME_API UMapMakerAsset : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "MapMakerAsset")
	FTransform AssetTransform;
	UPROPERTY(EditDefaultsOnly, Category = "MapMakerAsset")
	TSubclassOf<AActor> ActorClass;
	UPROPERTY(EditDefaultsOnly, Category = "MapMakerAsset")
	AActor* SpawnedActor;
	UPROPERTY(EditDefaultsOnly, Category = "MapMakerAsset")
	FSlateBrush Brush;
};