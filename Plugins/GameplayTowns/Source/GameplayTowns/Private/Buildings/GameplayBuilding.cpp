// Fill out your copyright notice in the Description page of Project Settings.


#include "Buildings/GameplayBuilding.h"
#include "Library/GameplayLibrary.h"
#include "Buildings/GameplayRoom.h"
#include "TownSystem/TownSystemComponent.h"
#include "TownSystem/TownSystemInterface.h"

#include "Components/BoxComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"

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

void AGameplayBuilding::OnConstruction(const FTransform& Transform)
{
	if (!RoomClasses.IsEmpty() && (RoomClasses.Num() != Rooms.Num() || Rooms.Contains(nullptr)))
	{
		int i = -1;
		Rooms.Reserve(RoomClasses.Num());
		for (const auto& RoomClass : RoomClasses)
		{
			if (Rooms.IsValidIndex(++i))
			{
				if (Rooms[i])
				{
					if (Rooms[i]->GetClass() == RoomClass)
					{
						continue;
					}
					else
					{
						Rooms[i]->Destroy();
						Rooms[i] = nullptr;
					}
				}
				if (AGameplayRoom* NewRoom = GetWorld()->SpawnActorDeferred<AGameplayRoom>(RoomClass, Transform, this))
				{
					Rooms[i] = NewRoom;
					NewRoom->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
					NewRoom->SetOwningBuilding(this);
					NewRoom->SetOwner(this);
					UGameplayStatics::FinishSpawningActor(NewRoom, Transform);
				}
			}
			else if (AGameplayRoom* NewRoom = GetWorld()->SpawnActorDeferred<AGameplayRoom>(RoomClass, Transform, this))
			{
				Rooms.Add(NewRoom);
				NewRoom->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
				NewRoom->SetOwningBuilding(this);
				NewRoom->SetOwner(this);
				UGameplayStatics::FinishSpawningActor(NewRoom, Transform);
			}
		}
	}
}

void AGameplayBuilding::EnterBuilding(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (UTownSystemComponent* TownSystem = UGameplayLibrary::GetGameplayTownsComponent(OtherActor))
	{
		AdmitSingleOccupent(TownSystem);
		APawn* Pawn = Cast<APawn>(OtherActor);
		if (Pawn)
		{
			OnEnteredBuilding(Pawn);
		}
	}
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
