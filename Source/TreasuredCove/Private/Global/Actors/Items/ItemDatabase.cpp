// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Actors/Items/ItemDatabase.h"
#include "GAWeapon.h"
// #include "AssetManager.h"
#include "ItemData.h"
#include "Json.h"

FItemDatabaseRow::FItemDatabaseRow()
{
	//// Get the Asset Manager from anywhere
	//if (UAssetManager* Manager = UAssetManager::GetIfValid())
	//{
	//	// Monster Id taken from a DataTable
	//	FPrimaryAssetId MonsterId = SelectedMonsterRow->MonsterId;
	//
	//	// Optional "bundles" like "UI"
	//	TArray<FName> Bundles;
	//
	//	// Locations array from omitted part of code (see github)
	//	FVector SpawnLocation = Locations[0];
	//
	//	// Delegate with parameters we need once the asset has been loaded such as the Id we loaded and the location to spawn at. Will call function 'OnMonsterLoaded' once it's complete.
	//	FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, MonsterId, SpawnLocation);
	//
	//	// The actual async load request
	//	Manager->LoadPrimaryAsset(MonsterId, Bundles, Delegate);
	//}

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
    
}
