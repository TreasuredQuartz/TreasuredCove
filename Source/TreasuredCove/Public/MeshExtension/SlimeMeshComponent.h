// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ProceduralMeshComponent.h"
#include "SlimeMeshComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API USlimeMeshComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	// Vertices
	TArray<FVector> SlimeVertices;

public:	
	// Sets default values for this component's properties
	USlimeMeshComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	/* Components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "false"))
	UProceduralMeshComponent* Mesh;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
