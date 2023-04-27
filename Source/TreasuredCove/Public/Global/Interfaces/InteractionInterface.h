// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ItemKeyStruct.h"
#include "InteractionInterface.generated.h"

class AGAActor;

/**
 * 
 */
UINTERFACE(BlueprintType)
class TREASUREDCOVE_API UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class TREASUREDCOVE_API IInteractionInterface
{
	GENERATED_BODY()

public:
	//
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void InteractedWith(AActor* OtherActor);
	virtual void InteractedWith_Implementation(AActor* OtherActor);

	//
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void NotifyCanInteract(FName InteractibleName, bool CanInteract);
	virtual void NotifyCanInteract_Implementation(FName InteractibleName, bool CanPickup);

	//
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void AddItemToInventory(AGAActor* Item);
	virtual void AddItemToInventory_Implementation(AGAActor* Item);

	//
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void AddItemToRepository(const FString& Category, const FItemKey& Item);
	virtual void AddItemToRepository_Implementation(const FString& Category, const FItemKey& Item);
};
