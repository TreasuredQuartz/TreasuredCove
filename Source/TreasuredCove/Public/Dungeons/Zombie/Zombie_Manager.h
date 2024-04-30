// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayManager.h"
#include "Zombie_Manager.generated.h"

class UZombie_SettingsConfig;
class AZombie_Spawner;
class UTeamManager;

UCLASS()
class TREASUREDCOVE_API AZombie_Manager 
	: public AGameplayManager
{
	GENERATED_BODY()
	
private:
	FRandomStream RandomStream;
	FTimerHandle SpawnTimerHandle;
	FTimerHandle StartWaveTimerHandle;

public:	
	// Sets default values for this actor's properties
	AZombie_Manager(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZombieManager|Rules")
	TArray<AZombie_Spawner*> Spawners;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool DetermineZombieCanSprint() const;

	UFUNCTION()
	void SpawnEnemies();

	uint32 WaveCount;
	uint32 TotalEnemiesToSpawn;
	uint32 CurrentSpawnedEnemies;
	uint32 TotalSpawnedEnemies;

	UPROPERTY()
	UTeamManager* ZombieTeamManager;
	UPROPERTY(EditAnywhere, Category = "ZombieManager|Rules")
	UZombie_SettingsConfig* Settings;
public:
	/* Called from zombie spawners, when spawned zombies die. */
	void SpawnedEnemyRemoved(const AActor* RemovedActor, const AActor* ResponsibleActor);
	/* Triggers timers for spawning. */
	UFUNCTION(BlueprintCallable)
	void StartWave();
	/* Ends timers, then sets timer to start next round. */
	UFUNCTION(BlueprintCallable)
	void EndWave();

	UFUNCTION(BlueprintCallable)
	void AddPoints(const APawn* InPawn, int InPoints);
	UFUNCTION(BlueprintCallable)
	bool CheckPoints(const APawn* PawnToCheck, int PointsRequired);
	void ReducePoints(const APawn* InPawn, int InPoints);

	virtual void AddUser(APawn* InPawn) override;
};
