// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/SkillTree/GASkillTreeNode.h"

#if WITH_EDITOR
void UGASkillTreeNode::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e)
{
	// Guard Against null properties
	if (e.Property == NULL) return;
	/*
	FName MemberPropertyName = e.MemberProperty->GetFName();
	FName PropertyName = e.Property->GetFName();

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UGASkillTreeNode, NextNodes))
	{
		switch (e.ChangeType)
		{
		case EPropertyChangeType::ArrayAdd:
			NextNodes[NextNodes.Num() - 1] = NewObject<UGASkillTreeNode>();
			break;
		}
	}
	*/
	Super::PostEditChangeChainProperty(e);
}
#endif

TArray<UGASkillTreeNode*> UGASkillTreeNode::GetAllNodes()
{
	TArray<UGASkillTreeNode*> Value;

	if (!NextNodes.IsEmpty())
	{
		for (int i = 0; i < NextNodes.Num(); ++i)
		{
			Value.Append(NextNodes[i]->GetAllNodes());
		}
	}
	else
	{
		Value.Add(this);
	}

	return Value;
}

void UGASkillTreeNode::Aquired()
{
	SetStatus(EAbilityAquiredStatus::Aquired);

	if (!NextNodes.IsEmpty())
	{
		for (UGASkillTreeNode* nextNode : NextNodes)
		{
			nextNode->SetStatus(EAbilityAquiredStatus::Unlocked);
		}
	}

	OnAquired.Broadcast();
}

void UGASkillTreeNode::Unlocked()
{
	SetStatus(EAbilityAquiredStatus::Unlocked);

	OnUnlocked.Broadcast();
}

void UGASkillTreeNode::Locked()
{
	SetStatus(EAbilityAquiredStatus::Locked);

	OnLocked.Broadcast();
}
