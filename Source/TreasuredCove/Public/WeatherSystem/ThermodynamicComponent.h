// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ThermodynamicComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UThermodynamicComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UThermodynamicComponent();

	// Referred to as temperature int getNumberof` 
	double ThermalEnergy;

	// Resistance per time unit that heat is transfered
	float ThermalResistance;

	// Radiation reflected off
	float radiantReflectivity;
};
