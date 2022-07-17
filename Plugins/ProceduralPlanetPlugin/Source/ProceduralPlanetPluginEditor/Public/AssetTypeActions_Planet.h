// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

/**
 * 
 */
class PROCEDURALPLANETPLUGINEDITOR_API FAssetTypeActions_Planet : public FAssetTypeActions_Base
{
public:
	// GetName() : return the asset type name shown in editor.
	virtual FText GetName() const override;

	// GetTypeColor() : return the asset type color shown in editor.
	virtual FColor GetTypeColor() const override;

	// GetSupportedClass() : return your asset class (UUserDefinedAsset).
	virtual UClass* GetSupportedClass() const override;

	// OpenAssetEditor() : entry to open asset editor.
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

	// GetCategories() : category shown in editor while creating asset, you can define custom category bitand return.
	virtual uint32 GetCategories() override;
};
