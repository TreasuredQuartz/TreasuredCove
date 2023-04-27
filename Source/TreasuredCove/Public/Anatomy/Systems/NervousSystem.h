// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Anatomy/Systems/OrganSystem.h"
#include "NervousSystem.generated.h"

class UEyeOrgan;
class UEarOrgan;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UNervousSystem : public UOrganSystem
{
	GENERATED_BODY()

private:
	TArray<UEyeOrgan*> Eyes;
	TArray<UEarOrgan*> Ears;

public:

};
