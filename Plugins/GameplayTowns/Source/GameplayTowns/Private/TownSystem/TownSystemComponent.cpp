// Fill out your copyright notice in the Description page of Project Settings.


#include "TownSystemComponent.h"
#include "GameplayTown.h"
#include "GameplayJobSite.h"
#include "GameplayResidence.h"

#include "GameFramework/Pawn.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values for this component's properties
UTownSystemComponent::UTownSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UTownSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void UTownSystemComponent::ArriveAtBuilding(AGameplayBuilding* NewCurrentBuilding)
{
	SetCurrentBuilding(NewCurrentBuilding);
}

void UTownSystemComponent::SetCurrentBuilding(AGameplayBuilding* NewCurrentBuilding)
{
	CurrentBuilding = NewCurrentBuilding;
	UpdateCurrentBuilding.Broadcast(CurrentBuilding);
}

void UTownSystemComponent::SetDesiredLocation(FVector NewDesiredLocation)
{
	DesiredLocation = NewDesiredLocation;
	UpdateDesiredLocation.Broadcast(NewDesiredLocation);
}

void UTownSystemComponent::SetTargetBuilding(AGameplayBuilding* NewTargetBuilding)
{
	TargetBuilding = NewTargetBuilding;
	UpdateTargetBuilding.Broadcast(TargetBuilding);
}

bool UTownSystemComponent::AttemptJobSiteRequest(FName ResourceTypeToRequest, float ResourceAmountToRequest, float CurrencyForRequest)
{
	bool bSucceeded = false;

	AGameplayJobSite* CurrentJobSite = Cast<AGameplayJobSite>(CurrentBuilding);

	if (CurrentJobSite)
	{
		bSucceeded = CurrentJobSite->CanMakeRequest(GetOwner(), ResourceTypeToRequest, ResourceAmountToRequest, CurrencyForRequest);
	}

	return bSucceeded;
}

bool UTownSystemComponent::AttemptUseResidence(FName Reason)
{
	AGameplayResidence* CurrentResidence = Cast<AGameplayResidence>(CurrentBuilding);

	if (CurrentResidence == Home)
	{
		if (Reason == FName("IsFatigued"))
		{
			if (CurrentResidence->bHasBedding)
			{
				CurrentResidence->ReduceFatigue(GetOwner());
				return true;
			}
		}
		else if (Reason == FName("IsHungry"))
		{
			if (CurrentResidence->bHasFood)
			{
				CurrentResidence->ReduceHunger(GetOwner());
				return true;
			}
		}
	}

	return false;
}

