// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseAreaComponent.h"
#include "GAPauseStudioPawn.h"

#include "GameFramework/Character.h"

#include "Kismet/GameplayStatics.h"

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

		FVector Location = GetOwner()->GetActorLocation();
		Location.Z += 500;
		const FTransform spawnTransform = FTransform(FRotator::ZeroRotator, Location);
		AGAPauseStudioPawn* NewPauseAreaActor = GetWorld()->SpawnActorDeferred<AGAPauseStudioPawn>(PauseAreaClass, spawnTransform, GetOwner(), nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		// Possible to get bad spawns and this checks that we did in fact spawn
		if (!NewPauseAreaActor)
			return;

		NewPauseAreaActor->SetActorEnableCollision(false);
		NewPauseAreaActor->OnDestroyed.AddDynamic(this, &UPauseAreaComponent::OnPauseAreaDestroyed);
		UGameplayStatics::FinishSpawningActor(NewPauseAreaActor, spawnTransform);

		PauseArea = NewPauseAreaActor;
		PauseArea->OnEnter();
	}
	else PauseArea->OnEnter();
}

void UPauseAreaComponent::ExitPauseArea()
{
	if (PauseArea) PauseArea->OnExit();
}

void UPauseAreaComponent::OnPauseAreaDestroyed(AActor* DestroyedActor)
{
	PauseArea = nullptr;
}


