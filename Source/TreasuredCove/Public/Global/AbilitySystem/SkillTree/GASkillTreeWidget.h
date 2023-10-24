// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Global/UserInterface/Widgets/GAWidget.h"
#include "GASkillTreeWidget.generated.h"

class UGASkillTree;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UGASkillTreeWidget : public UGAWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree", meta = (ExposeOnSpawn=true))
	UGASkillTree* SkillTree;
};
