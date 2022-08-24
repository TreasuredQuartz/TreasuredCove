// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Zombie_Spawner.generated.h"

class AZombie_Manager;

UCLASS()
class TREASUREDCOVE_API AZombie_Spawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZombie_Spawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Must be marked as UFUNCTION for delegate binding
	UFUNCTION()
	void OnZombieDamaged(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
public:	
	// Actor to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APawn> ActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AZombie_Manager* Manager;

	AActor* SpawnZombie() const;
};
