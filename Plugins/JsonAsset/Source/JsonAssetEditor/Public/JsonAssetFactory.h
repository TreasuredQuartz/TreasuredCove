// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "JsonAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class JSONASSETEDITOR_API UJsonAssetFactory : public UFactory
{
	GENERATED_BODY()
	
public:
	UJsonAssetFactory(const FObjectInitializer& ObjectInitializer);

	UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn);
	UObject* FactoryCreateBinary(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn);
};
