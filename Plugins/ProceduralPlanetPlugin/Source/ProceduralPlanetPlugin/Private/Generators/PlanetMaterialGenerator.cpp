// Fill out your copyright notice in the Description page of Project Settings.

#include "Generators/PlanetMaterialGenerator.h"
#include "Materials/MaterialInstanceDynamic.h"

void FPlanetMaterialGenerator::UpdateSettings(FPlanetMaterialSettings inSettings)
{
	Settings = inSettings;
}

void FPlanetMaterialGenerator::UpdateElevation(FMinMax MM)
{
	Settings.Material->SetVectorParameterValue(FName("elevation"), FLinearColor(MM.GetMax(), MM.GetMin(), 0, 0));
}
