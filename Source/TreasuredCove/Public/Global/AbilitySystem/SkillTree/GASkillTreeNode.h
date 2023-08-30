// Fill out your copyright notice in the Description page of Project Settings.

// Header files
#pragma once

#include "CoreMinimal.h"
#include "GenericGraphNode.h"
#include "Templates/SharedPointer.h"
#include "SkillInfoStruct.h"
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

	// virtual void Initialize(UGenericGraph* InGraph) override;
	void InitializeItems();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Abilities")
	FGASkillTreeTier Tier;

	virtual FText GetDescription_Implementation() const override;
	// virtual const FSlateBrush* GetInnerObjectIcon(int32 Index) const override;
	// virtual int32 GetInnerObjectNum() const override;
public:
#if WITH_EDITOR
	void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e) override;

	FOnSkillsUpdated OnSkillsUpdated;
#endif
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
		return Tier.AquiredStatus;
	}
	void SetStatus(EAbilityAquiredStatus Status)
	{
		Tier.AquiredStatus = Status;
	}

	FName GetAbilityName(uint8 AbilityIndex)
	{
		return FName();// Tier.Skills[AbilityIndex].Ability;
	}

	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Abilities")
	// TArray<FSkillInfo> Skills;
};
