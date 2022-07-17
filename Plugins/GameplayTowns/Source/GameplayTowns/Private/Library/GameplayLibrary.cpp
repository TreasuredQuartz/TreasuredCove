// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayLibrary.h"

void UGameplayLibrary::SetGameTime(float InDeltaTime, float InGameSpeed, float InInitialGameTime, float* OutGameTime, int* OutDaysPassed)
{
	*OutDaysPassed = ((InDeltaTime * InGameSpeed) + InInitialGameTime) / 24.f;
	*OutGameTime = fmodf( ((InDeltaTime * InGameSpeed) + InInitialGameTime), 24.f );
}