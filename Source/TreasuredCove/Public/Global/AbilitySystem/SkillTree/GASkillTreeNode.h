// Fill out your copyright notice in the Description page of Project Settings.

// Header files
#pragma once

#include "CoreMinimal.h"
#include "GenericGraphNode.h"
#include "GenericGraphNodeComponent.h"
#include "Templates/SharedPointer.h"
#include "Global/Structs/SkillInfoStruct.h"
#include "GASkillTreeNode.generated.h"

class UGASkillNode;

#if WITH_EDITOR
DECLARE_DELEGATE(FOnSkillsUpdated);
#endif

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestAquire, UGASkillTreeNode*, Skill);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatusChanged);

/** A node for the skill tree linked list.
 * Contains a reference to any or all nodes after this one.
 */
UCLASS(Blueprintable, EditInlineNew)
class TREASUREDCOVE_API UGASkillTreeNode : public UGenericGraphNode
{
	GENERATED_BODY()

public:
	UGASkillTreeNode();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Abilities")
	UAquiredStatusNodeComponent* AquiredStatus;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Abilities")
	USkillNodeComponent* Skill;

	virtual FText GetDescription_Implementation() const override;

public:
#if WITH_EDITOR
	void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e) override;

	FOnSkillsUpdated OnSkillsUpdated;
#endif
};

/**
 *
 */
UCLASS(ClassGroup = (GameplayAbilities), meta = (BlueprintSpawnableComponent))
class TREASUREDCOVE_API USkillNodeComponent : public UGenericGraphNodeComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<class UGameplayAbilityBase> AbilityClass;

public:
	TSubclassOf<class UGameplayAbilityBase> GetAbilityClass() { return AbilityClass; }
};

/**
 *
 */
UCLASS(ClassGroup = (GameplayAbilities), meta = (BlueprintSpawnableComponent))
class TREASUREDCOVE_API UAquiredStatusNodeComponent : public UGenericGraphNodeComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	EAbilityAquiredStatus AquiredStatus;

public:
	UFUNCTION(BlueprintCallable)
	void RequestAquire()
	{
		OnRequestAquire.Broadcast(Cast<UGASkillTreeNode>(GetOwner()));
	}
	void Aquired();
	void Unlocked();
	void Locked();

	UFUNCTION(BlueprintPure, BlueprintCallable)
	EAbilityAquiredStatus GetStatus()
	{
		return AquiredStatus;
	}
	void SetStatus(EAbilityAquiredStatus Status)
	{
		AquiredStatus = Status;
	}

public:
	UPROPERTY(BlueprintAssignable)
	FOnRequestAquire OnRequestAquire;
	UPROPERTY(BlueprintAssignable)
	FOnStatusChanged OnAquired;
	UPROPERTY(BlueprintAssignable)
	FOnStatusChanged OnUnlocked;
	UPROPERTY(BlueprintAssignable)
	FOnStatusChanged OnLocked;
};
