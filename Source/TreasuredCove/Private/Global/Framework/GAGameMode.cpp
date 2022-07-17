// Fill out your copyright notice in the Description page of Project Settings.

#include "GAGameMode.h"
#include "GAGameState.h"
#include "GAPlayerController.h"
#include "GACharacter.h"
#include "GAHUD.h"


AGAGameMode::AGAGameMode(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	// The Game State that is paired with this game mode
	GameStateClass = AGAGameState::StaticClass();

	// The Controller we want to use
	PlayerControllerClass = AGAPlayerController::StaticClass();

	// The default HUD for our controller
	HUDClass = AGAHUD::StaticClass();

	// Our Default Pawn that is controlled by our controller
	DefaultPawnClass = AGACharacter::StaticClass();

	// Finally the player state.
}