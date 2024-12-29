// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/TargetActors/GATargetActor_ProjectilePath.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/PlayerController.h"

void AGATargetActor_ProjectilePath::StartTargeting(UGameplayAbility* Ability)
{
	AGameplayAbilityTargetActor::StartTargeting(Ability);

	OwningController = Cast<APlayerController>(Ability->GetOwningActorFromActorInfo()->GetInstigatorController());
}
