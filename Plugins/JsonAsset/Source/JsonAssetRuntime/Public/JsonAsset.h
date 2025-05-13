// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonAsset.generated.h"

/**
*
*/
UCLASS()
class JSONASSETRUNTIME_API UJsonAsset : public UObject
{
	GENERATED_BODY()
public:
	UJsonAsset() : 
		JsonFileName("NewJsonFileName"),
		JsonFilePath(),
		JsonFileLines("{\n\n}"),
		AssetBrush(FSlateBrush())
	{};

private:
	FString JsonFileName;
	FString JsonFilePath;
	FString JsonFileLines;
	FSlateBrush AssetBrush;
	TSharedPtr<FJsonObject> JsonFileObject;

public:
	void Initialize(FString JsonFile);
	void RebuildAsset();

	FString GetJsonString() const;
	TArray<FString> GetJsonStringAsArray() const;
	FString ReadJsonFile();
	const FSlateBrush* GetAssetBrush() const { return &AssetBrush; };
	FString GetAssetFileName() const { return JsonFileName; };

	void SetAssetContents(FString NewAssetContents);
};
