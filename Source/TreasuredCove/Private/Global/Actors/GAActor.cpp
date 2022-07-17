// Fill out your copyright notice in the Description page of Project Settings.

#include "GAActor.h"
#include "GACharacter.h"
#include "GASystemComponent.h"
#include "GameplayAbilityBase.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ProceduralMeshComponent.h"

#include "Engine/Engine.h"

// Sets default values
AGAActor::AGAActor()
{
	// Ability System Component
	AbilitySystem =
		CreateDefaultSubobject<UGASystemComponent>(TEXT("AbilitySystem"));
}

UAbilitySystemComponent* AGAActor::GetAbilitySystemComponent() const {return AbilitySystem; };

// Called when the game starts or when spawned
void AGAActor::BeginPlay()
{
	Super::BeginPlay();
}

void AGAActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AGAActor::IntializeAbilitySystem()
{
	if (AbilitySystem)
	{
		AbilitySystem->InitAbilityActorInfo(this, this);

		if (!AttributeSetClasses.IsEmpty())
		{
			for (TSubclassOf<UAttributeSet> SetClass : AttributeSetClasses)
			{
				UAttributeSet* Set = const_cast<UAttributeSet*>(AbilitySystem->InitStats(SetClass, InitialStatsTable));
				InitializeAttributeSet(Set);
			}
		}

		if (CurrentActiveAbilities.Num() > 0)
		{
			int i = -1;
			for (TSubclassOf<UGameplayAbilityBase> Ability : CurrentActiveAbilities)
			{
				AquireAbility(Ability, CurrentActiveAbilityHandles[++i]);
			}
		}
	}
}

void AGAActor::InitializeAttributeSet(UAttributeSet* Set)
{

}

void AGAActor::AquireAbility(TSubclassOf<UGameplayAbility> InAbility, FGameplayAbilitySpecHandle& OutHandle)
{
	if (AbilitySystem)
	{
		if (HasAuthority() && InAbility)
		{
			FGameplayAbilitySpecDef SpecDef = FGameplayAbilitySpecDef();
			SpecDef.Ability = InAbility;
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(SpecDef, 1);
			OutHandle = AbilitySystem->GiveAbility(AbilitySpec);
		}
	}
}



