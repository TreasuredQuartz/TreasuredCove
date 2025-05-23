// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeons/StarFox/Minecraft/MCWorld_Manager.h"
#include "Dungeons/StarFox/Minecraft/MCWorld_Voxel.h"

// Sets default values
AMCWorld_Manager::AMCWorld_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RenderRange = 6;
	VoxelSize = 50;
	ChunkLineElements = 10;
	VoxelClass = AMCWorld_Voxel::StaticClass();

	bIsActive = false;
}

//
void AMCWorld_Manager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

// Called when the game starts or when spawned
void AMCWorld_Manager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMCWorld_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

