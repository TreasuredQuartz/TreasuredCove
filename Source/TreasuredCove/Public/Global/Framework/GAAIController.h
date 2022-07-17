// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AttributeType.h"
#include "Race.h"
#include "AICharacterInfoStruct.h"
#include "GAAIController.generated.h"

class APawn;
class UBehaviorTree;

class AGameplayBuilding;

class AGACharacter;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API AGAAIController : public AAIController
{
	GENERATED_BODY()

public:
	void SetFatigueTimers();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle FatigueLossTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle FatigueGainTimerHandle;
	void IncreaseFatigue();
	void DecreaseFatigue();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FatigueGainDelta;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FatigueLossDelta;

	void SetHungerTimers();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle HungerLossTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle HungerGainTimerHandle;
	void IncreaseHunger();
	void DecreaseHunger();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HungerGainDelta;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HungerLossDelta;

	void GetGameSpeed(float InGameSpeed);
	float GameSpeed;

	float SetTimerSpeed()
	{
		return GameSpeed / 60;
	}

	UFUNCTION(BlueprintCallable)
	void SetStatus();
public:
	AGAAIController(const FObjectInitializer& ObjectInitializer);
	
	FName CurrentBuildingKey;
	FName CurrentEnemyKey;
	FName DesiredLocationKey;
	FName IsBusyKey;
	FName IsFollowerKey;
	FName IsInCombatKey;
	FName IsInDialogueKey;
	FName TargetBuildingKey;
	FName WarPartyRoleKey;
public:
	void GetCharacterAIStats();
	void UpdateCurrentBuilding(AGameplayBuilding* CurrentBuilding);
	void UpdateCurrentEnemy(AActor* CurrentEnemy);
	void UpdateDesiredLocation(FVector DesiredLocation);
	void UpdateIsBusy(bool bIsBusy);
	void UpdateTargetBuilding(AGameplayBuilding* TargetBuilding);
	void UpdateWarPartyRollKey(bool bIsFollower);

	void OnPawnSeen(APawn* SeenPawn);
	void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);
	void OnHealed(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue);
	void OnDamaged(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue);

	TArray<UBehaviorTree*> BehaviorTreeAssets;

	AGACharacter* CharacterRef;
	AGACharacter* CurrentEnemy;
	TArray<AGACharacter*> Enemies;
	TArray<AGACharacter*> Friends;
public:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* PossessedPawn) override;
};
