// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/ProceduralMiniMap.h"

// const int32 VoxelTriangles[] = { 2, 1, 0, 0, 3, 2 };
const FVector VoxelNormalsTop[] = { FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1) };
const FVector VoxelNormalsBottom[] = { FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1) };
const FVector VoxelNormalsRight[] = { FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1) };
const FVector VoxelNormalsLeft[] = { FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1) };
const FVector VoxelNormalsForward[] = { FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1) };
const FVector VoxelNormalsBackward[] = { FVector(0, 0, -1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1) };

// Sets default values for this component's properties
UProceduralMiniMap::UProceduralMiniMap()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	PMesh =
		CreateDefaultSubobject<UProceduralMeshComponent>(FName("Mini-Map Mesh"));

	TestDirections =
		TArray<FVector>{ FVector(0, 0, 1), FVector(0, 0, -1), 
		FVector(0, 1, 0), FVector(0, -1, 0),
		FVector(1, 0, 0), FVector(-1, 0, 0)};

	MiniMapScale = -2;
	MiniMapElements = 32;
	MiniMapElementsHalf = 16;
	MiniMapElementSize = 0.1;
	RayCastRange = MiniMapElementSize;
}

// Called when the game starts
void UProceduralMiniMap::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

// Called every frame
void UProceduralMiniMap::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	bool bShouldGetPlayerLocation = false;
	AActor* Owner = GetOwner();
	FVector Location = (bShouldGetPlayerLocation && Owner) ? Owner->GetActorLocation() / MiniMapElementSize : GetComponentLocation() / MiniMapElementSize;
	if (Location != MiniMapLocation)
	{
		UpdateLocation(Location);
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

//
void UProceduralMiniMap::UpdateLocation(FVector NewLocation)
{
	MiniMapLocation = NewLocation;

	UpdateMesh();
}

void UProceduralMiniMap::UpdateMesh()
{
	// World Location to MiniMapLocation
	// x / MiniMapElementSize
	//
	// MiniMapLocation to World Location
	// x * MiniMapElementSize
	int face, x, y, z;
	int CHUNK_SIZE = 32;
	TArray<FVector> Vertices, Normals;
	TArray<FVector2D> UVs;
	TArray<int32> Triangles;

	Faces.Empty();

	for (z = -MiniMapElementsHalf; z <= MiniMapElementsHalf; ++z)
	{
		// z * MiniMapElementsP2;
		for (y = -MiniMapElementsHalf; y <= MiniMapElementsHalf; ++y)
		{
			// y * MiniMapElements;
			for (x = -MiniMapElementsHalf; x <= MiniMapElementsHalf; ++x)
			{
				FVector MiniMapElementLocation = MiniMapLocation + FVector(x, y, z);
				for (face = 0; face < 6; ++face)
				{
					// In "face" direction do this...
					FHitResult Hit;
					FVector Start, End, Direction, Current;
					FCollisionQueryParams CollisionParams;

					switch (face)
					{
					case 0:
						Direction = FVector(0, 0, 1);
						break;
					case 1:
						Direction = FVector(0, 0, -1);
						break;
					case 2:
						Direction = FVector(0, 1, 0);
						break;
					case 3:
						Direction = FVector(0, -1, 0);
						break;
					case 4:
						Direction = FVector(1, 0, 0);
						break;
					case 5:
						Direction = FVector(-1, 0, 0);
						break;
					}

					Start = GetComponentLocation();
					Current = Start;
					End = Start + MiniMapSizeHalf * Direction;
					do
					{
						if (GetWorld()->LineTraceSingleByChannel(Hit, Current, End, ECC_Visibility, CollisionParams))
						{
							Current = Hit.Location;
							
							float Size =		ApplyScale(MiniMapSize);
							float SizeHalf =	ApplyScale(MiniMapSizeHalf);
							VertexBuffer.AppendQuad((Direction * SizeHalf) + (Current * MiniMapElementSize),
													(Direction * MiniMapElementSizeHalf) + (Current * MiniMapElementSize),
													(Direction * MiniMapElementSizeHalf) + (Current * MiniMapElementSize),
													(Direction * MiniMapElementSizeHalf) + (Current * MiniMapElementSize), 
													(-Direction));
							continue;
						}

						Current = End;
					} while (Current != End);
				}
			}
		}
	}
	
	//*/

	/* Greedy Meshing
	FVector ChunkPos = MiniMapLocation;
	for (uint8 axis = 0; axis < 3; ++axis)
	{
		int nextAxisLocation, remainingAxisLocation, neighbor, clearMaskIndex, width, height, maskIndex;
		int nextAxis = (axis + 1) % 3;
		int remainingAxis = (axis + 2) % 3;

		TArray<int> axisSlice{ 0,0,0 }, searchSlice{ 0,0,0 };

		TArray<bool> mask;
		mask.SetNum(CHUNK_SIZE * CHUNK_SIZE);

		searchSlice[axis] = 1;

		// Check Each slice of the chunk
		for (axisSlice[axis] = -1; axisSlice[axis] < CHUNK_SIZE;)
		{
			// Compute the mask
			maskIndex = 0;
			for (axisSlice[nextAxis] = 0; axisSlice[nextAxis < CHUNK_SIZE]; ++axisSlice[nextAxis])
			{
				for(axisSlice[remainingAxis] = 0; axisSlice[remainingAxis < CHUNK_SIZE]; ++axisSlice[remainingAxis])
				{
					// CastRay gets the current world and casts a ray from the given position
					bool blockCurrent = 0 <= axisSlice[axis]				? CastRay(FVector(axisSlice[0], axisSlice[1], axisSlice[2])) : true;
					bool blockCompare = axisSlice[axis] < CHUNK_SIZE - 1	? CastRay(FVector(axisSlice[0] + searchSlice[0] + ChunkPos.X,
																							axisSlice[1] + searchSlice[1] + ChunkPos.Y,
																							axisSlice[2] + searchSlice[2] + ChunkPos.Z)) : true;

					// mask is set to true if there is a visible face between two blocks
					mask[maskIndex++] = blockCurrent != blockCompare;
				}
			}

			++axisSlice[axis];

			maskIndex = 0;
			// Generate mesh from mask using lexcographic ordering
			for (remainingAxisLocation = 0; remainingAxisLocation < CHUNK_SIZE; ++remainingAxisLocation)
			{
				for (nextAxisLocation = 0; nextAxisLocation < CHUNK_SIZE;)
				{
					if (mask[maskIndex])
					{
						// Compute width of quad
						for (width = 1; nextAxisLocation + width < CHUNK_SIZE && mask[maskIndex + width]; width++);

						bool done = false;
						// Compute height of quad
						for (height = 1; remainingAxisLocation + height < CHUNK_SIZE; height++)
						{
							for (neighbor = 0; neighbor < width; ++neighbor)
							{
								if (!mask[maskIndex + neighbor + height * CHUNK_SIZE])
								{
									done = true;
									break;
								}
							}
							if (done) break;
						}

						axisSlice[nextAxis] = (nextAxisLocation * MiniMapElementSize);
						axisSlice[remainingAxis] = remainingAxisLocation;

						// Distance traveled on the next axis
						int deltaNextAxis[3];
						deltaNextAxis[nextAxis] = width;

						// Distance traveled on the last axis
						int deltaRemainingAxis[3];
						deltaRemainingAxis[remainingAxis] = height;

						VertexBuffer.AppendQuad(FVector(axisSlice[0],												axisSlice[1],												axisSlice[2]),
												FVector(axisSlice[0] + deltaNextAxis[0],							axisSlice[1] + deltaNextAxis[1],							axisSlice[2] + deltaNextAxis[2]),
												FVector(axisSlice[0] + deltaRemainingAxis[0],						axisSlice[1] + deltaRemainingAxis[1],						axisSlice[2] + deltaRemainingAxis[2]),
												FVector(axisSlice[0] + deltaNextAxis[0] + deltaRemainingAxis[0],	axisSlice[1] + deltaNextAxis[1] + deltaRemainingAxis[1],	axisSlice[2] + deltaNextAxis[2] + deltaRemainingAxis[2]),
												FVector(0,0,1));

						// Clear this part of the mask, so we don't add duplicate faces
						for (clearMaskIndex = 0; clearMaskIndex < height; ++clearMaskIndex)
							for (neighbor = 0; neighbor < width; ++neighbor)
								mask[maskIndex + neighbor + clearMaskIndex * CHUNK_SIZE] = false;

						// Increment our counters before continuing
						nextAxisLocation += width;
						maskIndex += width;
					}
					else
					{
						// mask is false for this index
						// ie: continue
						++nextAxisLocation;
						++maskIndex;
					}
				}
			}
		}
	}//*/

	PMesh->ClearAllMeshSections();

	PMesh->CreateMeshSection(0, VertexBuffer.GetVertices(), VertexBuffer.GetTriangles(), VertexBuffer.GetNormals(), VertexBuffer.GetUVs(), VertexBuffer.GetColors(), VertexBuffer.GetTangents(), VertexBuffer.GetCollision());
	PMesh->SetMaterial(0, VertexBuffer.GetMaterial());
}

float UProceduralMiniMap::ApplyScale(int32 Size)
{
	float Value = 1.f;
	FString Num0s;
	if (MiniMapScale < 0)
	{
		for (int8 scale = 0; scale > MiniMapScale; --scale)
			Num0s.AppendChar(0);
		Value = FCString::Atof(*Num0s.Append(FString::FromInt(Size)));
	}
	else
	{
		for (int8 scale = 0; scale < MiniMapScale; ++scale)
			Num0s.AppendChar(0);
		Value = FCString::Atof(*FString::FromInt(Size).Append(Num0s));
	}
	
	return Value;
}

bool UProceduralMiniMap::CastRay(FHitResult& Hit, const FVector& Start, const FVector& Direction)
{
	FVector End = Start + RayCastRange * Direction;

	return GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, FCollisionQueryParams());
}
