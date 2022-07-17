// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PauseAreaComponent.generated.h"

class AGAActor;
class UProceduralMiniMapComponent;

class USkeletalMeshComponent;
class UInstancedStaticMeshComponent;
class USceneCaptureComponent2D;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UPauseAreaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPauseAreaComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//
	UInstancedStaticMeshComponent* PauseWalls;

	// Skeletal Mesh
	USkeletalMeshComponent* CharacterMesh;

	// Stowed Item interface
	AGAActor* StowedItemInterface;

	// Equipped Item Interface
	AGAActor* EquippedItemInterface;

	// Inventory Interface

	// Scene Capture Component
	USceneCaptureComponent2D* SceneCapture;
	
	// Mini Map
	UProceduralMiniMapComponent* MiniMap;
};
