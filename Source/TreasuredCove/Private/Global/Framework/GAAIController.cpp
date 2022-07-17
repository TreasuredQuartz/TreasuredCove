// Fill out your copyright notice in the Description page of Project Settings.

#include "GAAIController.h"
#include "GACharacter.h"

#include "TownSystemComponent.h"
#include "GameplayBuilding.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AGAAIController::AGAAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTHumanObject(TEXT("/Game/GameplayAbilitiesBP/Blueprint/AI/BT_Common"));
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTGoblinObject(TEXT("/Game/GameplayAbilitiesBP/Blueprint/AI/BT_Common"));
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTOctoObject(TEXT("/Game/GameplayAbilitiesBP/Blueprint/AI/BT_Common"));
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTRobotObject(TEXT("/Game/GameplayAbilitiesBP/Blueprint/AI/BT_Common"));
	
	BehaviorTreeAssets.SetNumUninitialized(4);

	if (BTGoblinObject.Succeeded())
	{
		BehaviorTreeAssets[0] =
			BTGoblinObject.Object;
	}
	if (BTHumanObject.Succeeded())
	{
		BehaviorTreeAssets[1] =
			BTHumanObject.Object;
	}
	if (BTOctoObject.Succeeded())
	{
		BehaviorTreeAssets[2] =
			BTOctoObject.Object;
	}
	if (BTRobotObject.Succeeded())
	{
		BehaviorTreeAssets[3] =
			BTRobotObject.Object;
	}

	CurrentBuildingKey = FName("CurrentBuilding");
	CurrentEnemyKey = FName("CurrentEnemy");
	DesiredLocationKey = FName("DesiredLocation");
	IsBusyKey = FName("IsBusy");
	IsFollowerKey = FName("IsFollower");
	IsInCombatKey = FName("IsInCombat");
	IsInDialogueKey = FName("IsInDialogue");
	TargetBuildingKey = FName("TargetBuilding");
	WarPartyRoleKey = FName("WarPartyRole");
}

void AGAAIController::BeginPlay()
{
}

void AGAAIController::OnPossess(APawn* PossessedPawn)
{
	CharacterRef = Cast<AGACharacter>(PossessedPawn);
}

void AGAAIController::GetGameSpeed(float InGameSpeed)
{
	GameSpeed = InGameSpeed;
	SetStatus();
}

//----- AI Functions -----//

void AGAAIController::SetStatus()
{
	SetFatigueTimers();
	SetHungerTimers();
}

void AGAAIController::SetFatigueTimers()
{
	if (GetWorldTimerManager().IsTimerPaused(FatigueGainTimerHandle))
	{
		GetWorldTimerManager().SetTimer(FatigueLossTimerHandle, this, &AGAAIController::IncreaseFatigue, SetTimerSpeed(), true);
	}
	else
	{
		GetWorldTimerManager().SetTimer(FatigueGainTimerHandle, this, &AGAAIController::DecreaseFatigue, SetTimerSpeed(), true);
	}
}

void AGAAIController::SetHungerTimers()
{
	if (GetWorldTimerManager().IsTimerPaused(HungerGainTimerHandle))
	{
		GetWorldTimerManager().SetTimer(HungerLossTimerHandle, this, &AGAAIController::IncreaseHunger, SetTimerSpeed(), true);
	}
	else
	{
		GetWorldTimerManager().SetTimer(HungerGainTimerHandle, this, &AGAAIController::DecreaseHunger, SetTimerSpeed(), true);
	}
}

void AGAAIController::IncreaseFatigue()
{
	float& Fatigue = CharacterRef->AIInfo.CurrentAITargetStats.Fatigue;
	float Value = Fatigue + (FatigueGainDelta / 60);
	Fatigue = FMath::Clamp(Value, 0.f, 1.f);
}

void AGAAIController::DecreaseFatigue()
{
	float& Fatigue = CharacterRef->AIInfo.CurrentAITargetStats.Fatigue;
	float Value = Fatigue - (FatigueLossDelta / 60);
	Fatigue = FMath::Clamp(Value, 0.f, 1.f);
}

void AGAAIController::IncreaseHunger()
{
	float& Hunger = CharacterRef->AIInfo.CurrentAITargetStats.Hunger;
	float Value = Hunger + (HungerGainDelta / 60);
	Hunger = FMath::Clamp(Value, 0.f, 1.f);
}

void AGAAIController::DecreaseHunger()
{
	float& Hunger = CharacterRef->AIInfo.CurrentAITargetStats.Hunger;
	float Value = Hunger + (HungerLossDelta / 60);
	Hunger = FMath::Clamp(Value, 0.f, 1.f);
}

//----- Update keys -----//

void AGAAIController::UpdateCurrentBuilding(AGameplayBuilding* CurrentBuilding)
{
	Blackboard->SetValueAsObject(CurrentBuildingKey, CurrentBuilding);
}

void AGAAIController::UpdateCurrentEnemy(AActor* NewEnemy)
{
	Blackboard->SetValueAsObject(CurrentEnemyKey, NewEnemy);
}

void AGAAIController::UpdateDesiredLocation(FVector DesiredLocation)
{
	Blackboard->SetValueAsVector(DesiredLocationKey, DesiredLocation);
}

void AGAAIController::UpdateIsBusy(bool bIsBusy)
{
	Blackboard->SetValueAsBool(IsBusyKey, bIsBusy);
}

void AGAAIController::UpdateTargetBuilding(AGameplayBuilding* TargetBuilding)
{
	Blackboard->SetValueAsObject(TargetBuildingKey, TargetBuilding);
}

void AGAAIController::UpdateWarPartyRollKey(bool bIsFollower)
{
	Blackboard->SetValueAsBool(IsFollowerKey, bIsFollower);
}

//----- Sensory Responses -----//

void AGAAIController::OnPawnSeen(APawn* SeenPawn)
{

}

void AGAAIController::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{

}

void AGAAIController::OnDamaged(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue)
{
	
}

void AGAAIController::OnHealed(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue)
{

}
