// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LoadoutItemInfo.h"
#include "MultiplayerLoadoutManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFailure);

class UMultiplayerLoadout;
class UGASkillTreeComponent;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UGASkillTreeComponent* PerkComponent;

public:
	UMultiplayerLoadout* Loadout;

	UFUNCTION(BlueprintCallable, Category = "Loadout")
	void SetCurrentLoadout(UMultiplayerLoadout* InLoadout);

	UFUNCTION(BlueprintPure, Category = "Loadout")
	FText GetLoadoutName();
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	void SetLoadoutName(FText InName);
	UFUNCTION(BlueprintPure, Category = "Loadout")
	int32 GetSpentPoints();

	UFUNCTION(BlueprintPure, Category = "Loadout")
	FLoadoutItemInfo GetPrimary();
	UFUNCTION(BlueprintPure, Category = "Loadout")
	FLoadoutItemInfo GetSecondary();
	UFUNCTION(BlueprintPure, Category = "Loadout")
	TArray<FLoadoutItemInfo> GetPrimaryAttachments();
	UFUNCTION(BlueprintPure, Category = "Loadout")
	TArray<FLoadoutItemInfo> GetSecondaryAttachments();
	UFUNCTION(BlueprintPure, Category = "Loadout")
	TArray<FLoadoutItemInfo> GetPerks();
	UFUNCTION(BlueprintPure, Category = "Loadout")
	TArray<FLoadoutItemInfo> GetStrikePackages();

	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddPrimary(UPARAM(Ref) FLoadoutItemInfo& InPrimary);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddPrimaryAttachment(UPARAM(Ref) FLoadoutItemInfo& InPrimaryAttachment);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddSecondary(UPARAM(Ref) FLoadoutItemInfo& InSecondary);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddSecondaryAttachment(UPARAM(Ref) FLoadoutItemInfo& InSecondaryAttachment);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddLethal(UPARAM(Ref) FLoadoutItemInfo& InLethal);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddTactical(UPARAM(Ref) FLoadoutItemInfo& InTactical);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddPerk(UPARAM(Ref) FLoadoutItemInfo& InPerk);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddStrikePackage(UPARAM(Ref) FLoadoutItemInfo& InStrikePackage);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool AddWildcard(UPARAM(Ref) FLoadoutItemInfo& InWildCard);

	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemovePrimary();
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemovePrimaryAttachment(UPARAM(Ref) FLoadoutItemInfo& InPrimaryAttachment);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemoveSecondary();
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemoveSecondaryAttachment(UPARAM(Ref) FLoadoutItemInfo& InSecondaryAttachment);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemoveLethal(UPARAM(Ref) FLoadoutItemInfo& InLethal);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemoveTactical(UPARAM(Ref) FLoadoutItemInfo& InTactical);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemovePerk(UPARAM(Ref) FLoadoutItemInfo& InPerk);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemoveStrikePackage(UPARAM(Ref) FLoadoutItemInfo& InStrikePackage);
	UFUNCTION(BlueprintCallable, Category = "Loadout")
	bool RemoveWildcard(UPARAM(Ref) FLoadoutItemInfo& InWildCard);

public:
	UPROPERTY(BlueprintAssignable)
	FOnSuccess OnSuccess;
	UPROPERTY(BlueprintAssignable)
	FOnFailure OnFailure;
};

// 9mm $0.21 - $0.23 a round
// 5.56 $1 a round
// .50 $3 a round
// 
// lethals and tacticals cost $50
// 250 9mm rounds cost $50
// 50 5.56 rounds cost $50
// 17 .50 rounds cost $50
