// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/SkillTree/GASkillTreeComponent.h"
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
	/*if (!InitialSkillTreeClasses.IsEmpty())
	{
		TArray<FString> Keys;
		InitialSkillTreeClasses.GetKeys(Keys);
		for (FString Key : Keys)
		{
			if (TSubclassOf<UGASkillTree>* Class = InitialSkillTreeClasses.Find(Key))
			{
				AddSkillList(Key, *Class);
			}
		}
	}*/
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
		}
	}
}

void UGASkillTreeComponent::AddSkillList(FString Category, UGASkillTree* SkillTree)
{
	if (SkillTree)
	{
		SkillTree->Initialize();
		SkillTree->OnAquired.AddDynamic(this, &UGASkillTreeComponent::AquireSkill);
		SkillTrees.Add(Category, SkillTree);
	}
}

void UGASkillTreeComponent::AddSkillList(FString Category, TSubclassOf<UGASkillTree> SkillTreeClass)
{
	if (SkillTreeClass)
	{
		UGASkillTree* SkillTree = NewObject<UGASkillTree>(SkillTreeClass);
		SkillTree->Initialize();
		SkillTree->OnAquired.AddDynamic(this, &UGASkillTreeComponent::AquireSkill);
		SkillTrees.Add(Category, SkillTree);
		
		// Debugging
		/*UE_LOG(LogTemp, Warning, TEXT("New Skill Tree Obtained!"));

		if (SkillTree->Root)
		{
			UE_LOG(LogTemp, Warning, TEXT("Root is valid!"));

			if (SkillTree->Root->NextNodes.IsEmpty())
			{
				UE_LOG(LogTemp, Warning, TEXT("Next Nodes is empty!"));
			}
		}*/
	}
}
