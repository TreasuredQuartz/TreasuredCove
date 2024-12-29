// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/Framework/GAGameMode.h"
#include "Global/Framework/GAGameState.h"
#include "Global/Framework/GAPlayerController.h"
#include "Global/Framework/GAHUD.h"
#include "Global/Actors/GACharacter.h"


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