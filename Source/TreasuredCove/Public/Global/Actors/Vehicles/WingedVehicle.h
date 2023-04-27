// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAVehicle.h"
#include "WingedVehicle.generated.h"

UCLASS()
class TREASUREDCOVE_API AWingedVehicle : public AGAVehicle
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AWingedVehicle();

private:
	//
	uint8 bLanded:1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
