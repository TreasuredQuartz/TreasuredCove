// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SavedAttribute.h"
#include "GASaveGame.generated.h"

class AGAActor;
class UGameplayAbilityBase;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UGASaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving")
	TArray<FSavedAttribute> SavedAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving")
	TArray<UGameplayAbilityBase*> PlayerAquiredAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving")
	TArray<AGAActor*> SavedInventory;
};
