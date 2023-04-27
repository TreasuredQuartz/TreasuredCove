// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/SkillTree/GASkillTree.h"
#include "GASkillTreeNode.h"

UGASkillTree::UGASkillTree() : Root(CreateDefaultSubobject<UGASkillTreeNode>(TEXT("Root")))
{
}

void UGASkillTree::Initialize()
{
	BindOnSkillAquiredToAll(Root);
}

int UGASkillTree::GetHeight()
{
	return 0;
}

void UGASkillTree::BindOnSkillAquiredToAll(UGASkillTreeNode* Parent)
{
	FScriptDelegate Del = FScriptDelegate();
	Del.BindUFunction(this, FName("OnSkillAquired"));
	Parent->OnRequestAquire.Add(Del);

	if (!Parent->NextNodes.IsEmpty())
	{
		for (UGASkillTreeNode* Node : Parent->NextNodes)
		{
			BindOnSkillAquiredToAll(Node);
		}
	}
}

void UGASkillTree::OnSkillAquired(FString Category, UGASkillTreeNode* Node)
{
	if (Points > 0)
	{
		--Points;
		Node->Aquired();
		OnAquired.Broadcast(Node->GetAbility());
	}
}


