// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Global/AbilitySystem/SkillTree/GASkillTree.h"
#include "GASkillTreeNode.h"
#include "GASkillTree_Static.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UGASkillTree_Static : public UGASkillTree
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Rows = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Columns = 1;

public:
	FSkillInfo GetSkillAtRowAndColumn(int32 Row, int32 Column) const;
};
