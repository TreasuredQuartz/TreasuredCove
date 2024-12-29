// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/Playfab/Playfab_GameInstanceSubsystem.h"
#include "AdvancedSessions.h"

#include "Core/PlayFabClientAPI.h"

#include "Kismet/GameplayStatics.h"


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
void UPlayfab_GameInstanceSubsystem::SignUp(FString Username, FString Email, FString Password)
{
	using namespace PlayFab;
	using namespace ClientModels;

	clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	FRegisterPlayFabUserRequest Request;
	Request.DisplayName = Username;
	Request.Email = Email;
	Request.Password = Password;
	Request.Username = Username;


	UPlayFabClientAPI::FRegisterPlayFabUserDelegate OnSuccess;
	FPlayFabErrorDelegate OnFailure;

	OnSuccess.BindUFunction(this, FName("OnRegisterSuccess"));
	OnFailure.BindUFunction(this, FName("OnLoginFailure"));

	clientAPI->RegisterPlayFabUser(Request, OnSuccess, OnFailure);
}

void UPlayfab_GameInstanceSubsystem::OnRegisterSuccess(const PlayFab::ClientModels::FRegisterPlayFabUserResult& SuccessResult)
{
	GetAccountAttributes().PlayerID = SuccessResult.PlayFabId;
	UGameplayStatics::OpenLevel(GetWorld(), FName("Lobby"), true, FString("listen"));
}

// Initial login screen uses this function. When expanding to multiple platforms, consider a change to this approach.
void UPlayfab_GameInstanceSubsystem::LoginWithEmail(FString Email, FString Password)
{
	using namespace PlayFab;
	using namespace ClientModels;

	clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	FLoginWithEmailAddressRequest Request;
	Request.Email = Email;
	Request.Password = Password;

	UPlayFabClientAPI::FLoginWithEmailAddressDelegate OnSuccess;
	FPlayFabErrorDelegate OnFailure;

	OnSuccess.BindUObject(this, &UPlayfab_GameInstanceSubsystem::OnLoginSuccess);
	OnFailure.BindUObject(this, &UPlayfab_GameInstanceSubsystem::OnLoginFailure);

	UE_LOG(LogTemp, Warning, TEXT("PlayFab: \n   Begin Login..."));
	clientAPI->LoginWithEmailAddress(Request, OnSuccess, OnFailure);
}

void UPlayfab_GameInstanceSubsystem::OnLoginSuccess(const PlayFab::ClientModels::FLoginResult& SuccessResult)
{
	UE_LOG(LogTemp, Warning, TEXT("   Login Successful! \n   Opening Lobby..."));
	GetAccountAttributes().PlayerID = SuccessResult.PlayFabId;
	UGameplayStatics::OpenLevel(GetWorld(), FName("Lobby"));
}

void UPlayfab_GameInstanceSubsystem::OnLoginFailure(const PlayFab::FPlayFabCppError& ErrorResult) const
{
	UE_LOG(LogTemp, Warning, TEXT("   LoginFailed!\n   Error Name: %s\n   Error Message: %s\n   Error Details:"), *ErrorResult.ErrorName, *ErrorResult.ErrorMessage);
	for (TPair<FString, FString> Pair : ErrorResult.ErrorDetails.Array())
	{
		UE_LOG(LogTemp, Warning, TEXT("   %s: %s"), *Pair.Key, *Pair.Value);
	}
}
#pragma endregion


void UPlayfab_GameInstanceSubsystem::IniializeInventory()
{
	/*FClientGetUserInventoryRequest Request;
	Request.AuthenticationContext;
	Request.CustomTags;
	UPlayFabClientAPI::FDelegateOnSuccessGetUserInventory OnSuccess;
	UPlayFabClientAPI::FDelegateOnFailurePlayFabError OnFailure;

	UPlayFabClientAPI::GetUserInventory(Request, OnSuccess, OnFailure, nullptr);*/
};

void UPlayfab_GameInstanceSubsystem::GetInventoryOnSuccess(FClientGetUserInventoryResult& Result, UObject* CustomData)
{
	// for (const auto& Item : Result.Inventory)
	{
		// FName(Item->GetField("ItemId")->AsString());
	}
}

void UPlayfab_GameInstanceSubsystem::GetUserLevel(FString UserId)
{
	using namespace PlayFab;
	using namespace ClientModels;

	UE_LOG(LogTemp, Warning, TEXT("PlayFab:   GettingUserLevel..."));

	clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	FGetUserDataRequest Request;
	Request.PlayFabId = UserId;
	Request.Keys.Add("Level");
	Request.Keys.Add("XP");

	UPlayFabClientAPI::FGetUserDataDelegate OnSuccess;
	OnSuccess.BindUObject(this, &UPlayfab_GameInstanceSubsystem::GetUserLevelOnSuccess);
	FPlayFabErrorDelegate OnFailure;

	clientAPI->GetUserData(Request, OnSuccess, OnFailure);
}

void UPlayfab_GameInstanceSubsystem::GetUserLevelOnSuccess(const PlayFab::ClientModels::FGetUserDataResult& SuccessResult)
{
	using namespace PlayFab;
	using namespace ClientModels;

	UE_LOG(LogTemp, Warning, TEXT("   Got User Data!\n   Extracting Data..."));


	TMap<FString, FUserDataRecord> Data = SuccessResult.Data;
	{
		TArray<FString> Keys;
		Data.GetKeys(Keys);
		for (const FString& Key : Keys)
		{
			UE_LOG(LogTemp, Warning, TEXT("Key: [%s], Value: [%s]"), *Key, *Data[Key].Value);

			if (Data.Contains("Level")) GetAccountAttributes().Level = FCString::Atoi(*Data["Level"].Value);
			if (Data.Contains("XP")) GetAccountAttributes().XP = FCString::Atoi(*Data["XP"].Value);
		}
	}
}

void UPlayfab_GameInstanceSubsystem::UpdateUserLevel(FString UserId, int32 Level, int32 XP)
{
	using namespace PlayFab;
	using namespace ClientModels;

	clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	FExecuteCloudScriptRequest Request;
	Request.FunctionName = "UpdatePlayerLevel";

	TSharedPtr<FJsonObject> JsonObject = MakeShareable<FJsonObject>(new FJsonObject());
	JsonObject->SetField("Level", MakeShareable<FJsonValueNumber>(new FJsonValueNumber(Level)));
	JsonObject->SetField("XP", MakeShareable<FJsonValueNumber>(new FJsonValueNumber(XP)));

	FJsonKeeper JsonKeeper(JsonObject);
	Request.FunctionParameter = JsonObject;

	UPlayFabClientAPI::FExecuteCloudScriptDelegate OnSuccess;
	FPlayFabErrorDelegate OnFailure;

	// UPlayFabClientAPI::ExecuteCloudScript(Request, OnSuccess, OnFailure, nullptr);
	clientAPI->ExecuteCloudScript(Request, OnSuccess, OnFailure);
}

// */
