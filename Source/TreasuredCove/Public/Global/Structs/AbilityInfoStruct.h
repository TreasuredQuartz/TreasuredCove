// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilityInfoStruct.generated.h"

class UMaterialInterface;
class UGameplayAbilityBase;

USTRUCT(BlueprintType)
struct FRequiredAttribute
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	FGameplayAttribute Attribute;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float AttributeAmount;
};

UENUM(BlueprintType)
enum class EAbilityCostType : uint8
{
	//-------------------
	// Core Set
	//-------------------

	Experience,
	Health,
	Mana,
	SkillPoints,
	Spirit,
	Stamina,
	StatPoints,

	//-------------------
	// Currency Set
	//-------------------

	Bronze,
	Silver,
	Gold,
	Platnum,

	//-------------------
	// Ammo Stock Set
	//-------------------

	// Ammo
	Ammo,

	// Handgun
	Point22LongRifle,
	Point22Magnum,
	Point25ACP,
	Point32ACP,
	Point380ACP,
	Point38Special,
	mm9NATO,
	Point357Magnum,
	Point45ACP,
	Point44Magnum,

	// Rifle
	Point223Remington,
	mm39x7Point62,
	Point30_30Winchester,
	Point308Winchester,
	R54x7Point62,
	Point30_06Springfield,

	// Vehicle
	Point50BMG,
	mm102x20,

	// Shotgun
	Guage12,
	Guage20,
	Guage28
};

USTRUCT(BlueprintType)
struct FAbilityInfo
{
	GENERATED_BODY()

	FAbilityInfo();
	FAbilityInfo(float InCoolDown, float InCost, EAbilityCostType InAbilityCostType, 
		UMaterialInterface* InUIMaterial, TSubclassOf<UGameplayAbilityBase> InAbilities, 
		TArray<TSubclassOf<UGameplayAbilityBase>> InRequiredAbilities,
		TArray<FRequiredAttribute> InRequiredAttributes, 
		FName InAbilityName, FText InAbilityDescription);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<UGameplayAbilityBase> Class;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	FString Category;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	UMaterialInterface* UIMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float Cooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	EAbilityCostType CostType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TArray<TSubclassOf<UGameplayAbilityBase>> RequiredAbilities;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TArray<FRequiredAttribute> RequiredAttributes;
};

