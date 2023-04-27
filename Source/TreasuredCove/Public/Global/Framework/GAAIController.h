// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AttributeType.h"
#include "Race.h"
#include "AICharacterInfoStruct.h"
#include "SensedStimulStruct.h"
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
	FName TargetBuildingKey;
	FName CurrentEnemyKey;
	FName DesiredLocationKey;
	FName HeardLocationKey;
	FName IsBusyKey;
	FName IsFollowerKey;
	FName IsInCombatKey;
	FName IsInDialogueKey;
	FName HeardSomethingKey;
	FName WarPartyRoleKey;
public:
	UFUNCTION(BlueprintCallable, Category = "AIController | Senses")
	void AddEnemy(AActor* NewEnemy);
	void GetCharacterAIStats();
	UFUNCTION()
	void UpdateCurrentBuilding(AGameplayBuilding* CurrentBuilding);
	void UpdateTargetBuilding(AGameplayBuilding* TargetBuilding);
	void UpdateCurrentEnemy(AActor* CurrentEnemy);
	void UpdateDesiredLocation(const FVector& DesiredLocation);
	void UpdateIsBusy(bool bIsBusy);
	void UpdateHeardSomething(bool bHeardSomething);
	void UpdateHeardLocation(const FVector& HeardLocation);
	void UpdateWarPartyRollKey(bool bIsFollower);

	UFUNCTION()
	void OnDamaged(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue);
	UFUNCTION()
	void OnHealed(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue);
	
	UFUNCTION()
	void OnNewSense(const USensorBase* Sensor, int32 Channel, const TArray<FSensedStimulus>& SensedStimulus);
	UFUNCTION()
	void OnCurrentSense(const USensorBase* Sensor, int32 Channel, const TArray<FSensedStimulus>& SensedStimulus);
	UFUNCTION()
	void OnLostSense(const USensorBase* Sensor, int32 Channel, const TArray<FSensedStimulus>& SensedStimulus);
	UFUNCTION()
	void OnForgetSense(const USensorBase* Sensor, int32 Channel, const TArray<FSensedStimulus>& SensedStimulus);

	UFUNCTION(BlueprintNativeEvent)
	void OnEnemyDiscovered();
	UFUNCTION(BlueprintNativeEvent)
	void OnEnemyLost();
	UFUNCTION()
	void OnEnemyDeath();

	TArray<UBehaviorTree*> BehaviorTreeAssets;

	AGACharacter* CharacterRef;
	AActor* CurrentEnemy;
	TArray<AActor*> Enemies;
	TArray<AActor*> Friends;
public:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* PossessedPawn) override;
};
