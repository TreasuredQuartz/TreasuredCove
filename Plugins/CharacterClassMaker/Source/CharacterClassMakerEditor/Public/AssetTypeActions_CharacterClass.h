// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

/**
 * 
 */
class CHARACTERCLASSMAKEREDITOR_API FAssetTypeActions_CharacterClass : public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_CharacterClass(EAssetTypeCategories::Type InAssetCategory);

	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual uint32 GetCategories() override;
	const TArray<FText>& GetSubMenus() const override;

private:
	EAssetTypeCategories::Type MyAssetCategory;
};
