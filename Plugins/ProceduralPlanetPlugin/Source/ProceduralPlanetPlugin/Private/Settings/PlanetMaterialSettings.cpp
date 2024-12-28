// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/PlanetMaterialSettings.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"

FPlanetMaterialSettings::FPlanetMaterialSettings(UMaterialInterface* inMaterial) : Material_Parent(inMaterial)
{
}

void FPlanetMaterialSettings::UpdateMaterial(UObject* Outer)
{
	if (Material == nullptr)
		Material = UMaterialInstanceDynamic::Create(Material_Parent, Outer);
}