// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "AssetTypeActions_JsonDatabase.h"
#include "JsonDatabaseAsset.h"
#include "JsonDatabaseAssetEditor/AssetEditor_JsonDatabase.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_JsonDatabase"

FAssetTypeActions_JsonDatabase::FAssetTypeActions_JsonDatabase(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory)
{
}

FText FAssetTypeActions_JsonDatabase::GetName() const
{
	return LOCTEXT("FJsonDatabaseAssetTypeActionsName", "Json Database");
}

FColor FAssetTypeActions_JsonDatabase::GetTypeColor() const
{
	return FColor(129, 196, 115);
}

UClass* FAssetTypeActions_JsonDatabase::GetSupportedClass() const
{
	return UJsonDatabaseAsset::StaticClass();
}

void FAssetTypeActions_JsonDatabase::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UJsonDatabaseAsset* Graph = Cast<UJsonDatabaseAsset>(*ObjIt))
		{
			TSharedRef<FAssetEditor_JsonDatabase> NewGraphEditor(new FAssetEditor_JsonDatabase());
			NewGraphEditor->InitJsonDatabaseAssetEditor(Mode, EditWithinLevelEditor, Graph);
		}
	}
}

uint32 FAssetTypeActions_JsonDatabase::GetCategories()
{
	return MyAssetCategory;
}

const TArray<FText>& FAssetTypeActions_JsonDatabase::GetSubMenus() const
{
	static const TArray<FText> SubMenus
	{
		LOCTEXT("JsonDatabaseSubMenu", "Json Database")
	};
	return SubMenus;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE