// Fill out your copyright notice in the Description page of Project Settings.

#include "GAAIController.h"
#include "GACharacter.h"

#include "TownSystemComponent.h"
#include "GameplayBuilding.h"

// Sense System Plugin
#include "SenseReceiverComponent.h"
#include "SenseStimulusBase.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

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
	CurrentEnemyKey = FName("Enemy");
	DesiredLocationKey = FName("DesiredLocation");
	HeardLocationKey = FName("HeardLocation");
	IsBusyKey = FName("IsBusy");
	IsFollowerKey = FName("IsFollower");
	IsInCombatKey = FName("IsInCombat");
	IsInDialogueKey = FName("IsInDialogue");
	HeardSomethingKey = FName("HeardSomething");
	TargetBuildingKey = FName("TargetBuilding");
	WarPartyRoleKey = FName("WarPartyRole");
}

void AGAAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AGAAIController::OnPossess(APawn* PossessedPawn)
{
	Super::OnPossess(PossessedPawn);

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
	if (Blackboard)
		Blackboard->SetValueAsObject(CurrentBuildingKey, CurrentBuilding);
}

void AGAAIController::UpdateCurrentEnemy(AActor* NewEnemy)
{
	CurrentEnemy = NewEnemy;
	if (Blackboard)
		Blackboard->SetValueAsObject(CurrentEnemyKey, CurrentEnemy);
}

void AGAAIController::UpdateDesiredLocation(const FVector& DesiredLocation)
{
	if (Blackboard)
		Blackboard->SetValueAsVector(DesiredLocationKey, DesiredLocation);
}

void AGAAIController::UpdateHeardLocation(const FVector& HeardLocation)
{
	if (Blackboard)
		Blackboard->SetValueAsVector(HeardLocationKey, HeardLocation);
}

void AGAAIController::UpdateIsBusy(bool bIsBusy)
{
	if (Blackboard)
		Blackboard->SetValueAsBool(IsBusyKey, bIsBusy);
}

void AGAAIController::UpdateHeardSomething(bool bHeardSomething)
{
	if (Blackboard)
		Blackboard->SetValueAsBool(HeardSomethingKey, bHeardSomething);
}

void AGAAIController::UpdateTargetBuilding(AGameplayBuilding* TargetBuilding)
{
	if (Blackboard)
		Blackboard->SetValueAsObject(TargetBuildingKey, TargetBuilding);
}

void AGAAIController::UpdateWarPartyRollKey(bool bIsFollower)
{
	if (Blackboard)
		Blackboard->SetValueAsBool(IsFollowerKey, bIsFollower);
}

//----- Sensory Responses -----//

void AGAAIController::AddEnemy(AActor* NewEnemy)
{
	Enemies.Add(NewEnemy);
}

void AGAAIController::OnDamaged(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue)
{

}

void AGAAIController::OnHealed(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue)
{

}

