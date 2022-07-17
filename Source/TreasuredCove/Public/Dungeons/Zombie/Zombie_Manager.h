// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayManager.h"
#include "Zombie_Manager.generated.h"

class AZombie_Spawner;

UCLASS()
class TREASUREDCOVE_API AZombie_Manager 
	: public AGameplayManager
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZombie_Manager(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZombieManager|Rules")
	int32 EnemyAdditionPerWaveBonus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZombieManager|Rules")
	int32 EnemyMovementBoostPerWave;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZombieManager|Rules")
	int32 EnemyStatBoostPerWave;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZombieManager|Rules")
	int32 MaxWaveMovementBoost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZombieManager|Rules")
	int32 MaxWaveStatBoost;

	/* Audio */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZombieManager|Rules")
	USoundBase* WaveStartSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZombieManager|Rules")
	USoundBase* WaveEndSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZombieManager|Rules")
	uint8 MaxEnemiesAtOnce = 24;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZombieManager|Rules")
	uint8 MaxEnemiesPerPlayer = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZombieManager|Rules")
	TArray<AZombie_Spawner*> Spawners;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnEnemies();
	void SpawnedEnemyRemoved(AActor* RemovedActor, AActor* ResponsibleActor);
	uint32 WaveCount;
	uint32 TotalEnemiesToSpawn;

	uint32 CurrentSpawnedEnemies;
	uint32 TotalSpawnedEnemies;
public:
	/*  */
	UFUNCTION(BlueprintCallable)
	void StartWave();
	UFUNCTION(BlueprintCallable)
	void EndWave();

	UFUNCTION(BlueprintCallable)
	void AddPoints(APawn* InPawn, int InPoints);
	UFUNCTION(BlueprintCallable)
	bool CheckPoints(APawn* PawnToCheck, int PointsRequired);
	void ReducePoints(APawn* InPawn, int InPoints);

	virtual void AddUser(APawn* InPawn) override;
};
