// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "PlayerSettings.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API APlayerSettings : public AInfo
{
	GENERATED_BODY()
	
public:
	bool bIsUsingController = false;
	bool bWeaponPriority = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	bool bToggleCrouch = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	bool bToggleQuickSelect = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	bool bToggleSprint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	bool bLockedViewpoint = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	bool bFirstPerson = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	FName DominantHand; // DominantHand = FName("grip_r");
	uint8 LookUpperLimit = 80;
	int8 InvertUpDown = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	float VerticalLookRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	float HorizontalLookRate;
};
