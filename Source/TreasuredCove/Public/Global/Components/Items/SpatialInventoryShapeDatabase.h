// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SpatialInventoryShapeDatabase.generated.h"

USTRUCT(BlueprintType)
struct TREASUREDCOVE_API FSpatialInventoryShape
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ShapeName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FIntPoint> Tiles;

};

/**
 * 
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API USpatialInventoryShapeDatabase : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (TitleProperty = "ShapeName"))
	TArray<FSpatialInventoryShape> Shapes;
};
