// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayManager.generated.h"

class APawn;
class APlayerState;
class AGameplayVoxel;
class UBoxComponent;

#define CHECK_RADIUS(x, y, z) FIntVector((x - PlayerCoords.X), (y - PlayerCoords.Y), (z - PlayerCoords.Z)).Size() < (ChunkSize * RenderRange)

UCLASS()
class GAMEPLAYTOWNS_API AGameplayManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameplayManager();

	// Called in editor and on begin play
	virtual void OnConstruction(const FTransform& Transform) override;

	// Called in editor when a uproperty changes
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* PlayArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 RenderRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 ChunkLineElements;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 ChunkZElements;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 VoxelSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 RandomSeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bIsActive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bUsePlayerPosition = false;

public:
	UPROPERTY(Editanywhere, BlueprintReadWrite)
	TArray<class APlayerState*> UserArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AGameplayVoxel> VoxelClass;
	UPROPERTY(BlueprintReadOnly)
	int32 ChunkSize;
	UPROPERTY(BlueprintReadOnly)
	int32 ChunkSizeHalf;

	// Player Chunk Coordinate
	UPROPERTY(BlueprintReadOnly)
	FIntVector ActiveChunkCoords;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FIntVector> ChunkCoords;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector PlayerCoords;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AGameplayVoxel*> Chunks;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void AddUser(APawn* InPawn);
	virtual void AddUser(APlayerState* InPlayerState);

	// Automatically adds new actor to chunks array
	// if successful.
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Manager")
	void AddVoxelActor(FVector VoxelSpawnLocation, FIntVector inVoxelIndex);
	UFUNCTION(BlueprintCallable, Category="Gameplay|Manager")
	void AddChunk();
	void RemoveChunk();

	UFUNCTION(BlueprintCallable, Category="Gameplay|Manager")
	void UpdatePlayerCoordinate();
	AGameplayVoxel* GetActiveVoxel();

	bool InPlayArea(int32 X, int32 Y);

	UFUNCTION()
	void EnterPlayArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void ExitPlayArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
