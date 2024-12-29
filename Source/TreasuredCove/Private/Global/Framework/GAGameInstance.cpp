// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/Framework/GAGameInstance.h"
#include "AbilitySystemGlobals.h"

void UGAGameInstance::Init()
{
	Super::Init();
	// FCoreDelegates::OnControllerConnectionChange.AddSP(TSharedRef<UGAGameInstance>(this), &UGAGameInstance::OnControllerChanged);

	UAbilitySystemGlobals& ASG = UAbilitySystemGlobals::Get();
	if (!ASG.IsAbilitySystemGlobalsInitialized())
	{
		ASG.InitGlobalData();
	}
}

//
void UGAGameInstance::OnControllerChanged(bool bIsConnected, FPlatformUserId UserId, int32 UserIndex)
{
	// If we ever need an event for this:
	/**
	* AGAPlayerController* Controller = Cast<AGAPlayerController>(this->GetLocalPlayerByIndex(UserIndex));
	* if (Controller)
	* {
	*	// Controller->OnControllerChanged(bIsConnected);
	* }
	*/
	bIsUsingController = bIsConnected;
}