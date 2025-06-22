// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/SkillTree/GASkillTreeNode.h"
#include "Global/AbilitySystem/SkillTree/GASkillTree.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#define LOCTEXT_NAMESPACE "SkillTreeNode"

UGASkillTreeNode::UGASkillTreeNode()
{
	AquiredStatus = NewObject<UAquiredStatusNodeComponent>(GetOuter(), TEXT("Aquired Status"));

	Skill = NewObject<USkillNodeComponent>(GetOuter(), TEXT("Skill"));

	AddInstanceComponent(AquiredStatus);
	AddInstanceComponent(Skill);
}

#if WITH_EDITOR
void UGASkillTreeNode::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e)
{
	Super::PostEditChangeChainProperty(e);
}
#endif

FText UGASkillTreeNode::GetDescription_Implementation() const
{
	return LOCTEXT("NodeDesc", "Skill Tree Node");
}

void UAquiredStatusNodeComponent::Aquired()
{
	SetStatus(EAbilityAquiredStatus::Aquired);

	OnAquired.Broadcast();
}

void UAquiredStatusNodeComponent::Unlocked()
{
	SetStatus(EAbilityAquiredStatus::Unlocked);

	OnUnlocked.Broadcast();
}

void UAquiredStatusNodeComponent::Locked()
{
	SetStatus(EAbilityAquiredStatus::Locked);

	OnLocked.Broadcast();
}

#undef LOCTEXT_NAMESPACE