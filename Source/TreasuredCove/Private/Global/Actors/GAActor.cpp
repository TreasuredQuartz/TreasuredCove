// Fill out your copyright notice in the Description page of Project Settings.

#include "GAActor.h"
#include "GACharacter.h"
#include "GASystemComponent.h"
#include "GameplayAbilityBase.h"
#include "AbilitySet.h"

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

UAbilitySystemComponent* AGAActor::GetAbilitySystemComponent() const { return AbilitySystem; }

FVector AGAActor::GetHeldHandOffset() const
{
	return DominantHandOffset;
}

FVector AGAActor::GetSupportingHandOffset() const
{
	return SupportingHandOffset;
}

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

		if (InitialActiveAbilities) AquireAbilities(InitialActiveAbilities->GetAbilities(), CurrentActiveAbilityHandles);
	}
}

void AGAActor::InitializeAttributeSet(UAttributeSet* Set)
{

}

void AGAActor::AquireAbilities(const TArray<TSubclassOf<UGameplayAbility>>& InAbilities, TArray<FGameplayAbilitySpecHandle>& OutHandles)
{
	OutHandles.Reserve(InAbilities.Num());
	for (const auto& Ability : InAbilities)
	{
		FGameplayAbilitySpecHandle Handle;
		AquireAbility(Ability, Handle);
		OutHandles.Add(Handle);
		UE_LOG(LogTemp, Warning, TEXT("Item ability obtained."));
	}
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

void AGAActor::UsePrimary_Implementation()
{
	if (CurrentActiveAbilityHandles.IsValidIndex(0) && CurrentActiveAbilityHandles[0].IsValid()) AbilitySystem->TryActivateAbility(CurrentActiveAbilityHandles[0]);
	else UE_LOG(LogTemp, Warning, TEXT("Item Primary not called."));
}

void AGAActor::UseSecondary_Implementation()
{
	if (CurrentActiveAbilityHandles.IsValidIndex(1) && CurrentActiveAbilityHandles[1].IsValid()) AbilitySystem->TryActivateAbility(CurrentActiveAbilityHandles[1]);
}

void AGAActor::UseThrow_Implementation()
{
	if (CurrentActiveAbilityHandles.IsValidIndex(2) && CurrentActiveAbilityHandles[2].IsValid()) AbilitySystem->TryActivateAbility(CurrentActiveAbilityHandles[2]);
}

void AGAActor::UseMelee_Implementation()
{
	if (CurrentActiveAbilityHandles.IsValidIndex(3) && CurrentActiveAbilityHandles[3].IsValid()) AbilitySystem->TryActivateAbility(CurrentActiveAbilityHandles[3]);
}

