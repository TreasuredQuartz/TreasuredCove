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
		JsonFileLines({ "{", "", "}" }),
		AssetBrush(FSlateBrush())
	{};

private:
	FString JsonFileName;
	TArray<FString> JsonFileLines;
	FSlateBrush AssetBrush;
	TSharedPtr<FJsonObject> JsonFileObject;

public:
	void Initialize(FString JsonFile);
	void RebuildAsset();

	TArray<FString> GetJsonString() const;
	bool GetJsonFileAsStringArray(TArray<FString>& JsonString) const;
	const FSlateBrush* GetAssetBrush() const { return &AssetBrush; };
	FString GetAssetFileName() const { return JsonFileName; };

	void SetAssetContents(TArray<FString> NewAssetContents);
};
