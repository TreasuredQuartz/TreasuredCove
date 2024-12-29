// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/ExecutionCalculations/SimpleDEC.h"
#include "Global/AbilitySystem/AttributeSets/ASHealth.h"
#include "Global/AbilitySystem/AttributeSets/ASCombat.h"

struct FSimpleDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefenseMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Attack);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackMultiplier);
	FSimpleDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UASHealth, Health, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UASHealth, Defense, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UASHealth, DefenseMultiplier, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UASCombat, Attack, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UASCombat, AttackMultiplier, Source, true);
	}
};

static FSimpleDamageStatics& GetSimpleDamageStatics()
{
	static FSimpleDamageStatics DamageStatic;
	return DamageStatic;
}

USimpleDEC::USimpleDEC()
{
	RelevantAttributesToCapture.Add(GetSimpleDamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(GetSimpleDamageStatics().DefenseDef);
	RelevantAttributesToCapture.Add(GetSimpleDamageStatics().DefenseMultiplierDef);
	RelevantAttributesToCapture.Add(GetSimpleDamageStatics().AttackDef);
	RelevantAttributesToCapture.Add(GetSimpleDamageStatics().AttackMultiplierDef);
}

void USimpleDEC::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	float AttackMagnitude = 0.f;
	float AttackMultiplierMagnitude = 0.f;
	float DefenseMagnitude = 0.f;
	float DefenseMultiplierMagnitude = 0.f;

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetSimpleDamageStatics().AttackDef, FAggregatorEvaluateParameters(), AttackMagnitude);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetSimpleDamageStatics().AttackMultiplierDef, FAggregatorEvaluateParameters(), AttackMultiplierMagnitude);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetSimpleDamageStatics().DefenseDef, FAggregatorEvaluateParameters(), DefenseMagnitude);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetSimpleDamageStatics().DefenseMultiplierDef, FAggregatorEvaluateParameters(), DefenseMultiplierMagnitude);

	// The actual calculation
	float FinalDamage = AttackMagnitude * AttackMultiplierMagnitude - DefenseMagnitude * DefenseMultiplierMagnitude;

	// Damage Can never be negative
	FinalDamage = FMath::Clamp(FinalDamage, 0.f, FinalDamage);

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(HealthProperty, EGameplayModOp::Additive, -FinalDamage));
}