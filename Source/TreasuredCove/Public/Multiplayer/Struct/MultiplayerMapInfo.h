// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Multiplayer/Enum/GamemodeType.h"
#include "MultiplayerMapInfo.generated.h"

USTRUCT(BlueprintType)
struct TREASUREDCOVE_API FMultiplayerMapInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer|Map")
	FText MapName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer|Map")
	FString MapLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer|Map")
	EGamemodeType Gamemode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer|Map")
	UTexture2D* Icon;
	
public:	
	// Sets default values for this actor's properties
	FMultiplayerMapInfo() : MapName(FText()),
		MapLevel(""),
		Gamemode(EGamemodeType::SinglePlayer),
		Icon(nullptr)
	{};

public:
	friend uint32 GetTypeHash(const FMultiplayerMapInfo& Other)
	{
		return GetTypeHash(Other.MapLevel);
	}

public:
	bool operator ==(const FMultiplayerMapInfo& Other) const
	{
		return this->MapName.EqualTo(Other.MapName) &&
			this->Gamemode == Other.Gamemode &&
			this->MapLevel == Other.MapLevel;
	}

	bool operator !=(const FMultiplayerMapInfo& Other) const
	{
		return !this->MapName.EqualTo(Other.MapName) ||
			this->Gamemode != Other.Gamemode ||
			this->MapLevel != Other.MapLevel;
	}
};
