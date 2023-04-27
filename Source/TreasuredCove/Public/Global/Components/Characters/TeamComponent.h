// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TeamComponent.generated.h"

class UTeamManager;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UTeamComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	// Team data storage and communication manager.
	UTeamManager* Manager;

public:	
	// Sets default values for this component's properties
	UTeamComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Called to initialize team manager.
	void FormTeam();

	// Called to add a teammate to the team data object
	void AddTeammate(UTeamComponent* NewTeammate);

	// Convience function to add a group of teammates at once. Just calls AddTeammate for each element in the given array.
	void AddTeammates(TArray<UTeamComponent*>& NewTeammates);

	// Event to inform other teammates of attackers.
	void InformOfEnemy(APawn* InEnemy);

	// Event to inform other teammates that we are injured
	void InformOfInjury();

	// Event to inform other teammates that we found the objective.
	void InformOfObjective(AActor* InObjective);

	// Accessor for all teammates to share data.
	UTeamManager* GetSharedData() const;

	// Ensure single shared manager for all teammates.
	void SetManager(UTeamManager* NewManager);
};
