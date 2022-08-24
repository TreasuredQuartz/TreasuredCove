// Fill out your copyright notice in the Description page of Project Settings.


#include "MCWorld_BasicInteractionTool.h"
#include "MCWorld_Manager.h"
#include "MCWorld_Voxel.h"
#include "MCWorld_VoxelItem.h"
#include "GA_MCWorld_PlaceVoxel.h"
#include "GA_MCWorld_RemoveVoxel.h"

#include "GASystemComponent.h"

#include "Gameframework/Character.h"

#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AMCWorld_BasicInteractionTool::AMCWorld_BasicInteractionTool()
	: AGAWeapon()
{
	// Need to move these into an AbilitySet
	/*CurrentActiveAbilities[0] = UGA_MCWorld_RemoveVoxel::StaticClass();
	CurrentActiveAbilities[1] = UGA_MCWorld_PlaceVoxel::StaticClass();*/

	QuickVoxelInventory.SetNumUninitialized(MaxColumns);
	StowedVoxelInventory.SetNumUninitialized(MaxColumns * (MaxRows - 1));

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> M(TEXT("/Game/GameplayAbilities/Environment/Minecraft/Material/MI_VoxelTargeting"));
	if (M.Succeeded())
	{
		VoxelTargetingMaterial = M.Object;
	}

	PrimaryActorTick.bCanEverTick = true;
}

void AMCWorld_BasicInteractionTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AbilityTargeting())
	{
		if (AbilityTargetingActor)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Purple, "Targeting is updating!");
			AbilityTargetingActor->SetActorLocation(Location);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Purple, "Targeting is being created!");

			FTransform Transform;
			AMCWorld_VoxelItem* SpawnedActor = GetWorld()->SpawnActorDeferred<AMCWorld_VoxelItem>(AMCWorld_VoxelItem::StaticClass(), Transform);
			if (SpawnedActor)
			{
				Transform = FTransform(FRotator(), Location, FVector());
				SpawnedActor->bMini = false;
				SpawnedActor->VoxelSize = 201;
				SpawnedActor->Material = VoxelTargetingMaterial;

				SpawnedActor->FinishSpawning(Transform);

				AbilityTargetingActor = SpawnedActor;

				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Purple, "Targeting was created!");
			}
		}
	}
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Purple, "Targeting is not updating!");
	}
}

void AMCWorld_BasicInteractionTool::SetCurrentItem(AGAActor* NewItem)
{
	CurrentItem = NewItem;
}

bool AMCWorld_BasicInteractionTool::AbilityTargeting()
{
	bool bShouldUpdate = false;
	FVector NewLocation = FVector();

	float Range = 500.f;
	FHitResult Hit;
	FVector CameraLocation = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation();
	FVector CameraForwardVector = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetActorForwardVector();

	FVector Start = CameraLocation;
	FVector End = Start + (CameraForwardVector * Range);
	FCollisionQueryParams CollisionParems;

	DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 1, 0, 1);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParems))
	{
		if (Hit.bBlockingHit)
		{
			DrawDebugPoint(GetWorld(), Hit.Location, 10, FColor::Magenta, true, 1);

			AMCWorld_Voxel* HitVoxel = Cast<AMCWorld_Voxel>(Hit.GetActor());
			if (HitVoxel)
			{
				ActiveChunk = HitVoxel;

				FVector Direction =
					(Hit.Location - CameraLocation).GetSafeNormal();

				FVector Vector2 = Hit.Location + (Direction * 1);

				FVector TempVector = Vector2; // / (ActiveChunk->ChunkLineElementsP2);

				NewLocation =
					FVector((int)(TempVector.X), (int)(TempVector.Y), (int)(TempVector.Z));

				if (Location != NewLocation)
				{
					bShouldUpdate = true;
					Location = NewLocation;
				}

				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, Location.ToString());
			}
		}
	}


	return bShouldUpdate;
}

void AMCWorld_BasicInteractionTool::UseThrow_Implementation()
{

}

void AMCWorld_BasicInteractionTool::UseUp_Implementation(float Val)
{
	if (Val > 0)
	{
		VoxelItemUp();
	}
	else if (Val < 0)
	{
		VoxelItemDown();
	}
}

void AMCWorld_BasicInteractionTool::UseRight_Implementation(float Val)
{
	if (Val > 0)
	{
		VoxelItemRight();
	}
	else if (Val < 0)
	{
		VoxelItemLeft();
	}
}

void AMCWorld_BasicInteractionTool::OpenVoxelMenu()
{
	for (int a = 0; a < QuickVoxelInventory.Num(); a++)
	{
		for (int i = 0; i < StowedVoxelInventory.Num(); i++)
		{
			int32 Index = a + i * (QuickVoxelInventory.Num() - 1);
			StowedVoxelInventory[Index]->SetActorHiddenInGame(false);

			int32 HalfA = QuickVoxelInventory.Num() / 2;
			int32 HalfI = StowedVoxelInventory.Num() / 2;
			FVector Loc = GetActorLocation() + FVector(200, a + (a * 20) - HalfA, i + (i * 20) - HalfI);

			StowedVoxelInventory[Index]->SetActorLocation(Loc);
		}
	}
}

void AMCWorld_BasicInteractionTool::CloseVoxelMenu()
{
	for (AGAActor* Item : StowedVoxelInventory)
	{
		Item->SetActorHiddenInGame(true);
	}
}

void AMCWorld_BasicInteractionTool::VoxelItemUp()
{
	if (CurrentSlot.Y == 0)
	{
		OpenVoxelMenu();
		CurrentSlot.Y += 1;
	}
	else if (CurrentSlot.Y == MaxRows)
	{
		CurrentSlot.Y = 0;
	}
	else
	{
		CurrentSlot.Y += 1;
	}
}

void AMCWorld_BasicInteractionTool::VoxelItemRight()
{
	if (CurrentSlot.X == QuickVoxelInventory.Num() - 1)
	{
		CurrentSlot.X = 0;
	}
	else
	{
		CurrentSlot.X += 1;
	}
}

void AMCWorld_BasicInteractionTool::VoxelItemLeft()
{
	if (CurrentSlot.X == 0)
	{
		CurrentSlot.X = QuickVoxelInventory.Num() - 1;
	}
	else
	{
		CurrentSlot.X -= 1;
	}
}

void AMCWorld_BasicInteractionTool::VoxelItemDown()
{
	if (CurrentSlot.Y == 1) 
	{ 
		CloseVoxelMenu(); 
		CurrentSlot.Y -= 1;
	}
	else if (CurrentSlot.Y == 0) 
	{ 
		CurrentSlot.Y = MaxRows; 
	}
	else 
	{ 
		CurrentSlot.Y -= 1; 
	}
	
}

