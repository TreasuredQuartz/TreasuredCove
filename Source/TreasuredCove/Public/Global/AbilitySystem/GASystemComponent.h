// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
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
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

public:
	void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	UFUNCTION()
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
};
