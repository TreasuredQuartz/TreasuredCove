// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "AssetTypeActions_CharacterClass.h"
#include "CharacterClassMakerAssetEditor/AssetEditor_CharacterClassMaker.h"
#include "CharacterClass.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_CharacterClass"

FAssetTypeActions_CharacterClass::FAssetTypeActions_CharacterClass(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory)
{
}

FText FAssetTypeActions_CharacterClass::GetName() const
{
	return LOCTEXT("FCharacterClassAssetTypeActionsName", "Character Class");
}

FColor FAssetTypeActions_CharacterClass::GetTypeColor() const
{
	return FColor(129, 129, 169);
}

UClass* FAssetTypeActions_CharacterClass::GetSupportedClass() const
{
	return UCharacterClass::StaticClass();
}

void FAssetTypeActions_CharacterClass::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UCharacterClass* Graph = Cast<UCharacterClass>(*ObjIt))
		{
			TSharedRef<FAssetEditor_CharacterClassMaker> NewGraphEditor(new FAssetEditor_CharacterClassMaker());
			NewGraphEditor->InitCharacterClassMakerAssetEditor(Mode, EditWithinLevelEditor, Graph);
		}
	}
}

uint32 FAssetTypeActions_CharacterClass::GetCategories()
{
	return MyAssetCategory;
}

const TArray<FText>& FAssetTypeActions_CharacterClass::GetSubMenus() const
{
	static const TArray<FText> SubMenus
	{
		LOCTEXT("CharacterClassSubMenu", "Character Class Maker")
	};
	return SubMenus;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
