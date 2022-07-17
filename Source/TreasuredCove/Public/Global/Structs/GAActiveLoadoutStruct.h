// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GAActiveLoadoutStruct.generated.h"

class UGameplayAbilityBase;

USTRUCT(BlueprintType)
struct FGAActivatedInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbilityBase> Ability;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Sound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage;
};

USTRUCT(BlueprintType)
struct FGAActiveLoadout
{
	GENERATED_BODY()

public:
	FGAActiveLoadout()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
		FGAActivatedInfo PrimaryAbility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
		FGAActivatedInfo SecondaryAbility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
		FGAActivatedInfo ThrowAbility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
		FGAActivatedInfo MeleeAbility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
		FGAActivatedInfo SlideAbility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
		FGAActivatedInfo InteractAbility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
		FGAActivatedInfo UltimateAbility;
};