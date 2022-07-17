// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlanetMaterialSettings.generated.h"

USTRUCT(BlueprintType)
struct PROCEDURALPLANETPLUGIN_API FPlanetMaterialSettings
{
	GENERATED_BODY()
public:
	FPlanetMaterialSettings() : Material_Parent(nullptr), Material(nullptr) {};
	FPlanetMaterialSettings(UMaterialInterface* inMaterial);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Material", meta=(DisplayName="Material"))
	UMaterialInterface* Material_Parent;
	UMaterialInstanceDynamic* Material;

	void UpdateMaterial(UObject* Outer);

	void UpdateSettings() 
	{
		
	};

	static bool isMemberName(FName Name)
	{
		return (Name == "Material_Parent");
	}
};