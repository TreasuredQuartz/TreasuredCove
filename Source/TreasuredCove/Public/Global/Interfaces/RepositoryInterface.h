// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RepositoryInterface.generated.h"

class URepositoryComponent;

// This class does not need to be modified.
UINTERFACE(Blueprintable, BlueprintType, MinimalAPI)
class URepositoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TREASUREDCOVE_API IRepositoryInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Repository", meta=(DisplayName="GetRepositoryComponent"))
	URepositoryComponent* GetRepositoryComponent() const; 
	virtual URepositoryComponent* GetRepositoryComponent_Implementation() const;
};
