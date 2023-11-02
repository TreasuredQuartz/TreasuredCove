// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
// #include "Classes/PlayFabClientAPI.h"
#include "Playfab_GameInstanceSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FPlayFabAccountAttributes
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FString PlayerID;
    UPROPERTY()
    FString Level;
    UPROPERTY()
    FString XP;
};

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UPlayfab_GameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

    
private:
    // PlayFabID (Must match corrosponding ID for title in PlayFab catelog)
    FString GameTitleId;
    FString PlayerId;

public:
    // Default Constructor
    UPlayfab_GameInstanceSubsystem();

    UFUNCTION(BlueprintPure, BlueprintCallable)
    FString GetPlayerID() const { return PlayerId; };

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

    // Delegate for account creation success
    UFUNCTION()
    void OnRegisterSuccess(const FClientRegisterPlayFabUserResult& result, UObject* customData);

    // Delegate for PlayFab login success
    UFUNCTION()
    void OnLoginSuccess(const struct FClientLoginResult result, UObject* customData);

    // Delegate for PlayFab login failure
    UFUNCTION()
    void OnLoginFailure(FPlayFabError error, UObject* customData);

    UFUNCTION()
    void IniializeInventory();

    // Delegate for Playfab inventory success
    UFUNCTION()
    void GetInventoryOnSuccess(FClientGetUserInventoryResult& Result, UObject* CustomData);

public:
    UFUNCTION()
    void GetUserLevel(FString UserId);

    // Delegate For PlayFab retrevial of user attributes
    UFUNCTION()
    void GetUserLevelOnSuccess(const struct FClientGetUserDataResult& result, UObject* customData);

    UFUNCTION()
    void UpdateUserLevel(FString UserId, FString Level, FString XP);

    // End PlayFab_GameInstanceSubsystem

    // */
};
