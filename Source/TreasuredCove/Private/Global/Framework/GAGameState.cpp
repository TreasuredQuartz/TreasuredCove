// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/Framework/GAGameState.h"
#include "GameplayClock.h"

AGAGameState::AGAGameState()
{
	GameClock =
		CreateDefaultSubobject<UGameplayClockComponent>(TEXT("GameClock"));
}

float AGAGameState::GetTimeOfDay_Implementation()
{
	return GameClock->GetSolarTime();
}

void AGAGameState::UpdateGameSpeed_Implementation(float SpeedMultiplier)
{
	IGameTimeInterface::Execute_UpdateGameSpeed(GameClock, SpeedMultiplier);
}