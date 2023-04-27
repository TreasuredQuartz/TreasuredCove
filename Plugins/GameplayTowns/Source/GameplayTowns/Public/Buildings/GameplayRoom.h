// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayRoom.generated.h"

class AGameplayBuilding;

UCLASS()
class GAMEPLAYTOWNS_API AGameplayRoom : public AActor
{
	GENERATED_BODY()

private:
	AGameplayBuilding* OwningBuilding;

public:	
	// Sets default values
	AGameplayRoom();

	void SetOwningBuilding(AGameplayBuilding* NewOwner);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

		
};
