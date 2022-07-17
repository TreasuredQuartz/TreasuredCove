// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ASCombat.generated.h"



/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UASCombat : 
	public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Health")
	FGameplayAttributeData Attack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Health")
	FGameplayAttributeData AttackMultiplier;
};
