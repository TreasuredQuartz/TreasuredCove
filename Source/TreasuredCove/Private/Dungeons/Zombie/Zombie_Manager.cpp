// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie_Manager.h"
#include "Zombie_Spawner.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AZombie_Manager::AZombie_Manager(const FObjectInitializer& ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AZombie_Manager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AZombie_Manager::SpawnEnemies()
{
	uint8 MaxEnemies = MaxEnemiesAtOnce + (UserArray.Num() * MaxEnemiesPerPlayer);

	for (CurrentSpawnedEnemies; CurrentSpawnedEnemies <= MaxEnemies; CurrentSpawnedEnemies++)
	{
		
		// Get all Player States
		for (APlayerState* PS : UserArray)
		{
			int32 CurrentIndex = UserArray.IndexOfByKey(PS);

			if (PS)
			{
				FVector PlayerLocation = PS->GetPawn()->GetActorLocation();
				AZombie_Spawner* BestSpawner = nullptr;
				float BestDistance = 999999;

				for (AZombie_Spawner* Spawner : Spawners)
				{
					FVector SpawnerLocation = Spawner->GetActorLocation();

					float Distance = FVector::Distance(PlayerLocation, SpawnerLocation);
					if (Distance < BestDistance)
					{
						BestDistance = Distance;
						BestSpawner = Spawner;
					}
				}

				if (BestSpawner != nullptr)
				{
					BestSpawner->SpawnZombie();
				}

				TotalSpawnedEnemies += 1;
			}
		}
	}
}

void AZombie_Manager::SpawnedEnemyRemoved(AActor* RemovedActor, AActor* ResponsibleActor)
{
	if (!RemovedActor || !ResponsibleActor || RemovedActor == ResponsibleActor)
	{
		// This was a suicide
		CurrentSpawnedEnemies -= 1;
		TotalSpawnedEnemies -= 1;

		if (TotalEnemiesToSpawn > TotalSpawnedEnemies)
		{
			SpawnEnemies();
		}
		return;
	}

	if (TotalEnemiesToSpawn == TotalSpawnedEnemies)
	{
		EndWave();
	}
}

void AZombie_Manager::StartWave()
{
	WaveCount += 1;
	if (WaveCount < 5)
	{
		uint8 MaxEnemies = MaxEnemiesAtOnce + (UserArray.Num() * MaxEnemiesPerPlayer);
		TotalEnemiesToSpawn = MaxEnemies * (WaveCount / 5);
	}
	else if (WaveCount < 10)
	{
		TotalEnemiesToSpawn = MaxEnemiesAtOnce + (UserArray.Num() * MaxEnemiesPerPlayer);
	}
	else
	{
		float Multiplier = WaveCount * 0.15;
		uint8 MaxEnemies = MaxEnemiesAtOnce + (UserArray.Num() * MaxEnemiesPerPlayer);
		TotalEnemiesToSpawn = Multiplier + MaxEnemies;
	}

	// Get all Player States
	for (APlayerState* PS : UserArray)
	{
		const APawn* Pawn = PS->GetPawn();
		UGameplayStatics::PlaySound2D(Pawn, WaveStartSound);
	}
}

void AZombie_Manager::EndWave()
{
	// Get all Player States
	for (APlayerState* PS : UserArray)
	{
		const APawn* Pawn = PS->GetPawn();
		UGameplayStatics::PlaySound2D(Pawn, WaveEndSound);
	}
}

void AZombie_Manager::AddUser(APawn* InPawn)
{
	Super::AddUser(InPawn);

	APlayerState* PS = InPawn->GetPlayerState();

	PS->SetScore(0);
}

void AZombie_Manager::AddPoints(APawn* InPawn, int InPoints)
{
	for (APlayerState* PS : UserArray)
	{
		if (InPawn == PS->GetPawn())
		{
			PS->SetScore(PS->GetScore() + InPoints);
		}
		break;
	}
}

bool AZombie_Manager::CheckPoints(APawn* PawnToCheck, int PointsRequired)
{
	bool bSucceeded = false;

	for (APlayerState* PS : UserArray)
	{
		if (PawnToCheck == PS->GetPawn())
		{
			if (PS->GetScore() >= PointsRequired)
			{
				ReducePoints(PawnToCheck, PointsRequired);
				bSucceeded = true;
			}
			break;
		}
	}

	return bSucceeded;
}

void AZombie_Manager::ReducePoints(APawn* InPawn, int InPoints)
{
	for (APlayerState* PS : UserArray)
	{
		if (InPawn == PS->GetPawn())
		{
			PS->SetScore(PS->GetScore() - InPoints);
			break;
		}
	}
}
