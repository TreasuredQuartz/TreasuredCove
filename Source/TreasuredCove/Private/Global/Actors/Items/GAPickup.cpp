// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Actors/Items/GAPickup.h"
#include "Global/Interfaces/InteractionInterface.h"
#include "Components/BoxComponent.h"

// Sets default values
AGAPickup::AGAPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root =
		CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	RootComponent = Root;
}

// Called when the game starts or when spawned
void AGAPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
//void AGAPickup::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void AGAPickup::SetPickupInfo(FItemKey& InInfo)
{
	PickupInfo = InInfo;
}

bool AGAPickup::OnCanBePickedUp_Implementation(AActor* OtherActor)
{
	return true;
}

bool AGAPickup::CanBePickedUp_Implementation(AActor* OtherActor)
{
	if (bBeingPickedUp) return false;

	bBeingPickedUp = Execute_OnCanBePickedUp(this, OtherActor);
	return bBeingPickedUp;
}

void AGAPickup::OnPickedUp_Implementation(AActor* OtherActor)
{
	if (OtherActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass())) 
	{ 
		IInteractionInterface::Execute_AddItemToRepository(OtherActor, Category, PickupInfo);
		this->Destroy();
	}
}

