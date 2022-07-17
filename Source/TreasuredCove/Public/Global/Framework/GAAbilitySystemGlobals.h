// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AttributeSet.h"
#include "GAAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UGAAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:
	virtual void AllocAttributeSetInitter() override;

private:
	
};
/*
struct TREASUREDCOVE_API FAttributeSetInitterCurveEval :
	public FAttributeSetInitter
{
	struct FAttributeDefaultCurveList
	{
		struct FPropertyCurvePair
		{
			FPropertyCurvePair(UProperty* InProperty, FRichCurve* InCurve)
				: Property(InProperty), Curve(InCurve)
			{
			}
			UProperty* Property;
			FRichCurve* Curve;
		};

		void AddPair(UProperty* InProperty, FRichCurve* InCurve)
		{
			List.Add(FPropertyCurvePair(InProperty, InCurve));
		}

		TArray<FPropertyCurvePair> List;
	};

	struct FAttributeSetDefaultsCurveCollection
	{
		TMap<TSubclassOf<UAttributeSet>, FAttributeDefaultCurveList> DataMap;
	};

	TMap<FName, FAttributeSetDefaultsCurveCollection> Defaults;

	bool IsSupportedProperty(UProperty* Property) const;
	void PreloadAttributeSetData(const TArray<UCurveTable*>& CurveData) override;
	void InitAttributeSetDefaults(UAbilitySystemComponent* AbilitySystemComponent, FName GroupName, int32 Level, bool bInitialInit) const override;
	void ApplyAttributeDefault(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute& InAttribute, FName GroupName, int32 Level) const override;
};
*/