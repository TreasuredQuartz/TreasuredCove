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
	// 
	UAnatomicalStructure* Parent;

	TArray<TSubclassOf<UOrgan>> OrganClasses;
	TArray<UOrgan*> Organs;

public:	
	// Sets default values for this component's properties
	UOrganSystem();

	// Called at runtime after all anatomical systems have been created.
	// Override at runtime and treat like a BeginPlay for actors.
	virtual void Initialize(UAnatomicalStructure* InParent = nullptr);
		
	// Getter for organs that this system needs in order to function
	TArray<TSubclassOf<UOrgan>> GetOrganClasses() const;
};
