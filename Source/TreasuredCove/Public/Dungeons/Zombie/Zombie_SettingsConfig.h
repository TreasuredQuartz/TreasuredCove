// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Zombie_SettingsConfig.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UZombie_SettingsConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
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
	TMap<FString, USoundBase*> AudioLibrary;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZombieManager|Rules")
	uint8 MaxEnemiesAtOnce = 24;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZombieManager|Rules")
	uint8 MaxEnemiesPerPlayer = 6;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZombieManager|Rules")
	float TimeBetweenSpawns = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ZombieManager|Rules")
	float TimeBetweenWaves = 5;

public:
	uint8 GetMaxEnemiesAtOnce(uint8 PlayerCount) const { return MaxEnemiesAtOnce + (PlayerCount * MaxEnemiesPerPlayer); };
	float GetZombieSpawnRate() const { return TimeBetweenSpawns; };
	uint32 GetTotalEnemiesToSpawn(uint32 WaveCount, uint8 PlayerCount) const;
	USoundBase* GetSound(const FString SoundName) const { return WaveStartSound; };
};
