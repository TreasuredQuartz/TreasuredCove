// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "JsonDatabaseAssetFactory.h"
#include "JsonDatabaseAsset.h"

UJsonDatabaseAssetFactory::UJsonDatabaseAssetFactory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedClass = UJsonDatabaseAsset::StaticClass();
    Formats.Add(TEXT("mcaddon;My custom Minecraft Pack extension"));

    bCreateNew = true;
    bText = false;
    bEditorImport = true;
    bEditAfterNew = false;
}

UObject* UJsonDatabaseAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	check(Class->IsChildOf(UJsonDatabaseAsset::StaticClass()));
	return NewObject<UJsonDatabaseAsset>(InParent, Class, Name, Flags);
}

bool UJsonDatabaseAssetFactory::FactoryCanImport(const FString& Filename)
{
    const FString Extension = FPaths::GetExtension(Filename);

    if (Extension == TEXT("mcaddon"))
    {
        return true;
    }

    return false;
} // */

UObject* UJsonDatabaseAssetFactory::FactoryCreateBinary(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn)
{
    UJsonDatabaseAsset* CustomData = NewObject<UJsonDatabaseAsset>(InParent, InClass, InName, Flags);

    // Do whatever initialization you need to do here

    return CustomData;
}
