// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/GISS_Multiplayer.h"
#include "MultiplayerLoadout.h"
#include "MultiplayerLoadoutManager.h"
#include "AdvancedIdentityLibrary.h"

void UGISS_Multiplayer::InitializeUserAccountAttributes(UPARAM(ref)FBPUserOnlineAccount& AccountInfo)
{
	FString DisplayNameValue;
	FString ClanNameValue;
	FString ExperienceValue;
	FString LevelValue;
	FString PrestigeValue;
	FString RankIcon;

	EBlueprintResultSwitch Result;

	UAdvancedIdentityLibrary::GetUserAccountDisplayName(AccountInfo, DisplayNameValue);
	AccountAttributes.DisplayName = FText::FromString(DisplayNameValue);

	UAdvancedIdentityLibrary::GetUserAccountAttribute(AccountInfo, FString("Experience"), ExperienceValue, Result);
	switch (Result)
	{
	case EBlueprintResultSwitch::OnSuccess:
		AccountAttributes.Experience = FCString::Atoi(*ExperienceValue);
		break;
	case EBlueprintResultSwitch::OnFailure:
		break;
	}

	UAdvancedIdentityLibrary::GetUserAccountAttribute(AccountInfo, FString("Level"), LevelValue, Result);
	switch (Result)
	{
	case EBlueprintResultSwitch::OnSuccess:
		AccountAttributes.Level = FCString::Atoi(*LevelValue);
		break;
	case EBlueprintResultSwitch::OnFailure:
		break;
	}

	UAdvancedIdentityLibrary::GetUserAccountAttribute(AccountInfo, FString("Prestige"), PrestigeValue, Result);
	switch (Result)
	{
	case EBlueprintResultSwitch::OnSuccess:
		AccountAttributes.Prestige = FCString::Atoi(*PrestigeValue);
		break;
	case EBlueprintResultSwitch::OnFailure:
		break;
	}
}

void UGISS_Multiplayer::GetUserAccountAttributes(FAccountAttributes& Attributes)
{
	Attributes = AccountAttributes;
}

void UGISS_Multiplayer::GetDefaultLoadouts(TArray<UMultiplayerLoadout*>& OutDefaultLoadouts)
{
	OutDefaultLoadouts.Reserve(5);

	int32 i = 0;
	for (const auto& Loadout : AccountAttributes.Loadouts)
	{
		if (i > 4) break;

		OutDefaultLoadouts.Add(Loadout);

		++i;
	}
}

void UGISS_Multiplayer::SetDefaultLoadouts(TArray<TSubclassOf<UMultiplayerLoadout>>& InDefaultLoadoutClasses)
{
	AccountAttributes.Loadouts.Reserve(5);

	for (const auto& Loadout : InDefaultLoadoutClasses)
	{
		UMultiplayerLoadout* NewLoadout = NewObject<UMultiplayerLoadout>(this, Loadout);
		AccountAttributes.Loadouts.Add(NewLoadout);
	}
}

void UGISS_Multiplayer::GetCustomLoadouts(TArray<UMultiplayerLoadout*>& OutCustomLoadouts)
{
	OutCustomLoadouts.Reserve(5);

	int32 i = 0;
	for (const auto& Loadout : AccountAttributes.Loadouts)
	{
		if (i <= 4) continue;

		OutCustomLoadouts.Add(Loadout);

		++i;
	}
}

void UGISS_Multiplayer::SetCustomLoadouts(TArray<UMultiplayerLoadout*>& InCustomLoadouts)
{
	AccountAttributes.Loadouts.Reserve(5 + InCustomLoadouts.Num());

	for (const auto& Loadout : InCustomLoadouts)
	{
		AccountAttributes.Loadouts.Add(Loadout);
	}
}

UMultiplayerLoadout* UGISS_Multiplayer::GetCurrentLoadout()
{
	return AccountAttributes.Loadouts[CurrentLoadout];
}

void UGISS_Multiplayer::SetCurrentLoadout(int32 NewLoadout)
{
	if (AccountAttributes.Loadouts.Num() <= 0)
	{
		return;
	}

	CurrentLoadout = FMath::Clamp<int32>(NewLoadout, 0, AccountAttributes.Loadouts.Num() - 1);
	GetLoadoutManager()->SetCurrentLoadout(GetCurrentLoadout());
}

UMultiplayerLoadoutManager* UGISS_Multiplayer::GetLoadoutManager()
{
	if (!LoadoutManager) LoadoutManager = NewObject<UMultiplayerLoadoutManager>();
	return LoadoutManager;
}
