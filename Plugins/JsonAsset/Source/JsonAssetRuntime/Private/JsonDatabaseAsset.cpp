// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "JsonDatabaseAsset.h"
#include "JsonAsset.h"
#include "Json.h"

void UJsonDatabaseAsset::Initialize()
{
	ImportZipFile();
}

void UJsonDatabaseAsset::ImportZipFile()
{
	TArray<FString> Files;
	IFileManager& FileManager = IFileManager::Get();

	FString FullPathDirectory = FPaths::ProjectContentDir() + Directory + "/";
	if (!FileManager.DirectoryExists(*FullPathDirectory))
	{
		UE_LOG(LogTemp, Warning, TEXT("JsonDatabaseAsset Directory {%s} does not exist."), *FullPathDirectory);
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("JsonDatabaseAsset Directory {%s} does exist."), *FullPathDirectory);
	}

	FString FullPathFileName = FullPathDirectory + "*.json";
	FileManager.FindFiles(Files, *FullPathFileName, true, false);

	Assets.Reserve(Files.Num());

	TArray<FString> JsonString;

	FText OutError;
	if (FFileHelper::IsFilenameValidForSaving(*Files[0], OutError))
	{
		UE_LOG(LogTemp, Warning, TEXT("File: %s is safe to save to."), *Files[0]);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: %s"), *OutError.ToString());
	}

	if (!FFileHelper::LoadANSITextFileToStrings(*Files[0], &IFileManager::Get(), JsonString))
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't load file: %s"), *Files[0]);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Loaded file: %s"), *Files[0]);
	}

	for (const FString& File : Files)
	{
		if (!IsNewFile(File)) continue;

		if (!FFileHelper::LoadANSITextFileToStrings(*File, &IFileManager::Get(), JsonString))
		{
			UE_LOG(LogTemp, Warning, TEXT("Couldn't load file: %s"), *File);
		}
		
		UJsonAsset* JsonAsset = NewObject<UJsonAsset>();
		JsonAsset->Initialize(File);
		Assets.Add(JsonAsset);
	}

	RebuildDatabase();
}

void UJsonDatabaseAsset::RebuildDatabase()
{
	for (UJsonAsset* JsonAsset : Assets)
	{
		JsonAsset->RebuildAsset();
	}
}

void UJsonDatabaseAsset::SetCurrentAsset(int32 InCurrentAssetIndex)
{
	CurrentAssetIndex = InCurrentAssetIndex;

	OnEditNewAsset.ExecuteIfBound();
};

void UJsonDatabaseAsset::CreateNewAsset()
{
	UJsonAsset* NewAsset = NewObject<UJsonAsset>();

	if (IsNewFile(NewAsset->GetAssetFileName()) == false)
		return;

	Assets.Add(NewAsset);
	SetCurrentAsset(Assets.Num() - 1);

	OnEditNewAsset.ExecuteIfBound();
}

bool UJsonDatabaseAsset::IsNewFile(FString FileName) const
{
	for (UJsonAsset* Asset : Assets) {
		if (Asset->GetAssetFileName() == FileName) {
			return false;
		}
	}

	return true;
}
