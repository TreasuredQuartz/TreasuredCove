// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/SkillTree/GASkillTreeNode.h"
#include "GASkillTree.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#define LOCTEXT_NAMESPACE "SkillTreeNode"

UGASkillTreeNode::UGASkillTreeNode()
{

}

void UGASkillTreeNode::Initialize(UGenericGraph* InGraph)
{
	UGenericGraphNode::Initialize(InGraph);
	InitializeItems();
}

void UGASkillTreeNode::InitializeItems()
{
	if (!Tier.Skills.IsEmpty())
	{
		UGASkillTree* SkillTree = Cast<UGASkillTree>(GetGraph());

		if (SkillTree && SkillTree->SkillTable)
		{
			Items.Empty(Tier.Skills.Num());
			for (FSkillInfo Skill : Tier.Skills)
			{
				UTexture2D* Texture = nullptr;
				TSharedPtr<FSlateBrush> Brush = MakeShared<FSlateBrush>();
				if (FSkillInfoRow* Info = SkillTree->SkillTable->FindRow<FSkillInfoRow>(Skill.AbilityName, "Initialize Skill Icons On Startup"))
					Texture = Info->Texture;
				else
					Brush->TintColor = FSlateColor(FLinearColor(1.0, 1.0, 1.0, 0.f));

				Brush->SetResourceObject(Texture);
				Brush->SetImageSize(FVector2D(100, 100));
				Items.Add(Brush);
			}
		}
	}
}

#if WITH_EDITOR
void UGASkillTreeNode::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e)
{
	// Guard Against null properties
	if (e.Property == NULL) return;
	FName MemberPropertyName = e.MemberProperty->GetFName();
	FName PropertyName = e.Property->GetFName();

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UGASkillTreeNode, Tier) 
		|| PropertyName == GET_MEMBER_NAME_CHECKED(FGASkillTreeTier, Skills)
		|| PropertyName == GET_MEMBER_NAME_CHECKED(FSkillInfo, AbilityName))
	{
		Items.Empty(Tier.Skills.Num());
		InitializeItems();

		OnItemsUpdated.ExecuteIfBound();
		UE_LOG(LogTemp, Warning, TEXT("Property Name: %s, Member Property Name: %s"), *PropertyName.ToString(), *MemberPropertyName.ToString());
	}


	Super::PostEditChangeChainProperty(e);
}
#endif

FText UGASkillTreeNode::GetDescription_Implementation() const
{
	return LOCTEXT("NodeDesc", "Skill Tree Node");
}

const FSlateBrush* UGASkillTreeNode::GetInnerObjectIcon(int32 Index) const
{
	return Items[Index].Get();
}

int32 UGASkillTreeNode::GetInnerObjectNum() const
{
	return Tier.Skills.Num();
}

void UGASkillTreeNode::Aquired()
{
	SetStatus(EAbilityAquiredStatus::Aquired);

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

#undef LOCTEXT_NAMESPACE