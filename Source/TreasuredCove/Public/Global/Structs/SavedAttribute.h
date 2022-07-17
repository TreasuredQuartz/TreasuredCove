// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "SavedAttribute.generated.h"

USTRUCT(BlueprintType)
struct TREASUREDCOVE_API FSavedAttribute
{
	GENERATED_BODY()

	FSavedAttribute()
		: Attribute(nullptr)
		, SavedValue(0.f)
	{
	}

	FSavedAttribute(FGameplayAttribute InAttribute, float InValue)
		: Attribute(InAttribute)
		, SavedValue(InValue)
	{
	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute Attribute;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SavedValue;
};