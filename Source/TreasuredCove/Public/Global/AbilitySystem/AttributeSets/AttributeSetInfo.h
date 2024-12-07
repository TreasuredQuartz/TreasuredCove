// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AttributeSet.h"
#include "AttributeSetInfo.generated.h"

USTRUCT()
struct TREASUREDCOVE_API FUIInfoRow
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FGameplayAttribute Attribute;
	UPROPERTY(EditAnywhere)
	FText DisplayName;
	UPROPERTY(EditAnywhere)
	FText Description;
};

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UAttributeSetInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TArray<FUIInfoRow> AttributeSetInfo;
};
