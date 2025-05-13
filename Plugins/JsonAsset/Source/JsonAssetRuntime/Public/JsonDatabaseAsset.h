// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "JsonDatabaseAsset.generated.h"

class UJsonAsset;

DECLARE_MULTICAST_DELEGATE(FOnDatabaseRebuild);
DECLARE_MULTICAST_DELEGATE(FOnDatabaseEditNewAsset);

/**
 * 
 */
UCLASS()
class JSONASSETRUNTIME_API UJsonDatabaseAsset : public UObject
{
	GENERATED_BODY()

private:
	int32 CurrentAssetIndex = -1;
	FString Directory = "JsonFiles";
	TArray<TObjectPtr<UJsonAsset>> Assets;

public:
	void Initialize();
	void ImportZipFile();
	void RebuildDatabase();

	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE TArray<UJsonAsset*> GetAssets() const { return Assets; };
	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE UJsonAsset* GetCurrentAsset() const { return Assets.IsValidIndex(CurrentAssetIndex) ? Assets[CurrentAssetIndex] : nullptr; };

	void NotifyDatabaseChanged() const;
	void SetCurrentAsset(int32 InCurrentAssetIndex);
	void CreateNewAsset();
	bool IsNewFile(FString FileName) const;

public:
	FOnDatabaseRebuild OnChanged;
	FOnDatabaseEditNewAsset OnEditNewAsset;
};
