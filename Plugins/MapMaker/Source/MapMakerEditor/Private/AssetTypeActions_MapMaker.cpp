#include "AssetTypeActions_MapMaker.h"
#include "MapMakerEditorPCH.h"
#include "MapMakerAssetEditor/AssetEditor_MapMaker.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_MapMaker"

FAssetTypeActions_MapMaker::FAssetTypeActions_MapMaker(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory)
{
}

FText FAssetTypeActions_MapMaker::GetName() const
{
	return LOCTEXT("FMapMakerAssetTypeActionsName", "Map Maker");
}

FColor FAssetTypeActions_MapMaker::GetTypeColor() const
{
	return FColor(129, 196, 115);
}

UClass* FAssetTypeActions_MapMaker::GetSupportedClass() const
{
	return UMapMaker::StaticClass();
}

void FAssetTypeActions_MapMaker::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UMapMaker* Graph = Cast<UMapMaker>(*ObjIt))
		{
			TSharedRef<FAssetEditor_MapMaker> NewGraphEditor(new FAssetEditor_MapMaker());
			NewGraphEditor->InitMapMakerAssetEditor(Mode, EditWithinLevelEditor, Graph);
		}
	}
}

uint32 FAssetTypeActions_MapMaker::GetCategories()
{
	return MyAssetCategory;
}

const TArray<FText>& FAssetTypeActions_MapMaker::GetSubMenus() const
{
	static const TArray<FText> SubMenus
	{
		LOCTEXT("MapMakerSubMenu", "Map Maker")
	};
	return SubMenus;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE