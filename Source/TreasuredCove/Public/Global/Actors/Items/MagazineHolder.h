// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Global/Actors/GAActor.h"
#include "MagazineHolder.generated.h"

class UMagazineAttatchment;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API AMagazineHolder : public AGAActor
{
	GENERATED_BODY()
	
public:
	TArray<UMagazineAttatchment*> Magazines;
};
