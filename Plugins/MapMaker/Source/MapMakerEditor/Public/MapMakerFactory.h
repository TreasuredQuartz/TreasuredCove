// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "MapMakerFactory.generated.h"

/**
 * 
 */
UCLASS()
class MAPMAKEREDITOR_API UMapMakerFactory : public UFactory
{
	GENERATED_BODY()

	UMapMakerFactory();
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
