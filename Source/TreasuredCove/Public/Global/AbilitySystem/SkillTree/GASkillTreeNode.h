// Fill out your copyright notice in the Description page of Project Settings.

// Header files
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbilityInfoStruct.h"
#include "GASkillTreeNode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestAquire, UGASkillTreeNode*, Skill);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatusChanged);

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
	FSkillInfo() : AbilityClass(nullptr),
		AquiredStatus(EAbilityAquiredStatus::Locked),
		Name(FText()),
		Description(FText()),
		UINodeTexture(nullptr)
	{}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<UGameplayAbilityBase> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	EAbilityAquiredStatus AquiredStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	UTexture2D* UINodeTexture;
};

/** A node for the skill tree linked list.
 * Contains a reference to any or all nodes after this one.
 */
UCLASS(Blueprintable, EditInlineNew)
class TREASUREDCOVE_API UGASkillTreeNode : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FSkillInfo Data;

	UPROPERTY(Instanced, BlueprintReadWrite, EditDefaultsOnly, Category = "Abilities")
	TArray<UGASkillTreeNode*> NextNodes;

public:
#if WITH_EDITOR
	void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e) override;
#endif
	// UFUNCTION(BlueprintPure)
	// TArray<UGASkillTreeNode*> GetNextNodes()
	/*{
		return NextNodes;
	}*/
	UFUNCTION(BlueprintPure)
	TArray<UGASkillTreeNode*> GetAllNodes();

	UPROPERTY(BlueprintAssignable)
	FOnRequestAquire OnRequestAquire;
	UPROPERTY(BlueprintAssignable)
	FOnStatusChanged OnAquired;
	UPROPERTY(BlueprintAssignable)
	FOnStatusChanged OnUnlocked;
	UPROPERTY(BlueprintAssignable)
	FOnStatusChanged OnLocked;

	UFUNCTION(BlueprintCallable)
	void RequestAquire()
	{
		OnRequestAquire.Broadcast(this);
	}
	void Aquired();
	void Unlocked();
	void Locked();

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
		return Data.AbilityClass;
	}
};
