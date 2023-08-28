// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/SkillTree/GASkillTree.h"
#include "GASkillTreeNode.h"

UGASkillTree::UGASkillTree()
{
	NodeType = UGASkillTreeNode::StaticClass();
}

void UGASkillTree::Initialize()
{
	BindOnSkillAquiredToAll();
}

void UGASkillTree::BindOnSkillAquiredToAll()
{
	for (UGenericGraphNode* Node : AllNodes)
		if (UGASkillTreeNode* SkillNode = Cast<UGASkillTreeNode>(Node))
			BindOnSkillAquired(SkillNode);
}

void UGASkillTree::BindOnSkillAquired(UGASkillTreeNode* InNode)
{
	FScriptDelegate Del = FScriptDelegate();
	Del.BindUFunction(this, FName("OnSkillAquired"));
	InNode->OnRequestAquire.Add(Del);
}

void UGASkillTree::OnSkillAquired(FString Category, UGASkillTreeNode* Node, uint8 SkillIndex)
{
	if (Points > 0)
	{
		--Points;
		Node->Aquired();
		OnAquired.Broadcast(SkillTable->FindRow<FSkillInfoRow>(Node->GetAbilityName(SkillIndex), "Aquiring Skill!")->Class);
	}
}


