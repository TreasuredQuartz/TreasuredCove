// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Path.generated.h"

class USplineComponent;

UCLASS()
class TREASUREDCOVE_API APath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APath();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USplineComponent* HelpfulSpline;
};
