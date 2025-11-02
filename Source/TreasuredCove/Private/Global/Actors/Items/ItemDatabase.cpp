// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Actors/Items/ItemDatabase.h"
#include "Global/Actors/Items/GAWeapon.h"
#include "Engine/AssetManager.h"
#include "Global/Actors/Items/ItemData.h"
#include "Json.h"

FItemDatabaseRow::FItemDatabaseRow()
{
	

}

UItemData* FItemDatabaseRow::FindItemDataByClass(TSubclassOf<UItemData> ItemDataClass) const
{
	UItemData* FoundData = nullptr;

	if (UClass* TargetClass = ItemDataClass.Get())
	{
		for (UItemData* Data : ItemData)
		{
			if (Data && Data->IsA(TargetClass))
			{
				FoundData = Data;
				break;
			}
		}
	}

	return FoundData;
};

UItemDatabase::UItemDatabase()
{
	Initialize();
}

void UItemDatabase::Initialize()
{
	// Get the Asset Manager from anywhere
	if (UAssetManager* Manager = UAssetManager::GetIfInitialized())
	{
		FPrimaryAssetType PrimaryAssetType = FPrimaryAssetType(FName(""));
		TArray<FAssetData> Assets;

		Manager->GetPrimaryAssetDataList(PrimaryAssetType, Assets);

		for (FAssetData Asset : Assets)
		{
			// Asset.;
		}

		//
		// AssetId

		// Optional "bundles" like "UI"
		TArray<FName> Bundles;

		// Delegate with parameters we need once the asset has been loaded such as the Id we loaded and the location to spawn at. Will call function 'OnMonsterLoaded' once it's complete.
		// FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this);

		// The actual async load request
		// Manager->LoadPrimaryAsset(AssetId, Bundles, Delegate);
	}
}
