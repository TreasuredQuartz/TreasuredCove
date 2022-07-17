// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GAGameMode.generated.h"

/** Remember the order of execution in ue4:
 * -Game Mode
 * -Game State
 * -Player Controller
 * -HUD
 * -Pawn
 * -Player State
 */
UCLASS()
class TREASUREDCOVE_API AGAGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Constructor
	AGAGameMode(const FObjectInitializer& ObjectInitializer);
};
