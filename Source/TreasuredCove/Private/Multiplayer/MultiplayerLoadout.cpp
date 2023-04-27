// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/MultiplayerLoadout.h"

void UMultiplayerLoadout::SetName(FText InName)
{
	Name = InName;
}

FText UMultiplayerLoadout::GetName()
{
	return Name;
}

FName UMultiplayerLoadout::GetPrimary() const
{
	return Primary;
}

TArray<FName> UMultiplayerLoadout::GetPrimaryAttachments() const
{
	return PrimaryAttachments;
}

FName UMultiplayerLoadout::GetSecondary() const
{
	return Secondary;
}

TArray<FName> UMultiplayerLoadout::GetSecondaryAttachments() const
{
	return SecondaryAttachments;
}

TArray<FName> UMultiplayerLoadout::GetLethals() const
{
	return Lethals;
}

TArray<FName> UMultiplayerLoadout::GetTacticals() const
{
	return Tacticals;
}

TArray<FName> UMultiplayerLoadout::GetPerks() const
{
	return Perks;
}

TArray<FName> UMultiplayerLoadout::GetStrikePackages() const
{
	return StrikePackages;
}

TArray<FName> UMultiplayerLoadout::GetWildcards() const
{
	return WildCards;
}




bool UMultiplayerLoadout::AddPrimary(FName InPrimary, int32 InCost)
{
	// We should call RemovePrimary first
	if (!Primary.IsNone()) return false;
	// InPrimary should not be null
	if (InPrimary.IsNone()) return false;

	// Cost should be positive and current points should not go over total points
	if (InCost <= 0 && CurrentPoints + InCost > TotalPoints) return false;
	Primary = InPrimary;
	CurrentPoints += InCost;

	return true;
}

bool UMultiplayerLoadout::AddPrimaryAttachment(FName InPrimaryAttachment, int32 InCost)
{
	//
	if (!InPrimaryAttachment.IsNone()) return false;
	
	// Cost should be positive and current points should not go over total points
	if (InCost <= 0 || CurrentPoints + InCost > TotalPoints) return false;
	CurrentPoints += InCost;

	return true;
}

bool UMultiplayerLoadout::AddSecondary(FName InSecondary, int32 InCost)
{
	// We should call RemoveSecondary first
	if (!Secondary.IsNone()) return false;
	// InSecondary should not be null
	if (InSecondary.IsNone()) return false;

	// Cost should be positive and current points should not go over total points
	if (InCost <= 0 && CurrentPoints + InCost > TotalPoints) return false;
	Secondary = InSecondary;
	CurrentPoints += InCost;

	return true;
}

bool UMultiplayerLoadout::AddSecondaryAttachment(FName InSecondaryAttachment, int32 InCost)
{
	// Cost should be positive and current points should not go over total points
	if (InCost <= 0 || CurrentPoints + InCost > TotalPoints) return false;
	CurrentPoints += InCost;

	return true;
}

bool UMultiplayerLoadout::AddLethal(FName InLethal, int32 InCost)
{
	// Only valid lethals should be passed through
	if (InLethal.IsNone()) return false;
	
	// Cost should be positive and current points should not go over total points
	if (InCost <= 0 && CurrentPoints + InCost > TotalPoints) return false;
	Lethals.Add(InLethal);
	CurrentPoints += InCost;

	return true;
}

bool UMultiplayerLoadout::AddTactical(FName InTactical, int32 InCost)
{
	// Only valid lethals should be passed through
	if (InTactical.IsNone()) return false;

	// Cost should be positive and current points should not go over total points
	if (InCost <= 0 && CurrentPoints + InCost > TotalPoints) return false;
	Tacticals.Add(InTactical);
	CurrentPoints += InCost;

	return true;
}

bool UMultiplayerLoadout::AddPerk(FName InPerk, int32 InCost)
{
	// Only valid lethals should be passed through
	if (InPerk.IsNone()) return false;

	// Cost should be positive and current points should not go over total points
	if (InCost <= 0 && CurrentPoints + InCost > TotalPoints) return false;
	Perks.Add(InPerk);
	CurrentPoints += InCost;

	return true;
}

bool UMultiplayerLoadout::AddStrikePackage(FName InStrikePackage, int32 InCost)
{
	// Only valid strikepackages should be passed through
	if (InStrikePackage.IsNone()) return false;

	// Cost should be positive and current points should not go over total points
	if (InCost <= 0 && CurrentPoints + InCost > TotalPoints) return false;
	StrikePackages.Add(InStrikePackage);
	CurrentPoints += InCost;

	return true;
}

