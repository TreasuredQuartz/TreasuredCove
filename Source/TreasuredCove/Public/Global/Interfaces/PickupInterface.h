// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemKeyStruct.h"
#include "InteractionInterface.h"
#include "PickupInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickupInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TREASUREDCOVE_API IPickupInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Items")
	bool OnCanBePickedUp(AActor* OtherActor);

	// C++ implementation checks private variable (bBeingPickedUp == false)
	// Then sets bBeingPickedUp to the return value of OnCanBePickedUp().
	// BP should override the OnCanBePickedUp() function instead.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Items")
	bool CanBePickedUp(AActor* OtherActor);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Items")
	void OnPickedUp(AActor* OtherActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Items")
	FItemKey GetPickup();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Items")
	FString GetPickupCategory();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Items")
	FName GetPickupName();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Items")
	int32 GetPickupQuantity();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Items")
	UStaticMeshComponent* GetPickupMesh();
};
