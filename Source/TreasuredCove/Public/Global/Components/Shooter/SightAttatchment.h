// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Global/Components/Shooter/ShooterAttatchment.h"
#include "SightAttatchment.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API USightAttatchment : public UShooterAttatchment
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float ADSSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float ADSViewZoom;
};
