// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemData.generated.h"

class UItemAttributeData;

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew)
class TREASUREDCOVE_API UItemPopupData : public UObject
{
	GENERATED_BODY()

public:
	UItemPopupData();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FText CooldownOrCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FText WeaponType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FText AimType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FText Summary;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	TMap<FString, FText> ItemAttributeData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FText Description;

public:
	UFUNCTION(BlueprintCallable, Category="Item")
	void GetAttributeMap(TMap<FString, FText>& Attributes);

};
