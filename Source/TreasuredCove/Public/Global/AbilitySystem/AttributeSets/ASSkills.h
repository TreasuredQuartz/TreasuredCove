// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ASSkills.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UASSkills : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData Athletics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData Acrobatics;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData SleightOfHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData Stealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData Arcana;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData History;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData Investigation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData Nature;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData Religion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData AnimalHandling;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData Insight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData Medicine;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData Perception;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData Survival;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData Deception;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData Intimidation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData Performance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Skills")
	FGameplayAttributeData Persuasion;

};
