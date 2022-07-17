// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie_Spawner.h"
#include "Zombie_Manager.h"

#include "GACharacter.h"

#include "Engine/World.h"

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
void AZombie_Spawner::SpawnZombie()
{
	UWorld* World = GetWorld();
	AGACharacter* Zombie = 
		World->SpawnActor<AGACharacter>(ActorToSpawn, GetTransform());
	Zombie->OnTakeAnyDamage.AddDynamic(this, &AZombie_Spawner::OnZombieDamaged);
}

//
void AZombie_Spawner::OnZombieDamaged(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	AGACharacter* DamagedZombie = Cast<AGACharacter>(DamagedActor);

	APawn* PawnCauser = Cast<APawn>(DamageCauser);

	if (!PawnCauser)
	{
		return;
	}

	Manager->AddPoints(PawnCauser, 10);

	if (DamagedZombie->AIInfo.CurrentAITargetStats.CurrentHealth <= 0)
	{
		if (DamagedZombie->ActorHasTag(FName("death.headshot")))
		{
			Manager->AddPoints(PawnCauser, 20);
		}
		else if (DamagedZombie->ActorHasTag(FName("death.melee")))
		{
			Manager->AddPoints(PawnCauser, 30);
		}
	}
}

