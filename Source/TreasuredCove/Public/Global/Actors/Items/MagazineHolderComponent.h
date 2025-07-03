// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MagazineHolderComponent.generated.h"

class UMagazineAttachment;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UMagazineHolderComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	TArray<UMagazineAttachment*> Magazines;
};
