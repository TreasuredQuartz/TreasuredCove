// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/GASkillTree.h"
#include "GASkillTreeNode.h"

UGASkillTree::UGASkillTree() : Root(CreateDefaultSubobject<UGASkillTreeNode>(TEXT("Root")))
{
}

void UGASkillTree::Initialize()
{
	BindOnSkillAuquiredToAll(Root);
}

int UGASkillTree::GetHeight()
{
	return 0;
}

void UGASkillTree::BindOnSkillAuquiredToAll(UGASkillTreeNode* Parent)
{
	FScriptDelegate Del = FScriptDelegate();
	Del.BindUFunction(this, FName("OnSkillAquired"));
	Parent->OnRequestAquire.Add(Del);

	if (!Parent->GetNextNodes().IsEmpty())
	{
		for (UGASkillTreeNode* Node : Parent->GetNextNodes())
		{
			BindOnSkillAuquiredToAll(Node);
		}
	}
}

void UGASkillTree::OnSkillAquired(FString Category, UGASkillTreeNode* Node)
{
	if (Points > 0)
	{
		--Points;
		Node->SetStatus(EAbilityAquiredStatus::Aquired);
		OnAquired.Broadcast(Node->GetAbility());

		if (!Node->GetNextNodes().IsEmpty())
		{
			for (UGASkillTreeNode* nextNode : Node->GetNextNodes())
			{
				nextNode->SetStatus(EAbilityAquiredStatus::Unlocked);
			}
		}
	}
}


