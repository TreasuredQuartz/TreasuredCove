// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GAGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UGAGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	bool bIsUsingController;

	virtual void Init() override;
	void OnControllerChanged(bool bIsConnected, FPlatformUserId UserId, int32 UserIndex);
};
