// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ASCharacterStats.generated.h"


/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UASCharacterStats : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Health")
	FGameplayAttributeData MaxGroundMovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Health")
	FGameplayAttributeData MaxJumpHeight;

	//
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
