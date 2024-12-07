// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "JsonDatabaseAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class JSONASSETEDITOR_API UJsonDatabaseAssetFactory : public UFactory
{
	GENERATED_BODY()

	UJsonDatabaseAssetFactory(const FObjectInitializer& ObjectInitializer);
	
	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool FactoryCanImport(const FString& Filename) override;
	virtual UObject* FactoryCreateBinary(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn);
	// End of UFactory interface
};
