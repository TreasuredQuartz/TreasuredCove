// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeType.h"
#include "ASHealth.generated.h"

struct FGameplayEffectModCallbackData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnDamagedDelegate, AActor*, Instigator, EAttributeType, AttributeType, float, Amount, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealedDelegate, AActor*, Instigator, EAttributeType, AttributeType, float, Amount, float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthModifiedDelegate, float, Health, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaModifiedDelegate, float, Stamina, float, MaxStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnManaModifiedDelegate, float, Mana, float, MaxMana);


/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UASHealth : 
	public UAttributeSet
{
	GENERATED_BODY()

private:
	bool bDamaged;
	bool bHealed;
	float DeltaAmount;
	EAttributeType AttributeType;

public:
	// Constructor
	UASHealth();

	// All health properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Health")
	FGameplayAttributeData MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Health")
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

	//
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data) override;
	//
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data) override;

	UPROPERTY(BlueprintAssignable)
	FOnDamagedDelegate OnDamaged;
	UPROPERTY(BlueprintAssignable)
	FOnHealedDelegate OnHealed;

	UPROPERTY(BlueprintAssignable)
	FOnHealthModifiedDelegate OnHealthModified;
	UPROPERTY(BlueprintAssignable)
	FOnStaminaModifiedDelegate OnStaminaModified;
	UPROPERTY(BlueprintAssignable)
	FOnManaModifiedDelegate OnManaModified;
};
