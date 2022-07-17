// Fill out your copyright notice in the Description page of Project Settings.


#include "MCWorld_Manager.h"
#include "MCWorld_Voxel.h"
#include "MCWorld_BasicInteractionTool.h"

#include "GACharacter.h"

#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "Components/InstancedStaticMeshComponent.h"

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

