// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/GISS_Multiplayer.h"
#include "Multiplayer/MultiplayerLoadout.h"
#include "Multiplayer/SGMultiplayerLoadouts.h"
#include "Multiplayer/MultiplayerLoadoutManager.h"

#include "Multiplayer/Playfab/PlayFab_GameInstanceSubsystem.h"

#include "AdvancedIdentityLibrary.h"
#include "kismet/GameplayStatics.h"

FAccountAttributes::FAccountAttributes() :
	DisplayName(FText()),
	ClanName(FText()),
	Title(FText()),
	Experience(0),
	Level(0),
	Prestige(0),
	RankIcon(nullptr),
	Background(nullptr),
	Portrait(nullptr),
	EmblemIcon(nullptr),
	EquippedCallingCards(),
	Loadouts()
{
	Loadouts.SetNumZeroed(5);
	for (int i = 0; i < 5; ++i)
	{
		Loadouts[i] = NewObject<UMultiplayerLoadout>();
	}
};

void UGISS_Multiplayer::InitializeUserAccountAttributes(UPARAM(ref)FBPUserOnlineAccount& AccountInfo)
{
	GetLoadoutManager()->OnSuccess.AddUniqueDynamic(this, &UGISS_Multiplayer::SaveCustomLoadoutChanges);

	// Set up the delegate.
	FAsyncLoadGameFromSlotDelegate LoadoutsLoadedDelegate;
	FAsyncLoadGameFromSlotDelegate SettingsLoadedDelegate;

	UPlayfab_GameInstanceSubsystem* PlayFab = GetGameInstance()->GetSubsystem<UPlayfab_GameInstanceSubsystem>();
	PlayFab->OnAttributesChanged.AddUniqueDynamic(this, &UGISS_Multiplayer::UpdatePlayerOnlineAttributes);
	PlayFab->GetUserLevel(PlayFab->GetAccountAttributes().PlayerID);

	// USomeUObjectClass::LoadGameDelegateFunction is a void function that takes the following parameters: const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData
	LoadoutsLoadedDelegate.BindUObject(this, &UGISS_Multiplayer::LoadCustomLoadouts);
	UGameplayStatics::AsyncLoadGameFromSlot("CustomLoadouts", 0, LoadoutsLoadedDelegate);
	UGameplayStatics::AsyncLoadGameFromSlot("MultiplayerSettings", 0, SettingsLoadedDelegate);
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
	for (int i = 0; i < InDefaultLoadoutClasses.Num(); ++i)
	{
		UMultiplayerLoadout* NewLoadout = NewObject<UMultiplayerLoadout>(this, InDefaultLoadoutClasses[i]);
		AccountAttributes.Loadouts[i] = NewLoadout;
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
	AccountAttributes.Loadouts.Append(InCustomLoadouts);
}



void UGISS_Multiplayer::SaveCustomLoadoutChanges()
{
	if (USGMultiplayerLoadouts* SaveGameInstance = Cast<USGMultiplayerLoadouts>(UGameplayStatics::CreateSaveGameObject(USGMultiplayerLoadouts::StaticClass())))
	{
		// Set up the (optional) delegate.
		FAsyncSaveGameToSlotDelegate SavedDelegate;
		// USomeUObjectClass::SaveGameDelegateFunction is a void function that takes the following parameters: const FString& SlotName, const int32 UserIndex, bool bSuccess
		SavedDelegate.BindUObject(this, &UGISS_Multiplayer::OnSavedCustomLoadoutChanges);

		// Set data on the savegame object.
		TArray<UMultiplayerLoadout*> Loadouts;
		GetCustomLoadouts(Loadouts);
		SaveGameInstance->CustomLoadouts = Loadouts;

		// Start async save process.
		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, "CustomLoadouts", 0, SavedDelegate);
	}
}

void UGISS_Multiplayer::OnSavedCustomLoadoutChanges(const FString& InString, const int32 InIndex, bool Succeded)
{

}

void UGISS_Multiplayer::LoadCustomLoadouts(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
	if (USGMultiplayerLoadouts* Data = Cast<USGMultiplayerLoadouts>(LoadedGameData))
	{
		SetCustomLoadouts(Data->CustomLoadouts);
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



void UGISS_Multiplayer::UpdatePlayerLevel(FString PlayerID)
{
	GetGameInstance()->GetSubsystem<UPlayfab_GameInstanceSubsystem>()->UpdateUserLevel(PlayerID, AccountAttributes.Level, AccountAttributes.Experience);
}

void UGISS_Multiplayer::UpdatePlayerOnlineAttributes(const FPlayFabAccountAttributes& Attributes)
{
	AccountAttributes.Level = Attributes.Level;
	AccountAttributes.Experience = Attributes.XP;
}
