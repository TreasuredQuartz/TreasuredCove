// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/GASkillTreeComponent.h"
#include "GameplayAbilityBase.h"
#include "GASkillTree.h"
#include "GASkillTreeNode.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

// Sets default values for this component's properties
UGASkillTreeComponent::UGASkillTreeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGASkillTreeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	IAbilitySystemInterface* Owner = Cast<IAbilitySystemInterface>(GetOwner());
	if (Owner) AbilityComponent = Owner->GetAbilitySystemComponent();
	if (!SkillTrees.IsEmpty())
	{
		TArray<UGASkillTree*> Trees;
		SkillTrees.GenerateValueArray(Trees);

		if (!Trees.IsEmpty())
		for (UGASkillTree* SkillTree : Trees)
		{
			SkillTree->Initialize();
			FScriptDelegate Del = FScriptDelegate();
			Del.BindUFunction(this, FName("AquireSkill"));
			SkillTree->OnAquired.Add(Del);
		}
	}
}


// Called every frame
void UGASkillTreeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGASkillTreeComponent::AquireSkill(TSubclassOf<UGameplayAbilityBase> Ability)
{
	if (AbilityComponent)
	{
		if (GetOwner()->HasAuthority() && Ability)
		{
			FGameplayAbilitySpecDef SpecDef = FGameplayAbilitySpecDef();
			SpecDef.Ability = Ability;
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(SpecDef, 1);
			SkillHandles.Add(AbilityComponent->GiveAbility(AbilitySpec));

			/*if (InAbility->IsChildOf(UGameplayAbilityBase::StaticClass()))
			{
				TSubclassOf<UGameplayAbilityBase> AbilityBaseClass = *InAbility;
				if (AbilityBaseClass != nullptr)
				{
					AddAbilityToUI(AbilityBaseClass, AbilityType, OutHandle, bFromItem);
				}
			}*/
		}
	}
}

void UGASkillTreeComponent::AddSkillList(FString Category, UGASkillTree* SkillTree)
{
	if (SkillTree)
	{
		SkillTrees.Add(Category, SkillTree);
	}
}

