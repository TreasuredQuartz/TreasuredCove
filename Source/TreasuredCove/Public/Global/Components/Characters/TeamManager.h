// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TeamManager.generated.h"

class UTeamComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewAttackerAdded, APawn*, AttackerPawn, APawn*, TeamateInformerPawn);

/**
 * 
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API UTeamManager : public UObject
{
	GENERATED_BODY()
	
private:
	// Every attacker against the team
	TArray<AController*> Attackers;

	// Every teammember
	TArray<AController*> Team;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	FString TeamTag;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AController*> GetTeamMembersControllers() const { return Team; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AController*> GetEnemiesControllers() const { return Attackers; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<APawn*> GetPhysicalTeamMembers() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<APawn*> GetPhysicalEnemies() const;

public:
	// Reserves enough space to contain the new team size.
	void SetTeamSize(int32 NewTeamSize);

	// Adds additional component to inner array.
	UFUNCTION(BlueprintCallable)
	void AddTeammate(AController* NewTeammate);

	// Removes an old teammate from the inner array.
	void RemoveTeammate(AController* OldTeammate);

	// Adds additional attacker to inner array.
	UFUNCTION(BlueprintCallable)
	void InformOfEnemy(AController* InEnemy);

	// Fires delegate to alert teammates of injury.
	void InformOfInjury(AController* InjuredTeammate);

	// Fires delegate to alert about objective.
	void InformOfObjective(AActor* Objective);

public:
	UPROPERTY(BlueprintAssignable)
	FOnNewAttackerAdded OnNewAttackerAdded;
};
