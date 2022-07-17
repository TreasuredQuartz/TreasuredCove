// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayManager.h"
#include "MCWorld_Manager.generated.h"

class UStaticMesh;
class UMaterialInterface;

class AMCWorld_Voxel;

UCLASS()
class TREASUREDCOVE_API AMCWorld_Manager
	: public AGameplayManager
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMCWorld_Manager();

	// Called in editor and on begin play
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasInteractionTool;
};
