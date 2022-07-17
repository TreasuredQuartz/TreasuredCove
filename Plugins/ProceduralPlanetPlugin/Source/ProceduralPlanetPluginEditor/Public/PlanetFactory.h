// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "PlanetFactory.generated.h"

/**
 * 
 */
UCLASS()
class PROCEDURALPLANETPLUGINEDITOR_API UPlanetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	// UFactory interface

	// FactoryCreateNew: create user defined asset object.
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	// CanCreateNew : conditions to create user defined asset.
	virtual bool ShouldShowInNewMenu() const override;

	// End of UFactory interface
};
