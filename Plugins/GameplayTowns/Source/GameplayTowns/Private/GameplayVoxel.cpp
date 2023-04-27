// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayVoxel.h"
#include "Components/SceneComponent.h"

AGameplayVoxel::AGameplayVoxel() :
	RandomSeed(0),
	VoxelSize(50),
	ChunkLineElements(32),
	ChunkZElements(32),
	ChunkIndex(FIntVector()),
	CurrentLOD(1)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));

	ChunkLineElementsExt = ChunkLineElements + 2;

	ChunkLineElementsP2 = ChunkLineElements * ChunkLineElements;
	ChunkLineElementsP2Ext = ChunkLineElementsExt * ChunkLineElementsExt;

	ChunkTotalElements = ChunkLineElementsP2Ext * ChunkZElements;
	ChunkHalfElements = ChunkTotalElements / 2;

	VoxelSizeHalf = VoxelSize / 2;
	VoxelSizeQuarter = VoxelSize / 4;
	VoxelSizeBit = VoxelSize / 8;
}

void AGameplayVoxel::Initialize(int32 inRandomSeed, int32 inVoxelSize, int32 inChunkLineElements, FIntVector inChunkIndex)
{
	RandomSeed = inRandomSeed;
	VoxelSize = inVoxelSize;
	ChunkLineElements = inChunkLineElements;
	ChunkIndex = inChunkIndex;

	ChunkLineElementsExt = ChunkLineElements + 2;

	ChunkLineElementsP2 = ChunkLineElements * ChunkLineElements;
	ChunkLineElementsP2Ext = ChunkLineElementsExt * ChunkLineElementsExt;

	ChunkTotalElements = ChunkLineElementsP2Ext * ChunkZElements;
	ChunkHalfElements = ChunkTotalElements / 2;

	VoxelSizeHalf = VoxelSize / 2;
	VoxelSizeQuarter = VoxelSize / 4;
	VoxelSizeBit = VoxelSize / 8;
}

// Mutators //
void AGameplayVoxel::SetRandomSeed(int32 InRandomSeed)
{
	RandomSeed = InRandomSeed;
}

void AGameplayVoxel::SetVoxelSize(int32 InVoxelSize)
{
	VoxelSize = InVoxelSize;
}

void AGameplayVoxel::SetChunkZElements(int32 InChunkZElements)
{
	ChunkZElements = InChunkZElements;
}

void AGameplayVoxel::SetChunkLineElements(int32 InChunkLineElements)
{
	ChunkLineElements = InChunkLineElements;
}

void AGameplayVoxel::SetChunkIndex(const FIntVector& InChunkIndex)
{
	ChunkIndex = InChunkIndex;
}

void AGameplayVoxel::SetCurrentLOD(int32 inCurrentLOD)
{
	CurrentLOD = FMath::Clamp<int32>(inCurrentLOD, 1, inCurrentLOD);
}


int32 AGameplayVoxel::GetVoxelSize()
{
	return VoxelSize;
}

void AGameplayVoxel::GenerateChunk()
{
	BeforeChunkGenerated();

	int32 Index = -1;
	for (int32 z = 0; z < ChunkZElements; ++z)
	for (int32 y = 0; y < ChunkLineElementsExt; ++y)
	for (int32 x = 0; x < ChunkLineElementsExt; ++x)
	{
		ChunkGenerating(FIntVector(x, y, z), ++Index);
	}

	AfterChunkGenerated();
}

void AGameplayVoxel::UpdateChunk(FIntVector inChunkIndex, int32 InCurrentLOD) 
{
	ChunkIndex = inChunkIndex;
	CurrentLOD = InCurrentLOD;
	OnChunkUpdated();
}

void AGameplayVoxel::OnChunkUpdated_Implementation() 
{

}

void AGameplayVoxel::BeforeChunkGenerated()
{

}

void AGameplayVoxel::ChunkGenerating(const FIntVector& CurrentLocation, int32 Index)
{

}

void AGameplayVoxel::AfterChunkGenerated()
{

}

void AGameplayVoxel::EnterVoxel()
{
	
}

void AGameplayVoxel::ExitVoxel()
{

}

