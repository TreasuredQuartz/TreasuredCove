// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OnAttributeModifiedEvent.h"
#include "Zombie_Spawner.generated.h"

class AZombie_Manager;

UCLASS()
class TREASUREDCOVE_API AZombie_Spawner : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category = "Zombie Spawner")
	float SpawnRadius = 1.f;
	
public:
	float GetSpawnRadius() const { return SpawnRadius; };

public:	
	// Sets default values for this actor's properties
	AZombie_Spawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Must be marked as UFUNCTION for delegate binding
	UFUNCTION()
	void OnZombieDamaged(const FOnAttributeModifiedPayload& Payload);
	// Bound to the Health component's delegate: OnHealthZeroed
	UFUNCTION()
	void OnZombieDied(const AActor* Victim, const AActor* ResponsibleActor);
public:	
	// Actor to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APawn> ActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AZombie_Manager* Manager;

	APawn* SpawnZombie() const;
	FTransform GetSpawnTransform() const;
};
