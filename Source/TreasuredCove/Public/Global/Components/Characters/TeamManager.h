// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TeamManager.generated.h"

class UTeamComponent;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UTeamManager : public UObject
{
	GENERATED_BODY()
	
public:
	// Every attacker against the team
	TArray<APawn*> Attackers;

	// Every teammember
	TArray<UTeamComponent*> Team;

public:
	// Adds additional component to inner array.
	void AddTeammate(UTeamComponent* NewTeammate);

	// Adds additional attacker to inner array.
	void InformOfEnemy(APawn* InEnemy);

	// Fires delegate to alert teammates of injury.
	void InformOfInjury(UTeamComponent* InjuredTeammate);

	// Fires delegate to alert about objective.
	void InformOfObjective(AActor* Objective);
};
