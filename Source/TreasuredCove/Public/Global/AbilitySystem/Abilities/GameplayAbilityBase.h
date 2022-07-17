// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilityInfoStruct.h"
#include "GameplayAbilityBase.generated.h"
\
class AGAActor;
class AGACharacter;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	UMaterialInterface* UIMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FName AbilityName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FText AbilityDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TArray<TSubclassOf<UGameplayAbilityBase>> RequiredAbilities;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TArray<FRequiredAttribute> RequiredAttributes;

	// Returns helpful struct containing ability info
	UFUNCTION(BlueprintCallable, Category = "GA_Base")
	FAbilityInfo GetAbilityInfo();

	UFUNCTION(BlueprintCallable, Category = "GA_Base")
	void UICooldown();

	// For specific usage on the owning actor (Casted to base character arctype)
	UFUNCTION(BlueprintCallable, Category = "GA_Base")
	AGACharacter* GetOwningCharacter();

	// For specific usage on the owning actor (Casted to base item arctype)
	UFUNCTION(BlueprintCallable, Category = "GA_Base")
	AGAActor* GetOwningItem();
};
