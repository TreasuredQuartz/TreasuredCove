// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// #include "UObject/NoExportTypes.h"
#include "MultiplayerLoadout.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFailure);

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
	FName Primary;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	TArray<FName> PrimaryAttachments;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	FName Secondary;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	TArray<FName> SecondaryAttachments;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	TArray<FName> Lethals;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	TArray<FName> Tacticals;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	TArray<FName> Perks;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	TArray<FName> StrikePackages;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Loadout")
	TArray<FName> WildCards;

public:
	void SetName(FText InName);
	FText GetName();

public:
	bool AddPrimary(FName InPrimary, int32 InCost);
	bool AddPrimaryAttachment(FName InPrimaryAttachment, int32 InCost);
	bool AddSecondary(FName InSecondary, int32 InCost);
	bool AddSecondaryAttachment(FName InSecondaryAttachment, int32 InCost);
	bool AddLethal(FName InLethal, int32 InCost);
	bool AddTactical(FName InTactical, int32 InCost);
	bool AddPerk(FName InPerk, int32 InCost);
	bool AddStrikePackage(FName InStrikePackage, int32 InCost);
	bool AddWildcard(FName InWildCard, int32 InCost);

	bool RemovePrimary(int32 InCost);
	bool RemovePrimaryAttachment(FName InPrimaryAttachment, int32 InCost);
	bool RemoveSecondary(int32 InCost);
	bool RemoveSecondaryAttachment(FName InSecondaryAttachment, int32 InCost);
	bool RemoveLethal(FName InLethal, int32 InCost);
	bool RemoveTactical(FName InTactical, int32 InCost);
	bool RemovePerk(FName InPerk, int32 InCost);
	bool RemoveStrikePackage(FName InStrikePackage, int32 InCost);
	bool RemoveWildcard(FName InWildCard, int32 InCost);

public:
	FName GetPrimary() const;
	TArray<FName> GetPrimaryAttachments() const;
	FName GetSecondary() const;
	TArray<FName> GetSecondaryAttachments() const;
	TArray<FName> GetLethals() const;
	TArray<FName> GetTacticals() const;
	TArray<FName> GetPerks() const;
	TArray<FName> GetStrikePackages() const;
	TArray<FName> GetWildcards() const;


public:
	UPROPERTY(BlueprintAssignable)
	FOnSuccess OnSuccess;
	UPROPERTY(BlueprintAssignable)
	FOnFailure OnFailure;
};
