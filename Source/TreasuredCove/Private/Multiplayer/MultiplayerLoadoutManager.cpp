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



FName UMultiplayerLoadoutManager::GetPrimary()
{
	return Loadout->GetPrimary();
}

FName UMultiplayerLoadoutManager::GetSecondary()
{
	return Loadout->GetSecondary();
}

TArray<FName> UMultiplayerLoadoutManager::GetPrimaryAttachments()
{
	return Loadout->GetPrimaryAttachments();
}

TArray<FName> UMultiplayerLoadoutManager::GetSecondaryAttachments()
{
	return Loadout->GetSecondaryAttachments();
}

TArray<FName> UMultiplayerLoadoutManager::GetPerks()
{
	return Loadout->GetPerks();
}

TArray<FName> UMultiplayerLoadoutManager::GetStrikePackages()
{
	return Loadout->GetStrikePackages();
}



bool UMultiplayerLoadoutManager::AddPrimary(FName InPrimary, int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->AddPrimary(InPrimary, InCost);
}

bool UMultiplayerLoadoutManager::AddPrimaryAttachment(FName InPrimaryAttachment, int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->AddPrimaryAttachment(InPrimaryAttachment, InCost);
}

bool UMultiplayerLoadoutManager::AddSecondary(FName InSecondary, int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->AddSecondary(InSecondary, InCost);
}

bool UMultiplayerLoadoutManager::AddSecondaryAttachment(FName InSecondaryAttachment, int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->AddSecondaryAttachment(InSecondaryAttachment, InCost);
}

bool UMultiplayerLoadoutManager::AddLethal(FName InLethal, int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->AddLethal(InLethal, InCost);
}

bool UMultiplayerLoadoutManager::AddTactical(FName InTactical, int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->AddTactical(InTactical, InCost);
}

bool UMultiplayerLoadoutManager::AddPerk(FName InPerk, int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->AddPerk(InPerk, InCost);
}

bool UMultiplayerLoadoutManager::AddStrikePackage(FName InStrikePackage, int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->AddStrikePackage(InStrikePackage, InCost);
}

bool UMultiplayerLoadoutManager::AddWildcard(FName InWildCard, int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->AddWildcard(InWildCard, InCost);
}



bool UMultiplayerLoadoutManager::RemovePrimary(int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->RemovePrimary(InCost);
}

bool UMultiplayerLoadoutManager::RemovePrimaryAttachment(FName InPrimaryAttachment, int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->RemovePrimaryAttachment(InPrimaryAttachment, InCost);
}

bool UMultiplayerLoadoutManager::RemoveSecondary(int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->RemoveSecondary(InCost);
}

bool UMultiplayerLoadoutManager::RemoveSecondaryAttachment(FName InSecondaryAttachment, int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->RemoveSecondaryAttachment(InSecondaryAttachment, InCost);
}

bool UMultiplayerLoadoutManager::RemoveLethal(FName InLethal, int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->RemoveLethal(InLethal, InCost);
}

bool UMultiplayerLoadoutManager::RemoveTactical(FName InTactical, int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->RemoveTactical(InTactical, InCost);
}

bool UMultiplayerLoadoutManager::RemovePerk(FName InPerk, int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->RemovePerk(InPerk, InCost);
}

bool UMultiplayerLoadoutManager::RemoveStrikePackage(FName InStrikePackage, int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->RemoveStrikePackage(InStrikePackage, InCost);
}

bool UMultiplayerLoadoutManager::RemoveWildcard(FName InWildCard, int32 InCost)
{
	if (!Loadout) return false;

	return Loadout->RemoveWildcard(InWildCard, InCost);
}


