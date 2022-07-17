// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetFactory.h"
#include "ProceduralPlanet.h"

UPlanetFactory::UPlanetFactory(const FObjectInitializer& o)
	: Super(o)
{
	SupportedClass = AProceduralPlanet::StaticClass();

	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UPlanetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	check(Class->IsChildOf(AProceduralPlanet::StaticClass()));
	return NewObject<AProceduralPlanet>(InParent, Class, Name, Flags);;
}

bool UPlanetFactory::ShouldShowInNewMenu() const
{
	return true;
}