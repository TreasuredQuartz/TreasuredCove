// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// #include "UObject/NoExportTypes.h"
#include "LoadoutItemInfo.h"
#include "MultiplayerLoadout.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API UMultiplayerLoadout : public UObject
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	FText Name;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	int32 TotalPoints = 15;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	int32 CurrentPoints = 0;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	FLoadoutItemInfo Primary;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	TArray<FLoadoutItemInfo> PrimaryAttachments;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	FLoadoutItemInfo Secondary;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	TArray<FLoadoutItemInfo> SecondaryAttachments;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	TArray<FLoadoutItemInfo> Lethals;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	TArray<FLoadoutItemInfo> Tacticals;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	TArray<FLoadoutItemInfo> Perks;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	TArray<FLoadoutItemInfo> StrikePackages;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	TArray<FLoadoutItemInfo> WildCards;

public:
	void SetName(FText InName);
	FText GetName();
	void CalculatePoints();
	int32 GetSpentPoints();

public:
	bool AddPrimary(FLoadoutItemInfo& InPrimary);
	bool AddPrimaryAttachment(FLoadoutItemInfo& InPrimaryAttachment);
	bool AddSecondary(FLoadoutItemInfo& InSecondary);
	bool AddSecondaryAttachment(FLoadoutItemInfo& InSecondaryAttachment);
	bool AddLethal(FLoadoutItemInfo& InLethal);
	bool AddTactical(FLoadoutItemInfo& InTactical);
	bool AddPerk(FLoadoutItemInfo& InPerk);
	bool AddStrikePackage(FLoadoutItemInfo& InStrikePackage);
	bool AddWildcard(FLoadoutItemInfo& InWildCard);

	bool RemovePrimary();
	bool RemovePrimaryAttachment(FLoadoutItemInfo& InPrimaryAttachment);
	bool RemoveSecondary();
	bool RemoveSecondaryAttachment(FLoadoutItemInfo& InSecondaryAttachment);
	bool RemoveLethal(FLoadoutItemInfo& InLethal);
	bool RemoveTactical(FLoadoutItemInfo& InTactical);
	bool RemovePerk(FLoadoutItemInfo& InPerk);
	bool RemoveStrikePackage(FLoadoutItemInfo& InStrikePackage);
	bool RemoveWildcard(FLoadoutItemInfo& InWildCard);

public:
	FLoadoutItemInfo GetPrimary() const;
	TArray<FLoadoutItemInfo> GetPrimaryAttachments() const;
	FLoadoutItemInfo GetSecondary() const;
	TArray<FLoadoutItemInfo> GetSecondaryAttachments() const;
	TArray<FLoadoutItemInfo> GetLethals() const;
	TArray<FLoadoutItemInfo> GetTacticals() const;
	TArray<FLoadoutItemInfo> GetPerks() const;
	TArray<FLoadoutItemInfo> GetStrikePackages() const;
	TArray<FLoadoutItemInfo> GetWildcards() const;


};
