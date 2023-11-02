// Fill out your copyright notice in the Description page of Project Settings.


#include "Playfab/Playfab_GameInstanceSubsystem.h"
#include "Classes/PlayFabUtilities.h"
#include "PlayFabClientAPI.h"
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
	// UPlayFabUtilities::setPlayFabSettings(GameTitleId);

	UE_LOG(LogTemp, Warning, TEXT("PlayFab: GameInstanceSubSystem Initialized."));
}

void UPlayfab_GameInstanceSubsystem::Deinitialize()
{
}


#pragma region Logins
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

	UE_LOG(LogTemp, Warning, TEXT("PlayFab: \n   Begin Login..."));
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

void UPlayfab_GameInstanceSubsystem::OnLoginSuccess(FClientLoginResult result, UObject* customData)
{
	PlayerId = result.PlayFabId;
	UGameplayStatics::OpenLevel(GetWorld(), FName("Lobby"));
}

void UPlayfab_GameInstanceSubsystem::OnLoginFailure(FPlayFabError error, UObject* customData)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayFab: \n   Error Name: %s\n   Error Message: %s\n   Error Details: %s\n"), *error.ErrorName, *error.ErrorMessage, *error.ErrorDetails);

	FClientLoginWithSteamRequest Request;
	UPlayFabClientAPI::FDelegateOnSuccessLoginWithSteam OnSuccess;
	UPlayFabClientAPI::FDelegateOnFailurePlayFabError OnFailure;

	UPlayFabClientAPI::LoginWithSteam(Request, OnSuccess, OnFailure, nullptr);
}
#pragma endregion


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

void UPlayfab_GameInstanceSubsystem::GetUserLevel(FString UserId)
{
	FClientGetUserDataRequest Request;
	Request.PlayFabId = UserId;
	Request.Keys = "Level";

	UPlayFabClientAPI::FDelegateOnSuccessGetUserData OnSuccess;
	OnSuccess.BindUFunction(this, FName("GetUserLevelOnSuccess"));
	UPlayFabClientAPI::FDelegateOnFailurePlayFabError OnFailure;

	UPlayFabClientAPI::GetUserData(Request, OnSuccess, OnFailure, nullptr);
}

void UPlayfab_GameInstanceSubsystem::GetUserLevelOnSuccess(const FClientGetUserDataResult& result, UObject* customData)
{
	UPlayFabJsonObject* Data = result.Data;
	UPlayFabJsonObject* InnerData = Data->GetField(Data->GetFieldNames()[0])->AsObject();	// Attribute
	InnerData->GetStringField(InnerData->GetFieldNames()[0]);								// Value
}

void UPlayfab_GameInstanceSubsystem::UpdateUserLevel(FString UserId, FString Level, FString XP)
{
	FClientExecuteCloudScriptRequest Request;
	Request.FunctionName = "UpdatePlayerLevel";

	UPlayFabJsonObject* JsonObject = UPlayFabJsonObject::ConstructJsonObject(GetWorld());
	JsonObject->SetField("Level", UPlayFabJsonValue::ConstructJsonValueString(GetWorld(), Level));
	JsonObject->SetField("XP", UPlayFabJsonValue::ConstructJsonValueString(GetWorld(), XP));
	Request.FunctionParameter = JsonObject;

	UPlayFabClientAPI::FDelegateOnSuccessExecuteCloudScript OnSuccess;
	UPlayFabClientAPI::FDelegateOnFailurePlayFabError OnFailure;

	UPlayFabClientAPI::ExecuteCloudScript(Request, OnSuccess, OnFailure, nullptr);
}

// */
