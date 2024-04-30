// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombie_Manager.h"
#include "Zombie_Spawner.h"
#include "Zombie_SettingsConfig.h"
#include "TeamComponent.h"
#include "TeamManager.h"
#include "GACharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "ASHealth.h"

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
	ZombieTeamManager = NewObject<UTeamManager>();
}

bool AZombie_Manager::DetermineZombieCanSprint() const
{
	switch (WaveCount)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			return false;
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			return (FMath::RandRange(1, 20) <= WaveCount * 1.f);
			break;
		default:
			return true;
	}
}

void AZombie_Manager::SpawnEnemies()
{
	ensure(Spawners.Num() > 0);

	uint8 MaxEnemies = Settings->GetMaxEnemiesAtOnce(UserArray.Num());

	if (CurrentSpawnedEnemies <= MaxEnemies && CurrentSpawnedEnemies <= TotalEnemiesToSpawn)
	{
		const int32 Index = RandomStream.RandRange(0, Spawners.Num() - 1);
		if (const auto* Zombie = Spawners[Index]->SpawnZombie())
		{
			if (UTeamComponent* TeamComp = Zombie->GetComponentByClass<UTeamComponent>())
			{
				ZombieTeamManager->AddTeammate(Zombie->GetController());
			}

			if (UGACharacterMovementComponent* MoveComp = Zombie->GetComponentByClass<UGACharacterMovementComponent>())
			{
				MoveComp->SetCanSprint(DetermineZombieCanSprint());
			}

			if (UAbilitySystemComponent* AbilitySystem = Zombie->GetComponentByClass<UAbilitySystemComponent>())
			{
				FProperty* HealthProperty = FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, Health));
				AbilitySystem->SetNumericAttributeBase(HealthProperty, 8 * WaveCount);
			}

			++CurrentSpawnedEnemies;
			++TotalSpawnedEnemies;

			if (TotalSpawnedEnemies == TotalEnemiesToSpawn)
			{
				GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Current Zombie count: %d\n Max Zombies at once: %d\n Total Spawned Zombies: %d\n Total Zombies to spawn: %d"), CurrentSpawnedEnemies, MaxEnemies, TotalSpawnedEnemies, TotalEnemiesToSpawn);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	}
}

void AZombie_Manager::SpawnedEnemyRemoved(const AActor* RemovedActor, const AActor* ResponsibleActor)
{
	--CurrentSpawnedEnemies;

	if (Cast<APawn>(RemovedActor))
	{
		ZombieTeamManager->RemoveTeammate(Cast<APawn>(RemovedActor)->GetController());
	}

	if (WaveCount > 2 && WaveCount < 10)
	{
		for (AController* Controller : ZombieTeamManager->GetTeamMembersControllers())
		{
			if (Controller && Controller->GetPawn())
			{
				if (UGACharacterMovementComponent* MoveComp = Controller->GetPawn()->GetComponentByClass<UGACharacterMovementComponent>())
				{
					MoveComp->SetCanSprint(true);
				}
			}
		}
	}

	if (!RemovedActor || !ResponsibleActor || RemovedActor == ResponsibleActor)
	{
		// This was a suicide or a mistake
		--TotalSpawnedEnemies;

		if (TotalEnemiesToSpawn > TotalSpawnedEnemies)
		{
			if (SpawnTimerHandle.IsValid()) return;
			
			GetWorldTimerManager().SetTimer(SpawnTimerHandle, Settings->GetZombieSpawnRate(), true);
			return;
		}
	}

	/*******************************************************************************************************/
	/**/ UE_LOG(LogTemp, Warning, TEXT("Zombie killed; %d Zombies left in wave."), ZombieTeamManager->GetTeamMembersControllers().Num()); /**/
	/*******************************************************************************************************/

	if (const APawn* Pawn = Cast<APawn>(ResponsibleActor))
	{
		AddPoints(Pawn, 50);
	}

	if (CurrentSpawnedEnemies == 0 && TotalEnemiesToSpawn == TotalSpawnedEnemies)
	{
		FTimerDelegate StartDel;
		StartDel.BindUFunction(this, FName("StartWave"), StartWaveTimerHandle);
		GetWorldTimerManager().SetTimer(StartWaveTimerHandle, StartDel, Settings->TimeBetweenWaves, false, Settings->TimeBetweenWaves);

		EndWave();
	}
}

void AZombie_Manager::StartWave()
{
	check(Settings);

	++WaveCount;
	TotalSpawnedEnemies = 0;
	CurrentSpawnedEnemies = 0;
	TotalEnemiesToSpawn = Settings->GetTotalEnemiesToSpawn(WaveCount, UserArray.Num());

	// Activate all Spawners
	FTimerDelegate SpawnDel;
	SpawnDel.BindUFunction(this, FName("SpawnEnemies"), SpawnTimerHandle);
	const float ZombieSpawnRates = Settings->GetZombieSpawnRate();
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, SpawnDel, ZombieSpawnRates, true, ZombieSpawnRates);

	// Get all Player States
	for (APlayerState* PS : UserArray)
	{
		const APawn* Pawn = PS->GetPawn();
		ZombieTeamManager->InformOfEnemy(PS->GetOwningController());
		UGameplayStatics::PlaySound2D(Pawn, Settings->WaveStartSound);
	}

	/**************************************************************************/
	/**/ UE_LOG(LogTemp, Warning, TEXT("Wave %d: Starting..."), WaveCount); /**/
	/**************************************************************************/
}

void AZombie_Manager::EndWave()
{
	// Get all Player States
	for (APlayerState* PS : UserArray)
	{
		const APawn* Pawn = PS->GetPawn();
		UGameplayStatics::PlaySound2D(Pawn, Settings->WaveEndSound);
	}

	/************************************************************************/
	/**/ UE_LOG(LogTemp, Warning, TEXT("Wave %d: Ending..."), WaveCount); /**/
	/************************************************************************/
}

void AZombie_Manager::AddUser(APawn* InPawn)
{
	Super::AddUser(InPawn);

	if (APlayerState* PS = InPawn->GetPlayerState())
	{
		PS->SetScore(0);
	}
}

void AZombie_Manager::AddPoints(const APawn* InPawn, int InPoints)
{
	if (UserArray.IsEmpty()) return;

	for (APlayerState* PS : UserArray)
	{
		if (InPawn == PS->GetPawn())
		{
			PS->SetScore(PS->GetScore() + InPoints);
			break;
		}
	}
}

bool AZombie_Manager::CheckPoints(const APawn* PawnToCheck, int PointsRequired)
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

void AZombie_Manager::ReducePoints(const APawn* InPawn, int InPoints)
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
