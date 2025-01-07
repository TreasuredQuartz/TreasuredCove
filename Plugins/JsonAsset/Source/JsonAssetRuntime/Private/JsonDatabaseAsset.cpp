// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "JsonDatabaseAsset.h"
#include "JsonAsset.h"
#include "Json.h"
#include "AssetRegistry/AssetRegistryModule.h"

void UJsonDatabaseAsset::Initialize()
{
	RebuildDatabase();
}

void UJsonDatabaseAsset::ImportZipFile()
{
	TArray<FString> Files;
	TArray<FString> JsonFiles;

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	// IFileManager& FileManager = IFileManager::Get();

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

	FString FullPathFileName = FullPathDirectory;
	FString JsonFileExtension = "json";

	TArray<FAssetData> AssetData;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistryModule.Get().GetAssetsByPath(FName(FullPathDirectory), AssetData);
	for (FAssetData Asset : AssetData)
	{
		Asset.AssetClass == UJsonAsset::StaticClass()->GetFName();
		Assets.Add(Cast<UJsonAsset>(Asset.GetAsset()));
	}

	FileManager.FindFiles(JsonFiles, *FullPathFileName, *JsonFileExtension);

	Files.Append(JsonFiles);

	Assets.Reserve(Files.Num());

	/* struct FFileVisitor : public IPlatformFile::FDirectoryVisitor {
		bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
		{
			if (bIsDirectory)
			{

			}
			else
			{

			}
			return true;
		}
	};

	FFileVisitor Visitor;
	FileManager.IterateDirectoryRecursively(*FullPathFileName, Visitor); */

	for (const FString& File : Files)
	{
		if (!IsNewFile(File)) continue;
		
		UJsonAsset* JsonAsset = NewObject<UJsonAsset>();
		JsonAsset->Initialize(File);
		Assets.Add(JsonAsset);
	}
}

void UJsonDatabaseAsset::RebuildDatabase()
{
	Assets.Empty();

	ImportZipFile();

	for (UJsonAsset* JsonAsset : Assets)
	{
		JsonAsset->RebuildAsset();
	}

	NotifyDatabaseChanged();
}

void UJsonDatabaseAsset::NotifyDatabaseChanged() const
{
	OnChanged.Broadcast();
}

void UJsonDatabaseAsset::SetCurrentAsset(int32 InCurrentAssetIndex)
{
	CurrentAssetIndex = InCurrentAssetIndex;

	OnEditNewAsset.Broadcast();
};

void UJsonDatabaseAsset::CreateNewAsset()
{
	UJsonAsset* NewAsset = NewObject<UJsonAsset>();

	// If not a new file, then return.
	if (!IsNewFile(NewAsset->GetAssetFileName()))
		return;

	Assets.Add(NewAsset);
	SetCurrentAsset(Assets.Num() - 1);

	OnEditNewAsset.Broadcast();
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
