// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie_Spawner.h"
#include "Zombie_Manager.h"

#include "GACharacter.h"

#include "Engine/World.h"
#include "Gameframework/Gamestate.h"
#include "Gameframework/PlayerState.h"
#include "kismet/GameplayStatics.h"

// Sets default values
AZombie_Spawner::AZombie_Spawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AZombie_Spawner::BeginPlay()
{
	Super::BeginPlay();
}

//
AActor* AZombie_Spawner::SpawnZombie() const
{
	APawn* Zombie = nullptr;
	if (UWorld* World = GetWorld())
	{
		Zombie = World->SpawnActor<APawn>(ActorToSpawn, GetTransform());
		if (Zombie)
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawning Zombie..."));
			Zombie->SpawnDefaultController();
			Zombie->OnTakeAnyDamage.AddDynamic(this, &AZombie_Spawner::OnZombieDamaged);
		}
	}

	return Zombie;
}

//
void AZombie_Spawner::OnZombieDamaged(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	APawn* PawnCauser = Cast<APawn>(DamageCauser);
	
	if (!PawnCauser || !DamagedActor) return;

	Manager->AddPoints(PawnCauser, 10);

	if (DamagedActor->ActorHasTag(FName("death.headshot")))
	{
		Manager->AddPoints(PawnCauser, 20);
	}
	else if (DamagedActor->ActorHasTag(FName("death.melee")))
	{
		Manager->AddPoints(PawnCauser, 30);
	}
}

