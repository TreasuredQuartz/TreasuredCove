// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Global/Components/Shooter/ShooterAttachment.h"
#include "SightAttachment.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API USightAttachment : public UShooterAttachment
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float ADSSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float ADSViewZoom;
};
