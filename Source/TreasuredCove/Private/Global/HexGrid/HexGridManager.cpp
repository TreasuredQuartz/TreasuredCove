// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/HexGrid/HexGridManager.h"
#include "HexGrid.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

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

	AddChunk();
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

void Subdivide(const FVector3d& v1, const FVector3d& v2, const FVector3d& v3, TArray<FVector3d>& spherePoints, const unsigned int depth) {
	if (depth == 0) {
		spherePoints.Push(v1);
		spherePoints.Push(v2);
		spherePoints.Push(v3);
		return;
	}
	const FVector3d v12 = (v1 + v2).GetUnsafeNormal();
	const FVector3d v23 = (v2 + v3).GetUnsafeNormal();
	const FVector3d v31 = (v3 + v1).GetUnsafeNormal();
	Subdivide(v1, v12, v31, spherePoints, depth - 1);
	Subdivide(v2, v23, v12, spherePoints, depth - 1);
	Subdivide(v3, v31, v23, spherePoints, depth - 1);
	Subdivide(v12, v23, v31, spherePoints, depth - 1);
}

void InitializeSphere(TArray<FVector3d>& spherePoints, const unsigned int depth) {
	const double X = 0.525731112119133606;
	const double Z = 0.850650808352039932;
	const FVector3d vdata[12] = {
		{-X, 0.0, Z}, { X, 0.0, Z }, { -X, 0.0, -Z }, { X, 0.0, -Z },
		{ 0.0, Z, X }, { 0.0, Z, -X }, { 0.0, -Z, X }, { 0.0, -Z, -X },
		{ Z, X, 0.0 }, { -Z, X, 0.0 }, { Z, -X, 0.0 }, { -Z, -X, 0.0 }
	};
	int tindices[20][3] = {
		{0, 4, 1}, { 0, 9, 4 }, { 9, 5, 4 }, { 4, 5, 8 }, { 4, 8, 1 },
		{ 8, 10, 1 }, { 8, 3, 10 }, { 5, 3, 8 }, { 5, 2, 3 }, { 2, 7, 3 },
		{ 7, 10, 3 }, { 7, 6, 10 }, { 7, 11, 6 }, { 11, 0, 6 }, { 0, 1, 6 },
		{ 6, 1, 10 }, { 9, 0, 11 }, { 9, 11, 2 }, { 9, 2, 5 }, { 7, 2, 11 }
	};
	for (int i = 0; i < 20; i++)
		Subdivide(vdata[tindices[i][0]], vdata[tindices[i][1]], vdata[tindices[i][2]], spherePoints, depth);

	/*
	for (uint8 i = 0; i < depth; ++i)
	{
		// Local Declarations
		double x = 0;
		double y = 0;
		double z = 0;
		double yDelta = 0;
		double curRatio = 0;

		// Logic

		y = 1 - 2 * ((i + .5f) / (depth - 1));

		yDelta = FMath::Sqrt(1 - FMath::Square(y));

		curRatio = i * UE_DOUBLE_GOLDEN_RATIO;

		x = FMath::Cos(curRatio) * yDelta;
		z = FMath::Sin(curRatio) * yDelta;

		spherePoints.Add(FVector(x, y, z));
	} // */
}

void AHexGridManager::AddChunk()
{
	UE_LOG(LogTemp, Warning, TEXT("Adding Chunk!"));
	ClearChunks();

	if (RenderRange <= 0 && false)
	{
		AddVoxelActor(FVector(ActiveChunkCoords), ActiveChunkCoords, 1);
		return;
	}

	TArray<FVector> Directions = {
		FVector::UpVector,
		FVector::DownVector,
		FVector::RightVector,
		FVector::LeftVector,
		FVector::ForwardVector,
		FVector::BackwardVector
	};

	TArray<FVector> Sides = {
		Directions[0] +
		Directions[2] +
		Directions[4],

		Directions[0] +
		Directions[3] +
		Directions[4],

		Directions[0] +
		Directions[2] +
		Directions[5],

		Directions[0] +
		Directions[3] +
		Directions[5],

		Directions[1] +
		Directions[2] +
		Directions[4],

		Directions[1] +
		Directions[3] +
		Directions[4],

		Directions[1] +
		Directions[2] +
		Directions[5],

		Directions[1] +
		Directions[3] +
		Directions[5],
	};

	// InitializeSphere(Sides, 0);
	Chunks.Reserve(Sides.Num());

	FIntVector VoxelIndex = ActiveChunkCoords;
	for (const FVector& Side : Sides)
	{
		AHexGrid* Grid = Cast<AHexGrid>(AddVoxelActor(GetActorLocation(), VoxelIndex, 0));
		if (!Grid) continue;
		Grid->NormalVector = Side.GetUnsafeNormal();
		UGameplayStatics::FinishSpawningActor(Grid, FTransform(GetActorLocation()));
		Grid->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));

		++VoxelIndex.X;
	}
}

AGameplayVoxel* AHexGridManager::AddVoxelActor(FVector VoxelSpawnLocation, FIntVector VoxelIndex, int32 CurrentLOD)
{
	if (GetWorld())
	{
		if (AGameplayVoxel* NewVoxel = GetWorld()->SpawnActorDeferred<AGameplayVoxel>(VoxelClass, FTransform(VoxelSpawnLocation)))
		{
			// UE_LOG(LogTemp, Warning, TEXT("Adding Voxel Actor: %s"), *VoxelSpawnLocation.ToString());
			Chunks.Add(NewVoxel);
			ChunkCoords.Add(VoxelIndex);
			NewVoxel->SetRandomSeed(RandomSeed);
			NewVoxel->SetVoxelSize(VoxelSize);
			NewVoxel->SetChunkZElements(ChunkZElements);
			NewVoxel->SetChunkLineElements(ChunkLineElements);
			NewVoxel->SetChunkIndex(VoxelIndex);
			NewVoxel->SetCurrentLOD(CurrentLOD);
			// NewVoxel->GetRootComponent()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
			// NewVoxel->GetRootComponent()->RegisterComponent();
			// NewVoxel->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
			// Register

			return NewVoxel;
		}
	}

	return nullptr;
}

AHexGrid* AHexGridManager::GetGridAtIndex(int32 InIndex)
{
	int32 GridIndex = 0;
	int32 Division = 0;
	int32 Remainder = InIndex;

	float ChunkLineElementsP3 = FMath::Pow(ChunkLineElements,3.f);
	if (Remainder > ChunkLineElementsP3)
		Division = Remainder / ChunkLineElementsP3;

	if (!Chunks.IsValidIndex(Division)) return nullptr;
	return Cast<AHexGrid>(Chunks[Division]);
}
