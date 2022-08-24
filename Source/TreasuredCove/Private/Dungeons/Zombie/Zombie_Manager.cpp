// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie_Manager.h"
#include "Zombie_Spawner.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

#include "TimerManager.h"

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

	RandomStream = FRandomStream(RandomSeed);
	CurrentSpawnedEnemies = 0;
	WaveCount = 0;
}

void AZombie_Manager::SpawnEnemies()
{
	ensure(Spawners.Num() > 0);

	uint8 MaxEnemies = MaxEnemiesAtOnce + (UserArray.Num() * MaxEnemiesPerPlayer);

	if (CurrentSpawnedEnemies <= MaxEnemies && CurrentSpawnedEnemies <= TotalEnemiesToSpawn)
	{
		const int32 Index = RandomStream.RandRange(0, Spawners.Num() - 1);
		if (const auto* Zombie = Spawners[Index]->SpawnZombie())
		{
			++CurrentSpawnedEnemies;
			++TotalSpawnedEnemies;

			if (TotalEnemiesToSpawn == TotalSpawnedEnemies)
			{
				GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Current Zombie count: %d\n Max Zombies at once: %d\n Total Spawned Zombies: %d"), CurrentSpawnedEnemies, MaxEnemies, TotalEnemiesToSpawn);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	}
}

void AZombie_Manager::SpawnedEnemyRemoved(AActor* RemovedActor, AActor* ResponsibleActor)
{
	--CurrentSpawnedEnemies;

	if (!RemovedActor || !ResponsibleActor || RemovedActor == ResponsibleActor)
	{
		// This was a suicide
		--TotalSpawnedEnemies;

		if (TotalEnemiesToSpawn > TotalSpawnedEnemies)
		{
			if (SpawnTimerHandle.IsValid()) return;
			
			GetWorldTimerManager().SetTimer(SpawnTimerHandle, TimeBetweenSpawns, true);
			return;
		}
	}

	if (TotalEnemiesToSpawn == TotalSpawnedEnemies)
	{
		EndWave();
		GetWorldTimerManager().SetTimer(StartWaveTimerHandle, TimeBetweenWaves, false);
	}
}

void AZombie_Manager::StartWave()
{
	++WaveCount;
	CurrentSpawnedEnemies = 0;

	if (WaveCount < 5)
	{
		uint8 MaxEnemies = MaxEnemiesAtOnce + (UserArray.Num() * MaxEnemiesPerPlayer);
		TotalEnemiesToSpawn = MaxEnemies * (WaveCount / 5.f);
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

	// Activate all Spawners
	FTimerDelegate SpawnDel;
	SpawnDel.BindUFunction(this, FName("SpawnEnemies"), SpawnTimerHandle);
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, SpawnDel, TimeBetweenSpawns, true);
	// GetWorldTimerManager().SetTimer(SpawnTimerHandle, TimeBetweenSpawns, true);

	// Get all Player States
	for (APlayerState* PS : UserArray)
	{
		const APawn* Pawn = PS->GetPawn();
		UGameplayStatics::PlaySound2D(Pawn, WaveStartSound);
	}

	UE_LOG(LogTemp, Warning, TEXT("Wave %d: Start."), WaveCount);
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
			break;
		}
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
