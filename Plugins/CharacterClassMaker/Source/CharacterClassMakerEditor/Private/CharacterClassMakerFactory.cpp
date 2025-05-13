// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "CharacterClassMakerFactory.h"
#include "CharacterClass.h"

UCharacterClassMakerFactory::UCharacterClassMakerFactory()
{
	SupportedClass = UCharacterClass::StaticClass();

    bCreateNew = true;
    bText = false;
    bEditorImport = false;
    bEditAfterNew = false;
}

UObject* UCharacterClassMakerFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UCharacterClass>(InParent, Class, Name, Flags | RF_Transactional);;
}
