// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemAttributeData.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UItemAttributeData : public UObject
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item|Attributes", meta = (AllowPrivateAccess = "true"))
	TMap<FString, FText> AttributeMap;

public:
	void GetAttributeMap(TMap<FString, FText>& OutAttributeMap);
};
