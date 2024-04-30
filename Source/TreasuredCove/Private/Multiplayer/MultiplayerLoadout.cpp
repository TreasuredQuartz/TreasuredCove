// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/MultiplayerLoadout.h"
#include "GASkillTreeComponent.h"

UMultiplayerLoadout::UMultiplayerLoadout() 
	/* : Name(FText())				// I have defined default values in the header file, but occasionally get an error:
	, TotalPoints(15)			//- "We are currently retrieving VTable ptr. Please use FVTableHelper constructor instead."
	, CurrentPoints(0)			//- There was no default constructor (My Mistake), but I see no reason to duplicate the defaults.
	, Primary()					//- So here is the default constructor. This can be uncommented if the error persists to 
	, PrimaryAttachments()		//- See if that will fix the error.
	, Secondary()
	, SecondaryAttachments()
	, Lethals()
	, Perks()
	, StrikePackages()
	, WildCards() // */
{
	// PerkComponent = CreateDefaultSubobject<UGASkillTreeComponent>(TEXT("Perk Component"));
}

void UMultiplayerLoadout::SetName(FText InName)
{
	Name = InName;
}

FText UMultiplayerLoadout::GetName()
{
	return Name;
}

void UMultiplayerLoadout::CalculatePoints()
{
	int32 Sum = 0;

	if (Primary.IsValid())
	{
		Sum += Primary.LoadoutCost;
		for (FLoadoutItemInfo& Item : PrimaryAttachments)
		{
			Sum += Item.LoadoutCost;
		}
	}

	if (Secondary.IsValid())
	{
		Sum += Secondary.LoadoutCost;
		for (FLoadoutItemInfo& Item : SecondaryAttachments)
		{
			Sum += Item.LoadoutCost;
		}
	}

	for (FLoadoutItemInfo& Lethal : Lethals)
	{
		Sum += Lethal.LoadoutCost;
	}
	for (FLoadoutItemInfo& Tactical : Tacticals)
	{
		Sum += Tactical.LoadoutCost;
	}
	for (FLoadoutItemInfo& Perk : Perks)
	{
		Sum += Perk.LoadoutCost;
	}
	for (FLoadoutItemInfo& StrikePackage : StrikePackages)
	{
		Sum += StrikePackage.LoadoutCost;
	}
	for (FLoadoutItemInfo& WildCard : WildCards)
	{
		Sum += WildCard.LoadoutCost;
	}

	CurrentPoints = Sum;
}

int32 UMultiplayerLoadout::GetSpentPoints()
{
	return CurrentPoints;
}



FLoadoutItemInfo UMultiplayerLoadout::GetPrimary() const
{
	return Primary;
}

TArray<FLoadoutItemInfo> UMultiplayerLoadout::GetPrimaryAttachments() const
{
	return PrimaryAttachments;
}

FLoadoutItemInfo UMultiplayerLoadout::GetSecondary() const
{
	return Secondary;
}

TArray<FLoadoutItemInfo> UMultiplayerLoadout::GetSecondaryAttachments() const
{
	return SecondaryAttachments;
}

TArray<FLoadoutItemInfo> UMultiplayerLoadout::GetLethals() const
{
	return Lethals;
}

TArray<FLoadoutItemInfo> UMultiplayerLoadout::GetTacticals() const
{
	return Tacticals;
}

TArray<FLoadoutItemInfo> UMultiplayerLoadout::GetPerks() const
{
	return Perks;
}

TArray<FLoadoutItemInfo> UMultiplayerLoadout::GetStrikePackages() const
{
	return StrikePackages;
}

TArray<FLoadoutItemInfo> UMultiplayerLoadout::GetWildcards() const
{
	return WildCards;
}



bool UMultiplayerLoadout::AddPrimary(FLoadoutItemInfo& InPrimary)
{
	// We should call RemovePrimary first
	if (!Primary.IsValid()) return false;
	// InPrimary should not be null
	if (InPrimary.IsValid()) return false;

	// Cost should be positive and current points should not go over total points
	if (InPrimary.LoadoutCost <= 0 && CurrentPoints + InPrimary.LoadoutCost > TotalPoints) return false;
	Primary = InPrimary;
	PrimaryAttachments.Empty(6);

	CalculatePoints();

	return true;
}

bool UMultiplayerLoadout::AddPrimaryAttachment(FLoadoutItemInfo& InPrimaryAttachment)
{
	//
	if (!InPrimaryAttachment.IsValid()) return false;
	
	// Cost should be positive and current points should not go over total points
	if (InPrimaryAttachment.LoadoutCost <= 0 || CurrentPoints + InPrimaryAttachment.LoadoutCost > TotalPoints) return false;
	CurrentPoints += InPrimaryAttachment.LoadoutCost;
	PrimaryAttachments.Add(InPrimaryAttachment);

	return true;
}

bool UMultiplayerLoadout::AddSecondary(FLoadoutItemInfo& InSecondary)
{
	// We should call RemoveSecondary first
	if (!Secondary.IsValid()) return false;
	// InSecondary should not be null
	if (InSecondary.IsValid()) return false;

	// Cost should be positive and current points should not go over total points
	if (InSecondary.LoadoutCost <= 0 && CurrentPoints + InSecondary.LoadoutCost > TotalPoints) return false;
	Secondary = InSecondary;
	SecondaryAttachments.Empty(6);

	CalculatePoints();

	return true;
}

