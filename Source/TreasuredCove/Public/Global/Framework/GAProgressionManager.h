// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "GAProgressionManager.generated.h"

/** While APlayerState is replicated across all clients
 * This class is intended to replicate for each client only.
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API AGAProgressionManager : public AInfo
{
	GENERATED_BODY()

public:
	AGAProgressionManager();

protected:
	APawn* OwningPlayer;

public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE APawn* GetPlayer() { return OwningPlayer; };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Progression", meta = (AllowPrivateAccess = "true"))
	class UHexProgressionComponent* SkillHexGridComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|Abilities", meta = (AllowPrivateAccess = "true"))
	class UGASkillTreeComponent* SkillTreeComponent;
};
