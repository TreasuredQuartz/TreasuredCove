// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/Characters/TeamManager.h"
#include "Global/Components/Characters/TeamComponent.h"

void UTeamManager::SetTeamSize(int32 NewTeamSize)
{
	if (NewTeamSize > 0)
	{
		Team.Reserve(NewTeamSize);
	}
}

void UTeamManager::AddTeammate(AController* NewTeammate)
{
	if (NewTeammate == nullptr || Team.Contains(NewTeammate)) return;

	Team.Add(NewTeammate);
	if (APawn* Pawn = NewTeammate->GetPawn())
	{
		if (UTeamComponent* TComp = Pawn->GetComponentByClass<UTeamComponent>())
		{
			TComp->SetManager(this);
		}
	}
}

void UTeamManager::RemoveTeammate(AController* OldTeammate)
{
	if (OldTeammate == nullptr || !Team.Contains(OldTeammate)) return;

	Team.RemoveSwap(OldTeammate, false);
}

void UTeamManager::InformOfEnemy(AController* InEnemy)
{
	if (InEnemy == nullptr) return;

	if (Attackers.Contains(InEnemy)) return;

	Attackers.Add(InEnemy);
}

void UTeamManager::InformOfInjury(AController* InjuredTeammate)
{

}

TArray<APawn*> UTeamManager::GetPhysicalTeamMembers() const
{
	TArray<APawn*> TeamMembers;

	for (AController* Member : Team)
	{
		if (APawn* Pawn = Member->GetPawn())
		{
			TeamMembers.Add(Pawn);
		}
	}

	return TeamMembers;
}

TArray<APawn*> UTeamManager::GetPhysicalEnemies() const
{
	TArray<APawn*> Enemies;

	for (AController* InEnemy : Attackers)
	{
		if (APawn* EnemyPawn = InEnemy->GetPawn())
		{
			Enemies.Add(EnemyPawn);
		}
	}
	
	return Enemies;
}

void UTeamManager::InformOfObjective(AActor* Objective)
{

}
