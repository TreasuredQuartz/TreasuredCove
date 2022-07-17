// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"
#include "GameplayJob.generated.h"

class AGameplayJobSite;

UCLASS()
class GAMEPLAYTOWNS_API UGameplayJob : 
	public UObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UGameplayJob();

	TMap<FName, float> RequiredResources;
	TMap<FName, float> GrantedResources;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Towns|Jobs")
	AActor* JobOwner;

	virtual void JobToDo();
};
