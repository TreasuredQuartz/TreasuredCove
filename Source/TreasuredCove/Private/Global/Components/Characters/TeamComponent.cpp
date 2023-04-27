// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/Characters/TeamComponent.h"
#include "Global/Components/Characters/TeamManager.h"

// Sets default values for this component's properties
UTeamComponent::UTeamComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTeamComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTeamComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTeamComponent::FormTeam()
{
	Manager = NewObject<UTeamManager>();
	Manager->AddTeammate(this);
}

void UTeamComponent::AddTeammate(UTeamComponent* NewTeammate)
{
	if (!Manager) FormTeam();

	Manager->AddTeammate(NewTeammate);
}

void UTeamComponent::AddTeammates(TArray<UTeamComponent*>& NewTeammates)
{
	for (UTeamComponent* NewTeammate : NewTeammates)
	{
		AddTeammate(NewTeammate);
	}
}

void UTeamComponent::InformOfEnemy(APawn* InEnemy)
{
	Manager->InformOfEnemy(InEnemy);
}

void UTeamComponent::InformOfInjury()
{
	Manager->InformOfInjury(this);
}

void UTeamComponent::InformOfObjective(AActor* InObjective)
{
	Manager->InformOfObjective(InObjective);
}

UTeamManager* UTeamComponent::GetSharedData() const
{
	return Manager;
}

void UTeamComponent::SetManager(UTeamManager* NewManager)
{
	Manager = NewManager;
}


