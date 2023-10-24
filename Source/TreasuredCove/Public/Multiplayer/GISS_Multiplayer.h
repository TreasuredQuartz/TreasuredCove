// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BlueprintDataDefinitions.h"
#include "GameFramework/SaveGame.h"
#include "GISS_Multiplayer.generated.h"

class USaveGame;
class AMultiplayerLoadout;
class UMultiplayerLoadoutManager;

USTRUCT(BlueprintType)
struct FAccountAttributes
{
	GENERATED_BODY()
public:
	FAccountAttributes();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
	FText ClanName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
	int32 Experience;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
	int32 Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
	int32 Prestige;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
	UMaterialInterface* RankIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
	UMaterialInterface* Background;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
	UMaterialInterface* Portrait;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
	UMaterialInterface* EmblemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
	TArray<UMaterialInterface*> EquippedCallingCards;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
	TArray<UMultiplayerLoadout*> Loadouts;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
	TArray<UObject*> Perks;
};

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UGISS_Multiplayer : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	USaveGame* SaveGame;

private:
	UPROPERTY()
	FAccountAttributes AccountAttributes;

	UPROPERTY()
	int32 CurrentLoadout;

	UPROPERTY()
	UMultiplayerLoadoutManager* LoadoutManager;

public:
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void InitializeUserAccountAttributes(UPARAM(ref)FBPUserOnlineAccount& AccountInfo);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void GetUserAccountAttributes(FAccountAttributes& Attributes);

	UFUNCTION(BlueprintPure, Category = "Multiplayer")
	void GetDefaultLoadouts(TArray<UMultiplayerLoadout*>& OutDefaultLoadouts);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void SetDefaultLoadouts(UPARAM(ref)TArray<TSubclassOf<UMultiplayerLoadout>>& InDefaultLoadoutClasses);

	UFUNCTION(BlueprintPure, Category = "Multiplayer")
	void GetCustomLoadouts(TArray<UMultiplayerLoadout*>& OutCustomLoadouts);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void SetCustomLoadouts(UPARAM(ref)TArray<UMultiplayerLoadout*>& InCustomLoadouts);

	UFUNCTION(BlueprintCallable)
	void SaveCustomLoadoutChanges();

	UFUNCTION(BlueprintCallable)
	void OnSavedCustomLoadoutChanges(const FString& InString, const int32 InIndex, bool Succeded);

	UFUNCTION(BlueprintCallable)
	void LoadCustomLoadouts(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);

	UFUNCTION(BlueprintPure, Category = "Multiplayer")
	UMultiplayerLoadout* GetCurrentLoadout();

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void SetCurrentLoadout(int32 NewLoadout);

	UFUNCTION(BlueprintPure, Category = "Multiplayer")
	UMultiplayerLoadoutManager* GetLoadoutManager();

public:
};
