// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayTown.h"
#include "GameplayTownState.h"
#include "GameplayJobSite.h"
#include "GameplayResidence.h"
#include "TownSystemComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// Sets default values
AGameplayTown::AGameplayTown()
{

}

// Called when the game starts or when spawned
void AGameplayTown::BeginPlay()
{
	Super::BeginPlay();

	FTransform StateTransform = GetTransform();
	TownState = GetWorld()->SpawnActorDeferred<AGameplayTownState>(TownStateClass, StateTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (TownState)
	{
		TownState->SetTown(this);
		UGameplayStatics::FinishSpawningActor(TownState, StateTransform);
	}
}

//
void AGameplayTown::UpdateResources()
{
	Resources;
	for (AGameplayJobSite* JobSite : JobSites)
	{
		// JobSite->Resource;
		continue;
	}
}


//----- Peoples -----//


void AGameplayTown::AddOccupent(AActor* NewOccupent)
{

}

void AGameplayTown::AddContractor(AActor* NewContractor)
{

}

void AGameplayTown::AddResident(AActor* NewResident)
{

}

void AGameplayTown::AddCitizen(AActor* NewCitizen)
{

}

void AGameplayTown::RemoveOccupent(AActor* OldOccupent)
{

}

void AGameplayTown::RemoveContractor(AActor* OldContractor)
{

}

void AGameplayTown::RemoveResident(AActor* OldResident)
{

}

void AGameplayTown::RemoveCitizen(AActor* OldCitizen)
{

}


//----- Structures -----//


void AGameplayTown::CreateJobSite(AGameplayJobSite* InJobSite)
{

}

void AGameplayTown::CreateResidence(AGameplayResidence* InResidence)
{

}

void AGameplayTown::DestroyJobSite(AGameplayJobSite* InJobSite)
{

}

void AGameplayTown::DestroyResidence(AGameplayResidence* InResidence)
{

}

void AGameplayTown::AddJobSiteToTown(AGameplayJobSite* InJobSite)
{

}

void AGameplayTown::AddResidenceToTown(AGameplayResidence* InResidence)
{

}

void AGameplayTown::RemoveJobSiteFromTown(AGameplayJobSite* InJobSite)
{

}

void AGameplayTown::RemoveResidenceFromTown(AGameplayResidence* InResidence)
{

}