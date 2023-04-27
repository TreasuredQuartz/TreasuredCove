// Fill out your copyright notice in the Description page of Project Settings.


#include "Playfab/Playfab_GameInstanceSubsystem.h"
/*#include "Classes/PlayFabUtilities.h"
#include "AdvancedSessions.h"

#include "Kismet/GameplayStatics.h"
#include "PlayFabJsonObject.h"
#include "PlayFabJsonValue.h"


UPlayfab_GameInstanceSubsystem::UPlayfab_GameInstanceSubsystem()
{
	GameTitleId = TEXT("DE3E5");
}

void UPlayfab_GameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// Set PlayFab to use our Game Title's session ID
	UPlayFabUtilities::setPlayFabSettings(GameTitleId);
}

void UPlayfab_GameInstanceSubsystem::Deinitialize()
{
}

// Initial login screen uses this function. When expanding to multiple platforms, consider a change to this approach.
void UPlayfab_GameInstanceSubsystem::LoginWithEmail(FString Email, FString Password)
{
	FClientLoginWithEmailAddressRequest Request;
	Request.Email = Email;
	Request.Password = Password;
	UPlayFabClientAPI::FDelegateOnSuccessLoginWithEmailAddress OnSuccess;
	UPlayFabClientAPI::FDelegateOnFailurePlayFabError OnFailure;

	OnSuccess.BindUFunction(this, FName("OnLoginSuccess"));
	OnFailure.BindUFunction(this, FName("OnLoginFailure"));

	UPlayFabClientAPI::LoginWithEmailAddress(Request, OnSuccess, OnFailure, nullptr);
}

void UPlayfab_GameInstanceSubsystem::SignUp(FString Username, FString Email, FString Password)
{
	FClientRegisterPlayFabUserRequest Request;
	Request.DisplayName = Username;
	Request.Email = Email;
	Request.Password = Password;
	Request.Username = Username;
	UPlayFabClientAPI::FDelegateOnSuccessRegisterPlayFabUser OnSuccess;
	UPlayFabClientAPI::FDelegateOnFailurePlayFabError OnFailure;

	OnSuccess.BindUFunction(this, FName("OnRegisterSuccess"));
	OnFailure.BindUFunction(this, FName("OnLoginFailure"));

	UPlayFabClientAPI::RegisterPlayFabUser(Request, OnSuccess, OnFailure, nullptr);
}

void UPlayfab_GameInstanceSubsystem::OnRegisterSuccess(const FClientRegisterPlayFabUserResult& result, UObject* customData)
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("Lobby"), true, FString("listen"));
}

void UPlayfab_GameInstanceSubsystem::OnLoginSuccess(const FClientLoginResult& result, UObject* customData)
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("Lobby"));
}

void UPlayfab_GameInstanceSubsystem::OnLoginFailure(const FPlayFabError& error, UObject* customData)
{
	FClientLoginWithSteamRequest Request;
	UPlayFabClientAPI::FDelegateOnSuccessLoginWithSteam OnSuccess;
	UPlayFabClientAPI::FDelegateOnFailurePlayFabError OnFailure;

	UPlayFabClientAPI::LoginWithSteam(Request, OnSuccess, OnFailure, nullptr);

}

void UPlayfab_GameInstanceSubsystem::IniializeInventory()
{
	FClientGetUserInventoryRequest Request;
	Request.AuthenticationContext;
	Request.CustomTags;
	UPlayFabClientAPI::FDelegateOnSuccessGetUserInventory OnSuccess;
	UPlayFabClientAPI::FDelegateOnFailurePlayFabError OnFailure;

	UPlayFabClientAPI::GetUserInventory(Request, OnSuccess, OnFailure, nullptr);
};

void UPlayfab_GameInstanceSubsystem::GetInventoryOnSuccess(FClientGetUserInventoryResult& Result, UObject* CustomData)
{
	for (const auto& Item : Result.Inventory)
	{
		FName(Item->GetField("ItemId")->AsString());
	}
}

*/
