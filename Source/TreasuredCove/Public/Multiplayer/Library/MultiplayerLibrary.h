// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Multiplayer/Struct/MultiplayerMapInfo.h"
#include "MultiplayerLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UMultiplayerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal FMultiplayerMapInfo", CompactNodeTitle = "==", Keywords = "== equal"), Category = "Math")
	static bool EqualEqual_FMultiplayerMapInfoFMultiplayerMapInfo(const FMultiplayerMapInfo& MapInfo_01, const FMultiplayerMapInfo& MapInfo_02);
};
