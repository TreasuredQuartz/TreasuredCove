// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/ExecutionCalculations/AdvancedDEC.h"
#include "AbilitySystemComponent.h"
#include "Global/AbilitySystem/AttributeSets/ASHealth.h"
#include "Global/AbilitySystem/AttributeSets/ASWeaponStats.h"
#include "Global/Actors/Items/GAProjectile.h"
#include "Engine/Engine.h"

struct FAdvancedDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefenseMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Attack);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MinRange);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxRange);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MinDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxDamage);
	FAdvancedDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UASHealth, Health, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UASHealth, Defense, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UASHealth, DefenseMultiplier, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UASWeaponStats, Attack, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UASWeaponStats, MinRange, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UASWeaponStats, MaxRange, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UASWeaponStats, MinDamage, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UASWeaponStats, MaxDamage, Source, true);
	}
};

static FAdvancedDamageStatics& GetAdvancedDamageStatics()
{
	static FAdvancedDamageStatics DamageStatic;
	return DamageStatic;
}

UAdvancedDEC::UAdvancedDEC()
{
	RelevantAttributesToCapture.Add(GetAdvancedDamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(GetAdvancedDamageStatics().DefenseDef);
	RelevantAttributesToCapture.Add(GetAdvancedDamageStatics().DefenseMultiplierDef);
	RelevantAttributesToCapture.Add(GetAdvancedDamageStatics().AttackDef);
	RelevantAttributesToCapture.Add(GetAdvancedDamageStatics().MinRangeDef);
	RelevantAttributesToCapture.Add(GetAdvancedDamageStatics().MaxRangeDef);
	RelevantAttributesToCapture.Add(GetAdvancedDamageStatics().MinDamageDef);
	RelevantAttributesToCapture.Add(GetAdvancedDamageStatics().MaxDamageDef);
}

void UAdvancedDEC::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	float MinRangeMagnitude = 0.f;
	float MaxRangeMagnitude = 0.f;
	float AttackMagnitude = 0.f;
	float DefenseMagnitude = 0.f;
	float DefenseMultiplierMagnitude = 1.f;

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAdvancedDamageStatics().MinRangeDef, FAggregatorEvaluateParameters(), MinRangeMagnitude);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAdvancedDamageStatics().MaxRangeDef, FAggregatorEvaluateParameters(), MaxRangeMagnitude);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAdvancedDamageStatics().AttackDef, FAggregatorEvaluateParameters(), AttackMagnitude);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAdvancedDamageStatics().DefenseDef, FAggregatorEvaluateParameters(), DefenseMagnitude);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAdvancedDamageStatics().DefenseMultiplierDef, FAggregatorEvaluateParameters(), DefenseMultiplierMagnitude);

	AActor* SourceActor = ExecutionParams.GetSourceAbilitySystemComponent()->GetOwner();
	AActor* TargetActor = ExecutionParams.GetTargetAbilitySystemComponent()->GetOwner();

	ExecutionParams.GetOwningSpec().GetContext().GetHitResult();

	AGAProjectile* Proj = Cast<AGAProjectile>(ExecutionParams.GetOwningSpec().GetContext().GetEffectCauser());
	if (Proj) UE_LOG(LogTemp, Warning, TEXT("Effect Causer is projectile!"))

	FVector Distance = TargetActor->GetActorLocation() - SourceActor->GetActorLocation();
	float DistanceTraveled = Distance.Size();

	float Range = FMath::Clamp(DistanceTraveled, MinRangeMagnitude, MaxRangeMagnitude);
	float AttackMultiplier = Range / MaxRangeMagnitude;
	AttackMultiplier = abs(1 - AttackMultiplier);

	float TotalAttack = AttackMagnitude * AttackMultiplier;
	float TotalDefense = DefenseMagnitude * DefenseMultiplierMagnitude;
	float FinalDamage = TotalAttack - TotalDefense;

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetAdvancedDamageStatics().HealthProperty, EGameplayModOp::Additive, -FinalDamage));

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::SanitizeFloat(Distance.Size()));
	}
}