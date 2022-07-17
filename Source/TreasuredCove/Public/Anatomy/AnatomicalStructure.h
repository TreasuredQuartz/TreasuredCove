// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "AnatomicalStructure.generated.h"

class UOrganSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UAnatomicalStructure : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAnatomicalStructure();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anatomy")
	TArray<TSubclassOf<UOrganSystem>> OrganSystemClasses;
	TArray<UOrganSystem*> OrganSystems;

	// Templated cast to given organ system.
	template<typename T>
	T* GetOrganSystem() const;
};