void AGAAIController::OnNewSense(const USensorBase* Sensor, int32 Channel, const TArray<FSensedStimulus>& SensedStimulus)
{
	for (const FSensedStimulus& Stimulus : SensedStimulus)
	{
		if (!Stimulus.StimulusComponent.IsValid())
		{
			// GEngine->AddOnScreenDebugMessage(9, 5.f, FColor::Cyan, "Stimulus Component is invalid!");
			continue;
		}

		if (Sensor->SensorTag == FName("SensorSight"))
		{
			if (AGACharacter* SeenCharacter = Stimulus.StimulusComponent.Get()->GetOwner<AGACharacter>())
			{
				if (SeenCharacter->GetIsDead())
				{
					// GEngine->AddOnScreenDebugMessage(9, 5.f, FColor::Cyan, "Seen Character is dead!");
					continue;
				}

				/*FAICharacterInfo SeenInfo(SeenCharacter, SeenCharacter->GetActorLocation(), 0);

				FAICharacterAwareness& AIState = SeenInfo.CurrentAIAwarenessTowardTarget;

				AIState.SetCurrentSuspicion(1);

				GetWorldTimerManager().SetTimer(SeenInfo.AwarenessTimerHandle, ReactionTime, true);
				AwareOfCharacters.Add(SeenInfo);*/

				if (CurrentEnemy == nullptr && Enemies.Contains(SeenCharacter))
				{
					// GEngine->AddOnScreenDebugMessage(10, 5.f, FColor::Cyan, "Updated Current Enemy.");
					SeenCharacter->OnDeathDelegate.AddUniqueDynamic(this, &AGAAIController::OnEnemyDeath);
					UpdateCurrentEnemy(SeenCharacter);
					OnEnemyDiscovered();
				}
				else
				{
					// GEngine->AddOnScreenDebugMessage(10, 5.f, FColor::Cyan, "CurrentEnemy is not nullptr or enemies does not contain character.");
				}
			}
		}
		else if (Sensor->SensorTag.IsEqual(FName("SensorHearing")))
		{
			if (CurrentEnemy != nullptr)
				continue;

			if (AActor* HeardActor = Stimulus.StimulusComponent.Get()->GetOwner()->GetOwner())
			{
				if (HeardActor != this && Enemies.Contains(HeardActor))
				{
					if (AGACharacter* HeardCharacter = Cast<AGACharacter>(HeardActor))
					{
						if (HeardCharacter->GetIsDead())
							continue;
					}

					UpdateHeardSomething(true);
					UpdateHeardLocation(HeardActor->GetActorLocation());
					GEngine->AddOnScreenDebugMessage(10, 5.f, FColor::Cyan, "We heard The Player.");
				}
			}
		}
		else
		{
			// GEngine->AddOnScreenDebugMessage(10, 5.f, FColor::Cyan, "Owner of Component fails cast to AGACharacter.");
		}
	}
}

void AGAAIController::OnCurrentSense(const USensorBase* Sensor, int32 Channel, const TArray<struct FSensedStimulus>& SensedStimulus)
{

}

void AGAAIController::OnLostSense(const USensorBase* Sensor, int32 Channel, const TArray<struct FSensedStimulus>& SensedStimulus)
{
	for (const FSensedStimulus& Stimulus : SensedStimulus)
	{
		if (!Stimulus.StimulusComponent.IsValid()) continue;

		if (Sensor->SensorTag == FName("SensorSight"))
		{
			if (AGACharacter* SeenCharacter = Cast<AGACharacter>(Stimulus.StimulusComponent.Get()->GetOwner()))
			{
				if (CurrentEnemy == SeenCharacter)
				{
					SeenCharacter->OnDeathDelegate.RemoveDynamic(this, &AGAAIController::OnEnemyDeath);
					UpdateCurrentEnemy(nullptr);
					OnEnemyLost();
				}
			}
		}
		else if (Sensor->SensorTag == FName("SensorHearing"))
		{
			UpdateHeardSomething(false);
		}
	}
}

void AGAAIController::OnForgetSense(const USensorBase* Sensor, int32 Channel, const TArray<struct FSensedStimulus>& SensedStimulus)
{
	if (Sensor == nullptr || SensedStimulus.IsEmpty())
		return;

	int Num = SensedStimulus.Num();

	for (int i = 0; i < Num; ++i)
	{
		if (!SensedStimulus.IsValidIndex(i))
			continue;

		const FSensedStimulus& Stimulus = SensedStimulus[i];

		if (!Stimulus.StimulusComponent.IsValid()) 
			continue;

		if (Sensor->SensorTag == FName("SensorSight"))
		{
			if (AGACharacter* SeenCharacter = Cast<AGACharacter>(Stimulus.StimulusComponent.Get()->GetOwner()))
			{
				if (CurrentEnemy == SeenCharacter)
				{
					SeenCharacter->OnDeathDelegate.RemoveDynamic(this, &AGAAIController::OnEnemyDeath);
					UpdateCurrentEnemy(nullptr);
					OnEnemyLost();
				}
			}
		}
		else if (Sensor->SensorTag == FName("SensorHearing"))
		{
			UpdateHeardSomething(false);
		}
	}
}

void AGAAIController::OnEnemyDiscovered_Implementation()
{
	
}

void AGAAIController::OnEnemyLost_Implementation()
{

}

void AGAAIController::OnEnemyDeath()
{
	UpdateCurrentEnemy(nullptr);
	OnEnemyLost();
}