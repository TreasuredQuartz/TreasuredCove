// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnAttributeModifiedEvent.generated.h"

struct FGameplayEffectSpec;

USTRUCT(BlueprintType)
struct FOnAttributeModifiedPayload
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Amount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float OldValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float NewValue;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* Victim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* Causer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* Instigator;
		const FGameplayEffectSpec* EffectSpec;

public:
	FOnAttributeModifiedPayload()
		: Amount(0.f)
		, OldValue(0.f)
		, NewValue(0.f)
		, Victim(nullptr)
		, Causer(nullptr)
		, Instigator(nullptr)
		, EffectSpec(nullptr)
	{}

	FOnAttributeModifiedPayload(float InOldValue, float InNewValue)
		: Amount(InNewValue - InOldValue)
		, OldValue(InOldValue)
		, NewValue(InNewValue)
		, Victim(nullptr)
		, Causer(nullptr)
		, Instigator(nullptr)
		, EffectSpec(nullptr)
	{}

	FOnAttributeModifiedPayload(float InOldValue, float InNewValue,
		AActor* InVictim, AActor* InCauser, AActor* InInstigator, const FGameplayEffectSpec* InEffectSpec)
		: Amount(InNewValue - InOldValue)
		, OldValue(InOldValue)
		, NewValue(InNewValue)
		, Victim(InVictim)
		, Causer(InCauser)
		, Instigator(InInstigator)
		, EffectSpec(InEffectSpec)
	{}
};

/**
 * Delegate used to broadcast attribute events, some of these parameters may be null on clients:
 * @param EffectInstigator	The original instigating actor for this event
 * @param EffectCauser		The physical actor that caused the change
 * @param EffectSpec		The full effect spec for this change
 * @param EffectMagnitude	The raw magnitude, this is before clamping
 * @param OldValue			The value of the attribute before it was changed
 * @param NewValue			The value after it was changed
*/
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAttributeModifiedEvent, const FOnAttributeModifiedPayload&);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeModifiedEvent_BP, const FOnAttributeModifiedPayload&, Payload);