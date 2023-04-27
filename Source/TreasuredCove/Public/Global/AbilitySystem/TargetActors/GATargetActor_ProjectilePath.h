// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GATargetActor_ProjectilePath.generated.h"

class APlayerController;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API AGATargetActor_ProjectilePath : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
private:
	APlayerController* OwningController;

public:
	/** Initialize and begin targeting logic  */
	virtual void StartTargeting(UGameplayAbility* Ability) override;
};
