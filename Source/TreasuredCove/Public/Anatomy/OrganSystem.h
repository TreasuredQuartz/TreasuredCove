// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "OrganSystem.generated.h"

class UAnatomicalStructure;
class UOrgan;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UOrganSystem : public UObject
{
	GENERATED_BODY()

private:
	UAnatomicalStructure* Parent;
	TArray<UOrgan*> Organs;

public:	
	// Sets default values for this component's properties
	UOrganSystem();

	// Initialize
	void Initialize(UAnatomicalStructure* InParent = nullptr);
		
};
