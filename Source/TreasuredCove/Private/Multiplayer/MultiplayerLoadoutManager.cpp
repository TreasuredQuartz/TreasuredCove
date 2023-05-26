// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/MultiplayerLoadoutManager.h"
#include "Multiplayer/MultiplayerLoadout.h"

UMultiplayerLoadoutManager::UMultiplayerLoadoutManager()
{
	Loadout = nullptr;
}

void UMultiplayerLoadoutManager::SetCurrentLoadout(UMultiplayerLoadout* InLoadout)
{
	Loadout = InLoadout;
}

FText UMultiplayerLoadoutManager::GetLoadoutName()
{
	return Loadout->GetName();
}

void UMultiplayerLoadoutManager::SetLoadoutName(FText InName)
{
	Loadout->SetName(InName);
}

int32 UMultiplayerLoadoutManager::GetSpentPoints() 
{
	return Loadout->GetSpentPoints();
}


FLoadoutItemInfo UMultiplayerLoadoutManager::GetPrimary()
{
	return Loadout->GetPrimary();
}

FLoadoutItemInfo UMultiplayerLoadoutManager::GetSecondary()
{
	return Loadout->GetSecondary();
}

TArray<FLoadoutItemInfo> UMultiplayerLoadoutManager::GetPrimaryAttachments()
{
	return Loadout->GetPrimaryAttachments();
}

TArray<FLoadoutItemInfo> UMultiplayerLoadoutManager::GetSecondaryAttachments()
{
	return Loadout->GetSecondaryAttachments();
}

TArray<FLoadoutItemInfo> UMultiplayerLoadoutManager::GetPerks()
{
	return Loadout->GetPerks();
}

TArray<FLoadoutItemInfo> UMultiplayerLoadoutManager::GetStrikePackages()
{
	return Loadout->GetStrikePackages();
}



bool UMultiplayerLoadoutManager::AddPrimary(FLoadoutItemInfo& InPrimary)
{
	if (!Loadout) return false;

	return Loadout->AddPrimary(InPrimary);

	OnSuccess.Broadcast();
}

bool UMultiplayerLoadoutManager::AddPrimaryAttachment(FLoadoutItemInfo& InPrimaryAttachment)
{
	if (!Loadout) return false;

	return Loadout->AddPrimaryAttachment(InPrimaryAttachment);
}

bool UMultiplayerLoadoutManager::AddSecondary(FLoadoutItemInfo& InSecondary)
{
	if (!Loadout) return false;

	return Loadout->AddSecondary(InSecondary);
}

bool UMultiplayerLoadoutManager::AddSecondaryAttachment(FLoadoutItemInfo& InSecondaryAttachment)
{
	if (!Loadout) return false;

	return Loadout->AddSecondaryAttachment(InSecondaryAttachment);
}

bool UMultiplayerLoadoutManager::AddLethal(FLoadoutItemInfo& InLethal)
{
	if (!Loadout) return false;

	return Loadout->AddLethal(InLethal);
}

bool UMultiplayerLoadoutManager::AddTactical(FLoadoutItemInfo& InTactical)
{
	if (!Loadout) return false;

	return Loadout->AddTactical(InTactical);
}

bool UMultiplayerLoadoutManager::AddPerk(FLoadoutItemInfo& InPerk)
{
	if (!Loadout) return false;

	return Loadout->AddPerk(InPerk);
}

bool UMultiplayerLoadoutManager::AddStrikePackage(FLoadoutItemInfo& InStrikePackage)
{
	if (!Loadout) return false;

	return Loadout->AddStrikePackage(InStrikePackage);
}

bool UMultiplayerLoadoutManager::AddWildcard(FLoadoutItemInfo& InWildCard)
{
	if (!Loadout) return false;

	return Loadout->AddWildcard(InWildCard);
}



bool UMultiplayerLoadoutManager::RemovePrimary()
{
	if (!Loadout) return false;

	return Loadout->RemovePrimary();
}

bool UMultiplayerLoadoutManager::RemovePrimaryAttachment(FLoadoutItemInfo& InPrimaryAttachment)
{
	if (!Loadout) return false;

	return Loadout->RemovePrimaryAttachment(InPrimaryAttachment);
}

bool UMultiplayerLoadoutManager::RemoveSecondary()
{
	if (!Loadout) return false;

	return Loadout->RemoveSecondary();
}

bool UMultiplayerLoadoutManager::RemoveSecondaryAttachment(FLoadoutItemInfo& InSecondaryAttachment)
{
	if (!Loadout) return false;

	return Loadout->RemoveSecondaryAttachment(InSecondaryAttachment);
}

bool UMultiplayerLoadoutManager::RemoveLethal(FLoadoutItemInfo& InLethal)
{
	if (!Loadout) return false;

	return Loadout->RemoveLethal(InLethal);
}

bool UMultiplayerLoadoutManager::RemoveTactical(FLoadoutItemInfo& InTactical)
{
	if (!Loadout) return false;

	return Loadout->RemoveTactical(InTactical);
}

bool UMultiplayerLoadoutManager::RemovePerk(FLoadoutItemInfo& InPerk)
{
	if (!Loadout) return false;

	return Loadout->RemovePerk(InPerk);
}

bool UMultiplayerLoadoutManager::RemoveStrikePackage(FLoadoutItemInfo& InStrikePackage)
{
	if (!Loadout) return false;

	return Loadout->RemoveStrikePackage(InStrikePackage);
}

bool UMultiplayerLoadoutManager::RemoveWildcard(FLoadoutItemInfo& InWildCard)
{
	if (!Loadout) return false;

	return Loadout->RemoveWildcard(InWildCard);
}


