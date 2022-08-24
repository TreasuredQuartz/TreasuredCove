// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MeleeCombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UMeleeCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TREASUREDCOVE_API IMeleeCombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// -- Intended for use with animation notfies for enabling hitboxes then diabling when called again -- //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Melee")
	void LeftArm_Notify();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Melee")
	void RightArm_Notify();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Melee")
	void LeftLeg_Notify();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat|Melee")
	void RightLeg_Notify();
};
