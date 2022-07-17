// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "PerformanceMeshComponent.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UPerformanceMeshComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()
	
public:
	// Primary Update function
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
