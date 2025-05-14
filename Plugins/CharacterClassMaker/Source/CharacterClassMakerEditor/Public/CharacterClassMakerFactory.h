// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "CharacterClassMakerFactory.generated.h"

/**
 *
 */
UCLASS()
class CHARACTERCLASSMAKEREDITOR_API UCharacterClassMakerFactory : public UFactory
{
	GENERATED_BODY()

	UCharacterClassMakerFactory();
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
