// Fill out your copyright notice in the Description page of Project Settings.


#include "GAPickup.h"
#include "InteractionInterface.h"
#include "Components/BoxComponent.h"

// Sets default values
AGAPickup::AGAPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh =
		CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Root =
		CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Overlap =
		CreateDefaultSubobject<UBoxComponent>(TEXT("Overlap"));

	RootComponent = Root;
	Overlap->SetupAttachment(Root);
	Mesh->SetupAttachment(Root);
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

