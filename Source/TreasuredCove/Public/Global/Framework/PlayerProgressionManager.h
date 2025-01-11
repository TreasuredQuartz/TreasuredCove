// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "PlayerProgressionManager.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API APlayerProgressionManager : public AInfo
{
	GENERATED_BODY()
	
public:
	// Skill trees component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities", meta = (AllowPrivateAccess = "true"))
	class UGASkillTreeComponent* SkillTrees;

	// Classes component
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities", meta = (AllowPrivateAccess = "true"))
	// class UCharacterClassComponent* Classes;
};
