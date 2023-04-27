// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MultiplayerLoadoutManager.generated.h"

class UMultiplayerLoadout;

/**
 * 
 */
UCLASS(BlueprintType)
class TREASUREDCOVE_API UMultiplayerLoadoutManager : public UObject
{
	GENERATED_BODY()
	
public:
	UMultiplayerLoadoutManager();

public:
	UMultiplayerLoadout* Loadout;

	UFUNCTION(BlueprintCallable, Category = "Loadout")
	void SetCurrentLoadout(UMultiplayerLoadout* InLoadout);

	UFUNCTION(BlueprintPure, Category = "Loadout")
	FText GetLoadoutName();
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	void SetLoadoutName(FText InName);

	UFUNCTION(BlueprintPure, Category = "Loadout")
	FName GetPrimary();
	UFUNCTION(BlueprintPure, Category = "Loadout")
	FName GetSecondary();
	UFUNCTION(BlueprintPure, Category = "Loadout")
	TArray<FName> GetPrimaryAttachments();
	UFUNCTION(BlueprintPure, Category = "Loadout")
	TArray<FName> GetSecondaryAttachments();
	UFUNCTION(BlueprintPure, Category = "Loadout")
	TArray<FName> GetPerks();
	UFUNCTION(BlueprintPure, Category = "Loadout")
	TArray<FName> GetStrikePackages();

	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddPrimary(FName InPrimary, int32 InCost);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddPrimaryAttachment(FName InPrimaryAttachment, int32 InCost);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddSecondary(FName InSecondary, int32 InCost);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddSecondaryAttachment(FName InSecondaryAttachment, int32 InCost);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddLethal(FName InLethal, int32 InCost);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddTactical(FName InTactical, int32 InCost);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddPerk(FName InPerk, int32 InCost);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddStrikePackage(FName InStrikePackage, int32 InCost);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddWildcard(FName InWildCard, int32 InCost);

	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemovePrimary(int32 InCost);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemovePrimaryAttachment(FName InPrimaryAttachment, int32 InCost);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemoveSecondary(int32 InCost);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemoveSecondaryAttachment(FName InSecondaryAttachment, int32 InCost);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemoveLethal(FName InLethal, int32 InCost);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemoveTactical(FName InTactical, int32 InCost);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemovePerk(FName InPerk, int32 InCost);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemoveStrikePackage(FName InStrikePackage, int32 InCost);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemoveWildcard(FName InWildCard, int32 InCost);
};

// 9mm $0.21 - $0.23 a round
// 5.56 $1 a round
// .50 $3 a round
// 
// lethals and tacticals cost $50
// 250 9mm rounds cost $50
// 50 5.56 rounds cost $50
// 17 .50 rounds cost $50
