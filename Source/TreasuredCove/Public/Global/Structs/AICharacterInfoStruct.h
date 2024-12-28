// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Global/Enumerations/Relationship.h"
#include "Global/Enumerations/AttributeType.h"
#include "AICharacterInfoStruct.generated.h"

UENUM(BlueprintType)
enum class EAIAwarenessState : uint8
{
	UnAware,
	Suspicious,
	Aware
};

UENUM(BlueprintType)
enum class EAITaskState : uint8
{
	Idle,
	NeedsResidency,
	IsFatigued,
	IsHungry,
	IsUnHappy,
	IsUnsatisfied
};

USTRUCT(BlueprintType)
struct FAICharacterAwareness
{
	GENERATED_BODY()

public:
	EAIAwarenessState CurrentAwarenessState;

	void UpdateState()
	{
		switch (CurrentAwarenessState)
		{
		case EAIAwarenessState::UnAware:

			if (CurrentSuspicion > SuspicionThreshold)
			{
				CurrentAwarenessState = EAIAwarenessState::Suspicious;
			}

			break;
		case EAIAwarenessState::Suspicious:

			if (CurrentSuspicion > AwarenessThreshold)
			{
				CurrentAwarenessState = EAIAwarenessState::Aware;
			}
			else if (CurrentSuspicion < SuspicionThreshold)
			{
				CurrentAwarenessState = EAIAwarenessState::UnAware;
			}

			break;
		case EAIAwarenessState::Aware:

			if (CurrentSuspicion < AwarenessThreshold)
			{
				CurrentAwarenessState = EAIAwarenessState::Suspicious;
			}

			break;
		}
	}
	void SetCurrentSuspicion(float Value)
	{
		CurrentSuspicion = Value;
		UpdateState();
	}
	float GetCurrentSuspicion()
	{
		return CurrentSuspicion;
	}

private:
	float CurrentSuspicion;

	float SuspicionThreshold;
	float AwarenessThreshold;

public:
	FAICharacterAwareness()
		: CurrentAwarenessState(EAIAwarenessState::UnAware)
		, CurrentSuspicion(0)
		, SuspicionThreshold(5)
		, AwarenessThreshold(10)
	{
	}

	FAICharacterAwareness(float SuspicionLevel)
		: CurrentAwarenessState(EAIAwarenessState::UnAware)
		, CurrentSuspicion(0)
		, SuspicionThreshold(5)
		, AwarenessThreshold(10)
	{
		SetCurrentSuspicion(SuspicionLevel);
	}
};

USTRUCT(BlueprintType)
struct FAICharacterRelationship
{
	GENERATED_BODY()

public:
	ERelationship RelationshipStatus;

	float CurrentRelationshipState;

	float LastRelationshipStateThreshold;
	float NextRelationshipStateThreshold;

	float HealthDamaged;
	float HealthHealed;
	float StaminaDamaged;
	float StaminaHealed;
	float ManaDamaged;
	float ManaHealed;

public:
	FAICharacterRelationship()
		: RelationshipStatus(ERelationship::Stranger)
		, CurrentRelationshipState(0)
		, LastRelationshipStateThreshold(-10)
		, NextRelationshipStateThreshold(10)
		, HealthDamaged(0)
		, HealthHealed(0)
		, StaminaDamaged(0)
		, StaminaHealed(0)
		, ManaDamaged(0)
		, ManaHealed(0)
	{
	}

	FAICharacterRelationship(ERelationship Relation)
		: RelationshipStatus(Relation)
		, CurrentRelationshipState(0)
		, LastRelationshipStateThreshold(-10)
		, NextRelationshipStateThreshold(10)
		, HealthDamaged(0)
		, HealthHealed(0)
		, StaminaDamaged(0)
		, StaminaHealed(0)
		, ManaDamaged(0)
		, ManaHealed(0)
	{
	}
};

USTRUCT(BlueprintType)
struct FAICharacterStats
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stats")
	float CurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stats")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stats")
	float Fatigue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stats")
	float Hunger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stats")
	float Fear;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stats")
	float ProfessionSatisfaction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stats")
	float Happiness;

public:
	/*
	float GetCurrentHealth()
	{
		return CurrentHealth;
	}
	float GetFatigue()
	{
		return Fatigue;
	}
	float GetHunger()
	{
		return Hunger;
	}
	float GetFear()
	{
		return Fear;
	}
	float GetProfessionSatisfaction()
	{
		return ProfessionSatisfaction;
	}
	float GetHappiness()
	{
		return Happiness;
	}
	*/
public:
	FAICharacterStats()
		: CurrentHealth(10)
		, MaxHealth(10)
		, Fatigue(0)
		, Hunger(0)
		, Fear(0)
		, ProfessionSatisfaction(0)
		, Happiness(1)
	{
	}

	FAICharacterStats(float InHealth, float InMaxHealth, float InFatigue, float InHunger, float InFear, float InSatisfaction, float InHappiness)
		: CurrentHealth(InHealth)
		, MaxHealth(10)
		, Fatigue(InFatigue)
		, Hunger(InHunger)
		, Fear(InFear)
		, ProfessionSatisfaction(InSatisfaction)
		, Happiness(InHappiness)
	{
	}
};

USTRUCT(BlueprintType)
struct FAICharacterInfo
{
	GENERATED_BODY()

public:
	FAICharacterInfo()
		: Target(nullptr)
		, CurrentAIAwarenessTowardTarget(0)
		, CurrentAIRelationshipTowardTarget()
		, ForgetIncrements(0)
		, LastSeenLocation(FVector())
	{
	}

	FAICharacterInfo(class AGACharacter* InTarget, FVector InLocation, float SuspicionLevel)
		: Target(InTarget)
		, CurrentAIAwarenessTowardTarget(SuspicionLevel)
		, CurrentAIRelationshipTowardTarget()
		, ForgetIncrements(0)
		, LastSeenLocation(InLocation)
	{
	}
public:
	AGACharacter* Target;

	FTimerHandle AwarenessTimerHandle;

	FAICharacterAwareness CurrentAIAwarenessTowardTarget;
	FAICharacterRelationship CurrentAIRelationshipTowardTarget;
	FAICharacterStats CurrentAITargetStats;

	TArray<EAttributeType> ReactiveAttributeTypes;

	uint8 ForgetIncrements;
	FVector LastSeenLocation;

	FORCEINLINE bool operator == (const FAICharacterInfo& OtherInfo) const
	{
		return OtherInfo.Target == Target;
	}
};
