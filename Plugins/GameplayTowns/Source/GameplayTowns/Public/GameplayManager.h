// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayManager.generated.h"

class APawn;
class APlayerState;
class AGameplayVoxel;
class UBoxComponent;

#define CHECK_RADIUS(x, y, z) FIntVector((x - PlayerCoords.X), (y - PlayerCoords.Y), (z - PlayerCoords.Z)).Size() <= (ChunkSize * RenderRange)

UCLASS()
class GAMEPLAYTOWNS_API AGameplayManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameplayManager();

	// Called in editor and on begin play
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void OnConstruction_DoOnce();

	// Called in editor when a uproperty changes
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* PlayArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Manager|Settings")
	int32 RenderRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Manager|Settings")
	int32 ChunkLineElements;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Manager|Settings")
	int32 ChunkZElements;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Manager|Settings")
	int32 VoxelSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Manager|Settings")
	int32 RandomSeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Manager|Settings")
	bool bIsActive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Manager|Settings")
	bool bUsePlayerPosition = false;
	bool bConstructionDoOnce = false;

public:
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Gameplay|Manager")
	TArray<class APlayerState*> UserArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Manager")
	TSubclassOf<AGameplayVoxel> VoxelClass;
	UPROPERTY(BlueprintReadOnly, Category = "Gameplay|Manager")
	int32 ChunkSize;
	UPROPERTY(BlueprintReadOnly, Category = "Gameplay|Manager")
	int32 ChunkSizeHalf;

	// Player Chunk Coordinate
	UPROPERTY(BlueprintReadOnly, Category = "Gameplay|Manager")
	FIntVector ActiveChunkCoords;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Manager")
	TArray<FIntVector> ChunkCoords;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Manager")
	FIntVector PlayerCoords;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Manager")
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
	virtual AGameplayVoxel* AddVoxelActor(FVector VoxelSpawnLocation, FIntVector inVoxelIndex, int32 CurrentLOD);
	UFUNCTION(BlueprintCallable, Category="Gameplay|Manager")
	virtual void AddChunk();
	void RemoveChunk();
	void ClearChunks();

	UFUNCTION(BlueprintCallable, Category="Gameplay|Manager")
	void UpdatePlayerCoordinate();
	AGameplayVoxel* GetActiveVoxel();

	bool InPlayArea(int32 X, int32 Y);

	UFUNCTION()
	void EnterPlayArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void ExitPlayArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
