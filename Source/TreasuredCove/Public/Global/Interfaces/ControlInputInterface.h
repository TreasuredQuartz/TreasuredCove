// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ControlInputInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UControlInputInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TREASUREDCOVE_API IControlInputInterface
{
	GENERATED_BODY()

public:
	//
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void UsePrimary();
	virtual void UsePrimary_Implementation() {};

	//
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void UseSecondary();
	virtual void UseSecondary_Implementation() {};

	//
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void UseThrow();
	virtual void UseThrow_Implementation() {};

	//
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void UseMelee();
	virtual void UseMelee_Implementation() {};

	//
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void UseUp(float Val);
	virtual void UseUp_Implementation(float Val) {};

	//
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void UseRight(float Val);
	virtual void UseRight_Implementation(float Val) {};
};
