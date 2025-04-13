// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/Characters/PauseAreaComponent.h"
#include "Global/Actors/GAPauseStudioPawn.h"

#include "GameFramework/Character.h"

#include "Kismet/GameplayStatics.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"

// Sets default values for this component's properties
UPauseAreaComponent::UPauseAreaComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UPauseAreaComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UPauseAreaComponent::EnterPauseArea_Client_Implementation()
{
	if (PauseArea == nullptr)
	{
		// We need the class specified before we can spawn text actors!
		if (!PauseAreaClass)
			return;

		FVector Location = FVector::ZeroVector;
		Location.Z -= 100000;
		const FTransform spawnTransform = FTransform(FRotator::ZeroRotator, Location);
		AGAPauseStudioPawn* NewPauseAreaActor = GetWorld()->SpawnActorDeferred<AGAPauseStudioPawn>(PauseAreaClass, spawnTransform, GetOwner(), nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		// Possible to get bad spawns and this checks that we did in fact spawn
		if (!NewPauseAreaActor)
			return;

		NewPauseAreaActor->SetActorEnableCollision(false);
		NewPauseAreaActor->OnDestroyed.AddDynamic(this, &UPauseAreaComponent::OnPauseAreaDestroyed);
		UGameplayStatics::FinishSpawningActor(NewPauseAreaActor, spawnTransform);

		PauseArea = NewPauseAreaActor;

		// In client/server only the server can activate a DataLayer
		if (GetOwner()->HasAuthority())
		{
			if (UDataLayerManager* DataLayerSubsystem = GetWorld()->GetDataLayerManager())
			{
				DataLayerSubsystem->SetDataLayerRuntimeState(PauseAreaDataLayer, EDataLayerRuntimeState::Loaded);
				const UDataLayerInstance* PauseInstance = DataLayerSubsystem->GetDataLayerInstanceFromAsset(PauseAreaDataLayer);

				PauseInstance->AddActor(PauseArea);
			}
		}
	}

	// In client/server only the server can activate a DataLayer
	if (GetOwner()->HasAuthority())
	{
		if (UDataLayerManager* DataLayerSubsystem = GetWorld()->GetDataLayerManager())
		{
			DataLayerSubsystem->SetDataLayerRuntimeState(PauseAreaDataLayer, EDataLayerRuntimeState::Activated);
		}
	}

	PauseArea->OnEnter();
}

void UPauseAreaComponent::ExitPauseArea()
{
	if (PauseArea) PauseArea->OnExit();

	// In client/server only the server can activate a DataLayer
	if (GetOwner()->HasAuthority())
	{
		if (UDataLayerManager* DataLayerSubsystem = GetWorld()->GetDataLayerManager())
		{
			DataLayerSubsystem->SetDataLayerRuntimeState(PauseAreaDataLayer, EDataLayerRuntimeState::Unloaded);
		}
	}
}

void UPauseAreaComponent::OnPauseAreaDestroyed(AActor* DestroyedActor)
{
	PauseArea = nullptr;
}


