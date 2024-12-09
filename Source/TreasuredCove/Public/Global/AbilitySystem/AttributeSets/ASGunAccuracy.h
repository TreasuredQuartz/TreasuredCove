// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ASGunAccuracy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpreadAngleModified, float, SpreadAngle);

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UASGunAccuracy : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UASGunAccuracy();

	/* Accuracy Dependent Stats */
	void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

public:
	// The unwieldiness of the weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData SpreadAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData SpreadMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData SpreadMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData SpreadDecreaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData SpreadIncreaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData SpreadDeltaMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData Sway;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData HorizontalRecoil;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData VerticalRecoil;

public:
	FOnSpreadAngleModified OnSpreadAngleModified;
};
