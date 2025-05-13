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
	InJsonFile.Split("/", &JsonFilePath, &JsonFileName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	
	RebuildAsset();
}

void UJsonAsset::RebuildAsset()
{
	JsonFileLines = ReadJsonFile();

	JsonFileObject = MakeShareable(new FJsonObject());
	FArchive JsonArchive = FArchive();
	JsonArchive << JsonFileLines;
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

FString UJsonAsset::GetJsonString() const
{
	return JsonFileLines;
}

TArray<FString> UJsonAsset::GetJsonStringAsArray() const
{
	TArray<FString> JsonStrings;
	JsonFileLines.ParseIntoArrayLines(JsonStrings);
	return JsonStrings;
}

FString UJsonAsset::ReadJsonFile()
{
	FString FileString;
	FString FullFilePath = JsonFilePath + "/" + JsonFileName;
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	IFileHandle* File = FileManager.OpenRead(*FullFilePath);

	if (File)
	{
		// Size of the file's contents.
		int64 FileSize = File->Size();

		// Buffer made to recieve the file's contents
		uint8* FileBuffer = reinterpret_cast<uint8*>(FMemory::Malloc(FileSize + 1));

		// Reading the file's contents into the premade file buffer.
		if (File->Read(FileBuffer, FileSize))
		{
			// Yippee

			// Convert file buffer to readable FString
			// FString BrokenString = BytesToString(FileBuffer, FileSize);
			auto MyBytesToString = [&](const uint8* In, int32 Count)
			{
				FString Result;
				Result.Empty(Count);

				while (Count)
				{
					int16 Value = *In;

					Result += TCHAR(Value);

					++In;
					Count--;
				}
				return Result;
			};

			FileString = MyBytesToString(FileBuffer, FileSize);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Couldn't read file: %s"), *JsonFileName);
		}

		// Close the file after reading (Succeffully or otherwise)
		delete File;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't load file: %s"), *JsonFileName);
	}

	return FileString;
}

void UJsonAsset::SetAssetContents(FString NewAssetContents)
{
	UE_LOG(LogTemp, Warning, TEXT("Previous Contents: %s"), *JsonFileLines);
	UE_LOG(LogTemp, Warning, TEXT("New File Contents: %s"), *NewAssetContents);
	JsonFileLines = NewAssetContents;
	FString FullFilePath = JsonFilePath + "/" + JsonFileName;
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	IFileHandle* File = FileManager.OpenWrite(*FullFilePath);

	if (File)
	{
		// for (const FString& FileLine : JsonFileLines)
		FString FileLine = JsonFileLines;
		{
			// Actually write our changes to file.
			if (File->Write((const uint8*)TCHAR_TO_ANSI(*FileLine), FileLine.Len()))
			{
				UE_LOG(LogTemp, Warning, TEXT("Successfully wrote to [%s]"), *GetAssetFileName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to write to [%s]"), *GetAssetFileName());
			}
		}

		// Close the file after writing (Succeffully or otherwise)
		delete File;
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't save file: %s"), *JsonFileName);
	}
}
