// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayVoxel.generated.h"

class AGameplayManager;

/** Originally an actor, but difficult to spawn in
* OnConstruction of managers.
*/
UCLASS()
class GAMEPLAYTOWNS_API AGameplayVoxel : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for properties
	AGameplayVoxel();

	virtual void Initialize(int32 inRandomSeed, int32 inVoxelSize, int32 inChunkLineElements, FIntVector inChunkIndex);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Voxel", Meta = (ExposeOnSpawn = true))
	int32 RandomSeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Voxel", Meta = (ExposeOnSpawn = true))
	int32 VoxelSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Voxel", Meta = (ExposeOnSpawn = true))
	int32 ChunkLineElements;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Voxel", Meta = (ExposeOnSpawn = true))
	int32 ChunkZElements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Voxel", Meta = (ExposeOnSpawn = true))
	FIntVector ChunkIndex;

	UPROPERTY(BlueprintReadOnly)
	int32 ChunkTotalElements;
	UPROPERTY(BlueprintReadOnly)
	int32 ChunkLineElementsP2;
	UPROPERTY(BlueprintReadOnly)
	int32 ChunkLineElementsExt;
	UPROPERTY(BlueprintReadOnly)
	int32 ChunkLineElementsP2Ext;
	UPROPERTY(BlueprintReadOnly)
	int32 ChunkHalfElements;
	UPROPERTY(BlueprintReadOnly)
	int32 VoxelSizeHalf;
	UPROPERTY(BlueprintReadOnly)
	int32 VoxelSizeQuarter;
	UPROPERTY(BlueprintReadOnly)
	int32 VoxelSizeBit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay|Voxel", Meta = (ExposeOnSpawn = true))
	AGameplayManager* ManagerRef;

public:	
	void GenerateChunk();
	void UpdateChunk(FIntVector inChunkIndex);

	UFUNCTION(BlueprintNativeEvent, Category = "Gameplay|Voxel")
	void OnChunkUpdated();

	virtual void EnterVoxel();
	virtual void ExitVoxel();

	// Executes right before 3-Dimensional loop. Override in child classes.
	virtual void BeforeChunkGenerated();
	// Executes during 3-Dimensional loop. (Can become Very expensive because of this!!!) Override in child classes.
	virtual void ChunkGenerating(FIntVector CurrentLocation, int32 Index);
	// Executes right after 3-Dimensional loop. Override in child classes.
	virtual void AfterChunkGenerated();
};
