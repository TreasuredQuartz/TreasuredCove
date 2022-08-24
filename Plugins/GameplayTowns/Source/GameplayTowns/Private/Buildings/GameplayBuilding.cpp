// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayBuilding.h"
#include "TownSystemComponent.h"
#include "TownSystemInterface.h"

#include "Components/BoxComponent.h"
#include "NavigationSystem.h"

// Sets default values
AGameplayBuilding::AGameplayBuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	EntryPoint =
		CreateDefaultSubobject<UBoxComponent>(TEXT("EntryPoint"));
	EntryPoint->OnComponentBeginOverlap.AddDynamic(this, &AGameplayBuilding::EnterBuilding);
	EntryPoint->OnComponentEndOverlap.AddDynamic(this, &AGameplayBuilding::ExitBuilding);

	USceneComponent* Root =
		CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	RootComponent = Root;
	EntryPoint->SetupAttachment(RootComponent);
}

void AGameplayBuilding::EnterBuilding(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->GetClass()->ImplementsInterface(UTownSystemInterface::StaticClass()))
	{
		UTownSystemComponent* TownSystem = Cast<ITownSystemInterface>(OtherActor)->GetTownSystemComponent();

		if (TownSystem)
		{
			AdmitSingleOccupent(TownSystem);
		}

		APawn* Pawn = Cast<APawn>(OtherActor);
		if (Pawn)
		{
			OnEnteredBuilding(Pawn);
		}
	}
	else UE_LOG(LogTemp, Warning, TEXT("Actor entered building, but does not implement UTownInterface!"));
}

void AGameplayBuilding::ExitBuilding(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetClass()->ImplementsInterface(UTownSystemInterface::StaticClass()))
	{
		UTownSystemComponent* TownSystem = Cast<ITownSystemInterface>(OtherActor)->GetTownSystemComponent();

		if (TownSystem)
		{
			ReleaseSingleOccupent(TownSystem);
		}

		APawn* Pawn = Cast<APawn>(OtherActor);
		if (Pawn)
		{
			OnExitedBuilding(Pawn);
		}
	}
}

void AGameplayBuilding::AdmitSingleOccupent(UTownSystemComponent* NewOccupent)
{
	bool bIsNewOccupent = true;

	for (UTownSystemComponent* Occupent : Occupents)
	{
		if (Occupent == NewOccupent)
		{
			bIsNewOccupent = false;
			break;
		}
	}

	if (bIsNewOccupent)
	{
		NewOccupent->ArriveAtBuilding(this);
		Occupents.Add(NewOccupent);
	}
}

void AGameplayBuilding::ReleaseSingleOccupent(UTownSystemComponent* OldOccupent)
{
	OldOccupent->SetCurrentBuilding(nullptr);

	for (UTownSystemComponent* Occupent : Occupents)
	{
		if (Occupent == OldOccupent)
		{
			Occupents.Remove(Occupent);
			break;
		}
	}
}

void AGameplayBuilding::ReleaseAllOccupents()
{
	for (UTownSystemComponent* Occupent : Occupents)
	{
		FVector Direction = (GetActorLocation() - EntryPoint->GetComponentLocation()).GetSafeNormal();
		FVector Start = EntryPoint->GetComponentLocation();
		FVector End = Start + (Direction * 1000);

		FVector NewLocation = FVector(
			End.X + FMath::RandRange(-250, 250), 
			End.Y + FMath::RandRange(-250, 250), 
			End.Z);

		Occupent->SetCurrentBuilding(nullptr);
		Occupent->SetDesiredLocation(NewLocation);
	}
}
