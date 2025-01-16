// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayCivilization.generated.h"

class AGameplayTown;

/**
 * 
 */
UCLASS()
class GAMEPLAYTOWNS_API UGameplayCivilization : public UObject
{
	GENERATED_BODY()
	
public:
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Towns")
	TArray<AGameplayTown*> Towns;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Towns")
	TMap<TSubclassOf<AGameplayTown>, int32> AvailableTownClasses;
};