bool UMultiplayerLoadout::AddWildcard(FName InWildCard, int32 InCost)
{
	// Only valid wildcards should be passed through
	if (InWildCard.IsNone()) return false;

	return true;
}



bool UMultiplayerLoadout::RemovePrimary(int32 InCost)
{
	// Primary already removed
	if (!Primary.IsNone()) return false;

	// We should be getting refunded
	if (InCost <= 0) return false;
	
	Primary = FName();
	CurrentPoints = CurrentPoints - InCost >= 0 ? CurrentPoints - InCost : 0;

	return true;
}

bool UMultiplayerLoadout::RemovePrimaryAttachment(FName InPrimaryAttachment, int32 InCost)
{
	// Valid names should be passed
	if (InPrimaryAttachment.IsNone()) return false;

	// Cost should be positive and current points should not go over total points
	if (InCost <= 0 || CurrentPoints - InCost < 0) return false;
	CurrentPoints -= InCost;

	return true;
}

bool UMultiplayerLoadout::RemoveSecondary(int32 InCost)
{
	// Secondary already removed
	if (!Secondary.IsNone()) return false;
	// We should be getting refunded
	if (InCost <= 0) return false;

	Secondary = FName();
	CurrentPoints = CurrentPoints - InCost >= 0 ? CurrentPoints - InCost : 0;

	return true;
}

bool UMultiplayerLoadout::RemoveSecondaryAttachment(FName InSecondaryAttachment, int32 InCost)
{
	// Valid names should be passed
	if (InSecondaryAttachment.IsNone()) return false;

	// Cost should be positive and current points should not go over total points
	if (InCost <= 0 || CurrentPoints - InCost < 0) return false;
	CurrentPoints -= InCost;

	return true;
}

bool UMultiplayerLoadout::RemoveLethal(FName InLethal, int32 InCost)
{
	// There are no lethals to remove or we do not have the lethal equipped
	if (Lethals.IsEmpty() || !Lethals.Contains(InLethal)) return false;
	// We should be getting refunded
	if (InCost <= 0) return false;

	Lethals.RemoveSwap(InLethal);
	CurrentPoints = CurrentPoints - InCost >= 0 ? CurrentPoints - InCost : 0;

	return true;
}

bool UMultiplayerLoadout::RemoveTactical(FName InTactical, int32 InCost)
{
	// There are no Tacticals to remove or we do not have the Tactical equipped
	if (Tacticals.IsEmpty() || !Tacticals.Contains(InTactical)) return false;
	// We should be getting refunded
	if (InCost <= 0) return false;

	Tacticals.RemoveSwap(InTactical);
	CurrentPoints = CurrentPoints - InCost >= 0 ? CurrentPoints - InCost : 0;

	return true;
}

bool UMultiplayerLoadout::RemovePerk(FName InPerk, int32 InCost)
{
	// There are no Perks to remove or we do not have the Perk equipped
	if (Perks.IsEmpty() || !Perks.Contains(InPerk)) return false;
	// We should be getting refunded
	if (InCost <= 0) return false;

	Perks.RemoveSwap(InPerk);
	CurrentPoints = CurrentPoints - InCost >= 0 ? CurrentPoints - InCost : 0;

	return true;
}

bool UMultiplayerLoadout::RemoveStrikePackage(FName InStrikePackage, int32 InCost)
{
	// There are no StrikePackages to remove or we do not have the StrikePackage equipped
	if (StrikePackages.IsEmpty() || !StrikePackages.Contains(InStrikePackage)) return false;
	// We should be getting refunded
	if (InCost <= 0) return false;

	StrikePackages.RemoveSwap(InStrikePackage);
	CurrentPoints = CurrentPoints - InCost >= 0 ? CurrentPoints - InCost : 0;

	return true;
}

bool UMultiplayerLoadout::RemoveWildcard(FName InWildCard, int32 InCost)
{
	// There are no WildCards to remove or we do not have the WildCard equipped
	if (WildCards.IsEmpty() || !StrikePackages.Contains(InWildCard)) return false;
	// We should be getting refunded
	if (InCost <= 0) return false;

	WildCards.RemoveSwap(InWildCard);
	CurrentPoints = CurrentPoints - InCost >= 0 ? CurrentPoints - InCost : 0;

	return true;
}