// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectUIData.h"
#include "GameplayEffectUIData_Icon.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UGameplayEffectUIData_Icon : public UGameplayEffectUIData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data)
	UTexture2D* Icon;
};
