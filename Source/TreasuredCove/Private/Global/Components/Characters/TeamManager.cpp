// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/Characters/TeamManager.h"
#include "Global/Components/Characters/TeamComponent.h"

void UTeamManager::AddTeammate(UTeamComponent* NewTeammate)
{
	if (Team.Contains(NewTeammate)) return;

	Team.Add(NewTeammate);
	NewTeammate->SetManager(this);
}

void UTeamManager::InformOfEnemy(APawn* InEnemy)
{
	if (!Attackers.Contains(InEnemy)) Attackers.Add(InEnemy);
}

void UTeamManager::InformOfInjury(UTeamComponent* InjuredTeammate)
{

}

void UTeamManager::InformOfObjective(AActor* Objective)
{

}
