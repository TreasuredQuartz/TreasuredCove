// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie_Spawner.h"
#include "Zombie_Manager.h"

#include "HealthComponent.h"

#include "Engine/World.h"
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
FTransform AZombie_Spawner::GetSpawnTransform() const
{
	FTransform Transform = GetActorTransform();

	FVector2D RandPoint = FMath::RandPointInCircle(GetSpawnRadius());

	Transform.SetLocation(GetActorLocation() + FVector(RandPoint.X, RandPoint.Y, 0));

	return Transform;
}

//
APawn* AZombie_Spawner::SpawnZombie() const
{
	APawn* Zombie = nullptr;
	if (UWorld* World = GetWorld())
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Zombie = World->SpawnActor<APawn>(ActorToSpawn, GetSpawnTransform(), Params);
		if (Zombie)
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawning Zombie..."));
			Zombie->SpawnDefaultController();
			if (UHealthComponent* HealthComp = Zombie->GetComponentByClass<UHealthComponent>())
			{
				HealthComp->OnHealthModified.AddDynamic(this, &AZombie_Spawner::OnZombieDamaged);
				HealthComp->OnHealthZeroed.AddDynamic(this, &AZombie_Spawner::OnZombieDied);
			}
		}
	}

	return Zombie;
}

//
void AZombie_Spawner::OnZombieDamaged(const FOnAttributeModifiedPayload& Payload)
{
	const APawn* PawnCauser = Cast<APawn>(Payload.Causer);
	const AActor* DamagedActor = Payload.Victim;
	
	if (!PawnCauser || !DamagedActor || !Manager) return;

	UE_LOG(LogTemp, Warning, TEXT("Spawned Zombie has taken damage..."));
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

void AZombie_Spawner::OnZombieDied(const AActor* Victim, const AActor* ResponsibleActor)
{
	Manager->SpawnedEnemyRemoved(Victim, ResponsibleActor);
	if (UHealthComponent* HealthComp = Victim->GetComponentByClass<UHealthComponent>())
	{
		HealthComp->OnHealthModified.RemoveAll(this);
		HealthComp->OnHealthZeroed.RemoveAll(this);
	}
}

