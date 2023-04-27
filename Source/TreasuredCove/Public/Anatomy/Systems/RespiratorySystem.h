// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Anatomy/Systems/OrganSystem.h"
#include "RespiratorySystem.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API URespiratorySystem : public UOrganSystem
{
	GENERATED_BODY()
public:
	URespiratorySystem();

private:
	TArray<FName> RequiredGasses;
	TArray<FName> ContainedGasses;
};
