// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PlayFab.h"
#include "Core/PlayFabError.h"
#include "Core/PlayFabClientDataModels.h"
// #include "Classes/PlayFabClientAPI.h"
#include "Playfab_GameInstanceSubsystem.generated.h"

namespace PlayFab
{
    struct FPlayFabCppError;

    namespace ClientModels
    {
        struct FRegisterPlayFabUserResult;
        struct FGetUserDataResult;
        struct FLoginResult;
    }
}

USTRUCT(BlueprintType)
struct FPlayFabAccountAttributes
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FString PlayerID;
    UPROPERTY()
    int32 Level;
    UPROPERTY()
    int32 XP;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributesChanged, const FPlayFabAccountAttributes&, NewAttributes);

/**
 * 
 */
UCLASS(BlueprintType)
class TREASUREDCOVE_API UPlayfab_GameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

    
private:
    PlayFabClientPtr clientAPI = nullptr;

    // PlayFabID (Must match corrosponding ID for title in PlayFab catelog)
    FString GameTitleId;
    FPlayFabAccountAttributes PlayerAccountAttributes;

public:
    UPROPERTY(BlueprintAssignable)
    FOnAttributesChanged OnAttributesChanged;

public:
    // Default Constructor
    UPlayfab_GameInstanceSubsystem();

    UFUNCTION(BlueprintPure, BlueprintCallable)
    FString GetPlayerID() const { return PlayerAccountAttributes.PlayerID; };

    FPlayFabAccountAttributes& GetAccountAttributes() { return PlayerAccountAttributes; };

public:
    // Begin USubsystem
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    // End USubsystem

public:
    // Begin PlayFab_GameInstanceSubsystem

    // Primary testing function for PlayFab login
    UFUNCTION(BlueprintCallable)
    void LoginWithEmail(FString Email, FString Password);

    // Simple PlayFab account creation proxy. Called from a login screen
    UFUNCTION(BlueprintCallable)
    void SignUp(FString Username, FString Email, FString Password);

    UFUNCTION()
    void IniializeInventory();

    // Delegate for Playfab inventory success
    UFUNCTION()
    void GetInventoryOnSuccess(FClientGetUserInventoryResult& Result, UObject* CustomData);

public:
    UFUNCTION()
    void GetUserLevel(FString UserId);

    UFUNCTION()
    void UpdateUserLevel(FString UserId, int32 Level, int32 XP);

    // End PlayFab_GameInstanceSubsystem

    // Delegate for account creation success
    void OnRegisterSuccess(const struct PlayFab::ClientModels::FRegisterPlayFabUserResult & result);
    // Delegate for PlayFab login success
    void OnLoginSuccess(const struct PlayFab::ClientModels::FLoginResult & SuccessResult);
    // Delegate for PlayFab login failure
    void OnLoginFailure(const struct PlayFab::FPlayFabCppError& ErrorResult) const;
    // Delegate For PlayFab retrevial of user attributes
    void GetUserLevelOnSuccess(const struct PlayFab::ClientModels::FGetUserDataResult & SuccessResult);
 

    // */
};
