// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "JsonAssetFactory.h"
#include "JsonAsset.h"


UJsonAssetFactory::UJsonAssetFactory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedClass = UJsonAsset::StaticClass();
    Formats.Add(TEXT("json;Java Script Object Notation"));

    bCreateNew = true;
    bText = false;
    bEditorImport = true;
    bEditAfterNew = false;
}

UObject* UJsonAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    check(Class->IsChildOf(UJsonAsset::StaticClass()));

    UJsonAsset* NewJsonAsset = NewObject<UJsonAsset>(InParent, Class, Name, Flags);

    /*****************************************/
    // Set JsonAsset FileName.
    // Set JsonAsset FilePath.
    // NewJsonAsset->JsonFileName(Name);
    /*****************************************/
    NewJsonAsset->Initialize(Name.ToString());

    return NewJsonAsset;
}

UObject* UJsonAssetFactory::FactoryCreateBinary(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn)
{
    UJsonAsset* NewJsonAsset = NewObject<UJsonAsset>(InParent, InClass, InName, Flags);

    /*

    // Convert file buffer to readable FString
    FString FileString;
    // FString BrokenString = BytesToString(FileBuffer, FileSize);
    auto MyBytesToString = [&](const uint8* In, int32 Count)
    {
        FString Result;
        Result.Empty(Count);

        while (Count)
        {
            int16 Value = *In;

            Result += TCHAR(Value);

            ++In;
            Count--;
        }
        return Result;
    };
    FileString = MyBytesToString(Buffer, sizeof(BufferEnd));

    /*****************************************
    // Set JsonAsset FileName.
    // Set JsonAsset FilePath.
    // NewJsonAsset->JsonFileName(Name);
    /*****************************************
    NewJsonAsset->Initialize(FileString); // */

    return NewJsonAsset;
}
