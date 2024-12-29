// Copyright Epic Games, Inc. All Rights Reserved.

#include "JsonAsset.h"

/*	
"format_version": "1.20.30",
	"minecraft:block": {
		"description": {
			"identifier": "bloodnbones:ignatius_block",
			"menu_category": {
				"category": "construction",
				"group": "itemGroup.name.ore"
			}
		},
		"components": {
			"tag:minecraft:metal": {},
			"minecraft:material_instances": {
				"*": {
					"texture": "ignatius_block"
				}
			},
			"minecraft:destructible_by_mining": {
				"seconds_to_destroy": 3.0
			},
			"minecraft:destructible_by_explosion": {
				"explosion_resistance": 1
			},
			"minecraft:geometry": {
				"identifier": "minecraft:geometry.full_block"
			}
		}
	}
*/
void UJsonAsset::Initialize(FString InJsonFile)
{
	JsonFileName = InJsonFile;
	
	RebuildAsset();
}

void UJsonAsset::RebuildAsset()
{
	TArray<FString> JsonStrings;
	if (!GetJsonFileAsStringArray(JsonStrings))
	{
		JsonStrings = JsonFileLines;
	}

	JsonFileObject = MakeShareable(new FJsonObject());
	FArchive JsonArchive = FArchive();
	JsonArchive << JsonStrings;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(&JsonArchive);

	if (FJsonSerializer::Deserialize(JsonReader, JsonFileObject) && JsonFileObject.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Deserialized %s"), *JsonFileName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't deserialize %s"), *JsonFileName);
		return;
	}

	FString FormatVersion = "format_version";
	FString Namespace = "treasured";
	FString ObjectClass = "item";
	FString Description = "description";
	FString Identifier = "identifier";
	FString Components = "components";

	JsonFileObject->GetStringField(*FormatVersion);
	TSharedPtr<FJsonObject> JsonItem = JsonFileObject->GetObjectField(*(Namespace + ":" + ObjectClass));
	if (JsonItem.IsValid())
	{
		AActor* CurrentItem = nullptr;
		TSharedPtr<FJsonObject> JsonItemDescription = JsonItem->GetObjectField(*Description);
		if (JsonItemDescription.IsValid())
		{
			FString ItemIdentifier = JsonItemDescription->GetStringField(*Identifier);

			/* for (FItemDatabaseRow& ItemRow : ItemDatabase)
			{
				if (ItemRow.ItemIdentifier == ItemIdentifier)
				{
					CurrentItem = ItemRow.ItemInstance;
				}
			}

			if (CurrentItem == nullptr)
			{
				CurrentItem = NewObject<AGAActor>(this, *ItemIdentifier);
			} */
		}

		TSharedPtr<FJsonObject> JsonItemComponents = JsonItem->GetObjectField(*Components);
		if (CurrentItem && JsonItemComponents.IsValid())
		{
			// NewItem->InitailizeFromJson(JsonItemComponents);
			// 
			// ||
			// \/

			TArray<FString> ComponentStrings;
			TArray<TSharedPtr<FJsonValue>> ComponentProperties;
			{
				TMap<FString, TSharedPtr<FJsonValue>> ComponentMap = JsonItemComponents->Values;
				ComponentMap.GetKeys(ComponentStrings);
				ComponentMap.GenerateValueArray(ComponentProperties);

				for (FString ComponentKey : ComponentStrings)
				{
					/*TSharedPtr<FJsonObject> ComponentJsonObject = ComponentMap.Find(ComponentKey)->Get()->AsObject();
					TSubclassOf<UActorComponent> ComponentClass = *ItemComponentClasses.Find(ComponentKey);

					UActorComponent* Component = CurrentItem->GetComponentByClass(ComponentClass);
					if (!Component)
					{
						Component = CurrentItem->AddComponentByClass(*ItemComponentClasses.Find(ComponentKey), false, FTransform::Identity, false);
					} */

					// Use a JsonComponentInterface here.
					// Component->InitializeFromJson(ComponentJsonObject);
				}
			}
			// */
		}
	}
}

TArray<FString> UJsonAsset::GetJsonString() const
{
	TArray<FString> JsonString;
	if (GetJsonFileAsStringArray(JsonString))
	{
		return JsonString;
	}

	return JsonFileLines;
}

bool UJsonAsset::GetJsonFileAsStringArray(TArray<FString>& JsonStrings) const
{
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *JsonFileName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't load file: %s"), *JsonFileName);
		JsonStrings = { "{", "", "}" };
		return false;
	}

	JsonString.ParseIntoArrayLines(JsonStrings);
	return true;
}

void UJsonAsset::SetAssetContents(TArray<FString> NewAssetContents)
{
	JsonFileLines = NewAssetContents;
	if (!FFileHelper::SaveStringArrayToFile(JsonFileLines, *JsonFileName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't save file: %s"), *JsonFileName);
	}
}
