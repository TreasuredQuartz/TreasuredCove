// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GASystemComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API UGASystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FGameplayTag ClassTag;
};
