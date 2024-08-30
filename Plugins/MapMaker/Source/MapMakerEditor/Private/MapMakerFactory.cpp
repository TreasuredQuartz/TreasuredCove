// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerFactory.h"
#include "MapMaker.h"

UMapMakerFactory::UMapMakerFactory()
{
	SupportedClass = UMapMaker::StaticClass();
	bCreateNew = true;
}

UObject* UMapMakerFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UMapMaker>(InParent, Class, Name, Flags | RF_Transactional);;
}
