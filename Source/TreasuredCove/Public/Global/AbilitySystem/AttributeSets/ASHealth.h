// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeType.h"
#include "AttributeMacros.h"
#include "OnAttributeModifiedEvent.h"
#include "ASHealth.generated.h"

struct FGameplayEffectModCallbackData;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UASHealth : 
	public UAttributeSet
{
	GENERATED_BODY()

private:
	bool bHealthZeroed;
	float HealthBeforeAttributeChange;

private:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

public:
	// Constructor
	UASHealth();

	ATTRIBUTE_ACCESSORS(UASHealth, Health);
	ATTRIBUTE_ACCESSORS(UASHealth, MaxHealth);

	// All health properties
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes|Health")
	FGameplayAttributeData MaxHealth;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes|Health")
	FGameplayAttributeData Health;

	// All stamina properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Body")
	FGameplayAttributeData MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Body")
	FGameplayAttributeData Stamina;

	// All mana properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Mind")
	FGameplayAttributeData MaxMana;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Mind")
	FGameplayAttributeData Mana;

	// Defense
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Health")
	FGameplayAttributeData Defense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Health")
	FGameplayAttributeData DefenseMultiplier;

public:

	//
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	//
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnHealthModified;
	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnMaxHealthModified;
	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnHealthZeroed;
};