bool UMultiplayerLoadout::AddSecondaryAttachment(FLoadoutItemInfo& InSecondaryAttachment)
{
	if (!InSecondaryAttachment.IsValid()) return false;

	// Cost should be positive and current points should not go over total points
	if (InSecondaryAttachment.LoadoutCost <= 0 || CurrentPoints + InSecondaryAttachment.LoadoutCost > TotalPoints) return false;

	SecondaryAttachments.Add(InSecondaryAttachment);
	CurrentPoints += InSecondaryAttachment.LoadoutCost;

	return true;
}

bool UMultiplayerLoadout::AddLethal(FLoadoutItemInfo& InLethal)
{
	// Only valid lethals should be passed through
	if (InLethal.IsValid()) return false;
	
	// Cost should be positive and current points should not go over total points
	if (InLethal.LoadoutCost <= 0 && CurrentPoints + InLethal.LoadoutCost > TotalPoints) return false;
	Lethals.Add(InLethal);
	CurrentPoints += InLethal.LoadoutCost;

	return true;
}

bool UMultiplayerLoadout::AddTactical(FLoadoutItemInfo& InTactical)
{
	// Only valid lethals should be passed through
	if (InTactical.IsValid()) return false;

	// Cost should be positive and current points should not go over total points
	if (InTactical.LoadoutCost <= 0 && CurrentPoints + InTactical.LoadoutCost > TotalPoints) return false;
	Tacticals.Add(InTactical);
	CurrentPoints += InTactical.LoadoutCost;

	return true;
}

bool UMultiplayerLoadout::AddPerk(FLoadoutItemInfo& InPerk)
{
	// Only valid lethals should be passed through
	if (InPerk.IsValid()) return false;

	// Cost should be positive and current points should not go over total points
	if (InPerk.LoadoutCost <= 0 && CurrentPoints + InPerk.LoadoutCost > TotalPoints) return false;
	Perks.Add(InPerk);
	CurrentPoints += InPerk.LoadoutCost;

	return true;
}

bool UMultiplayerLoadout::AddStrikePackage(FLoadoutItemInfo& InStrikePackage)
{
	// Only valid strikepackages should be passed through
	if (InStrikePackage.IsValid()) return false;

	// Cost should be positive and current points should not go over total points
	if (InStrikePackage.LoadoutCost <= 0 && CurrentPoints + InStrikePackage.LoadoutCost > TotalPoints) return false;
	StrikePackages.Add(InStrikePackage);
	CurrentPoints += InStrikePackage.LoadoutCost;

	return true;
}

bool UMultiplayerLoadout::AddWildcard(FLoadoutItemInfo& InWildCard)
{
	// Only valid wildcards should be passed through
	if (InWildCard.IsValid()) return false;

	return true;
}



bool UMultiplayerLoadout::RemovePrimary()
{
	// Primary already removed
	if (!Primary.IsValid()) return false;
	
	Primary.InValidate();
	PrimaryAttachments.Empty(6);

	CalculatePoints();

	return true;
}

bool UMultiplayerLoadout::RemovePrimaryAttachment(FLoadoutItemInfo& InPrimaryAttachment)
{
	// Valid names should be passed
	if (InPrimaryAttachment.IsValid()) return false;

	CurrentPoints -= InPrimaryAttachment.LoadoutCost;
	InPrimaryAttachment.InValidate();

	return true;
}

bool UMultiplayerLoadout::RemoveSecondary()
{
	// Secondary already removed
	if (!Secondary.IsValid()) return false;

	Secondary.InValidate();
	SecondaryAttachments.Empty(6);

	CalculatePoints();

	return true;
}

bool UMultiplayerLoadout::RemoveSecondaryAttachment(FLoadoutItemInfo& InSecondaryAttachment)
{
	// Valid names should be passed
	if (InSecondaryAttachment.IsValid()) return false;

	CurrentPoints -= InSecondaryAttachment.LoadoutCost;
	InSecondaryAttachment.InValidate();

	return true;
}

bool UMultiplayerLoadout::RemoveLethal(FLoadoutItemInfo& InLethal)
{
	// There are no lethals to remove or we do not have the lethal equipped
	if (Lethals.IsEmpty() || !Lethals.Contains(InLethal)) return false;

	CurrentPoints -= InLethal.LoadoutCost;
	InLethal.InValidate();

	return true;
}

bool UMultiplayerLoadout::RemoveTactical(FLoadoutItemInfo& InTactical)
{
	// There are no Tacticals to remove or we do not have the Tactical equipped
	if (Tacticals.IsEmpty() || !Tacticals.Contains(InTactical)) return false;

	CurrentPoints -= InTactical.LoadoutCost;
	InTactical.InValidate();

	return true;
}

bool UMultiplayerLoadout::RemovePerk(FLoadoutItemInfo& InPerk)
{
	// There are no Perks to remove or we do not have the Perk equipped
	if (Perks.IsEmpty() || !Perks.Contains(InPerk)) return false;

	CurrentPoints -= InPerk.LoadoutCost;
	InPerk.InValidate();

	return true;
}

bool UMultiplayerLoadout::RemoveStrikePackage(FLoadoutItemInfo& InStrikePackage)
{
	// There are no StrikePackages to remove or we do not have the StrikePackage equipped
	if (StrikePackages.IsEmpty() || !StrikePackages.Contains(InStrikePackage)) return false;

	CurrentPoints -= InStrikePackage.LoadoutCost;
	InStrikePackage.InValidate();

	return true;
}

bool UMultiplayerLoadout::RemoveWildcard(FLoadoutItemInfo& InWildCard)
{
	// There are no WildCards to remove or we do not have the WildCard equipped
	if (WildCards.IsEmpty() || !WildCards.Contains(InWildCard)) return false;

	CurrentPoints -= InWildCard.LoadoutCost;
	InWildCard.InValidate();

	return true;
}