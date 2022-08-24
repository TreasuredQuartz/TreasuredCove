// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/HexGrid/HexGridManager.h"
#include "HexGrid.h"

// Sets default values
AHexGridManager::AHexGridManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RenderRange = 6;
	VoxelSize = 50;
	ChunkLineElements = 10;
	VoxelClass = AHexGrid::StaticClass();

	bIsActive = false;
}

//
void AHexGridManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

// Called when the game starts or when spawned
void AHexGridManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHexGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHexGridManager::AddChunk()
{
	UE_LOG(LogTemp, Warning, TEXT("Adding Chunk!"));

	// Z
	int32 z = 0;
	int32 LocZ = z + ActiveChunkCoords.Z;
	int32 ZScaled = LocZ * ChunkSize;
	int32 ZCenter = ZScaled + ChunkSizeHalf;

	// Y
	for (int32 y = -RenderRange; y <= RenderRange; ++y)
	{
		int32 LocY = y + ActiveChunkCoords.Y;
		int32 YScaled = LocY * ChunkSize * VerticalOffset;
		int32 YCenter = YScaled + (ChunkSizeHalf * VerticalOffset);

		// X
		for (int32 x = -RenderRange; x <= RenderRange; ++x)
		{
			int32 LocX = x + ActiveChunkCoords.X;
			int32 XScaled = LocX * ChunkSize * OddHorizontalOffset;
			int32 XCenter = XScaled + (ChunkSizeHalf * OddHorizontalOffset);

			FIntVector LocVector = FIntVector(LocX, LocY, LocZ);

			// if chunk is outside play area, don't spawn
			//if (!InPlayArea(XCenter, YCenter)) {
			//	continue;
			//}

			// if Chunk is in player radius space...
			if (CHECK_RADIUS(XCenter, YCenter, ZCenter))
			{
				// And if Chunk does not exist in coords...
				if (!ChunkCoords.Contains(LocVector))
				{
					FVector Location = FVector(XCenter, YCenter, ZCenter);

					UE_LOG(LogTemp, Warning, TEXT("Should Add Voxel Actor!"));
					int32 Distance = FMath::Abs(x) + FMath::Abs(y);
					int32 LOD = 0;
					if (Distance <= 1) LOD = 1;
					else if (Distance <= 3) LOD = 2;
					else LOD = 4;
					AddVoxelActor(Location, LocVector, 1);
				}
				else {
					GEngine->AddOnScreenDebugMessage(3, 10.f, FColor::Red, "Chunk updated!");
					Chunks[ChunkCoords.IndexOfByKey(LocVector)]->UpdateChunk(LocVector);
				}
			}
		}
	}
}
