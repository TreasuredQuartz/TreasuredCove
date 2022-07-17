// Fill out your copyright notice in the Description page of Project Settings.

// Header files
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbilityInfoStruct.h"
#include "GASkillTreeNode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRequestAquire, FString, Category, UGASkillTreeNode*, Skill);

UENUM(BlueprintType)
enum class EAbilityAquiredStatus : uint8
{
	Locked,
	Unlocked,
	Aquired
};

USTRUCT(BlueprintType)
struct FSkillInfo 
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	EAbilityAquiredStatus AquiredStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FString Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FAbilityInfo AbilityInfo;
};

/** A node for the skill tree linked list.
 * Contains a reference to any or all nodes after this one.
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API UGASkillTreeNode : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FSkillInfo Data;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TArray<UGASkillTreeNode*> NextNodes;

public:
#if WITH_EDITOR
	void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e) override;
#endif
	UFUNCTION(BlueprintPure)
	TArray<UGASkillTreeNode*> GetNextNodes()
	{
		return NextNodes;
	}
	UFUNCTION(BlueprintPure)
	TArray<UGASkillTreeNode*> GetAllNodes();

	UPROPERTY(BlueprintAssignable)
	FOnRequestAquire OnRequestAquire;
	UFUNCTION(BlueprintCallable)
	void RequestAquire()
	{
		OnRequestAquire.Broadcast(Data.Category, this);
	}

	// Accessors and Mutators //

	EAbilityAquiredStatus GetStatus()
	{
		return Data.AquiredStatus;
	}
	void SetStatus(EAbilityAquiredStatus Status)
	{
		Data.AquiredStatus = Status;
	}

	TSubclassOf<UGameplayAbilityBase> GetAbility()
	{
		return Data.AbilityInfo.Class;
	}
};
