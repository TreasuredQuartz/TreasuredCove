// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayLibrary.generated.h"

class UTownSystemComponent;

/**
 * 
 */
UCLASS()
class GAMEPLAYTOWNS_API UGameplayLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void SetGameTime(float InDeltaTime, float InGameTime, float InInitialGameTime, float* OutGameTime, int* OutDaysPassed);

	static UTownSystemComponent* GetGameplayTownsComponent(AActor* Actor);
};
