// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SGMultiplayerLoadouts.generated.h"

class UMultiplayerLoadout;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API USGMultiplayerLoadouts : public USaveGame
{
	GENERATED_BODY()
	
public:
	TArray<UMultiplayerLoadout*> CustomLoadouts;
};
