// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TownSystem/GameTimeInterface.h"
#include "GAGameState.generated.h"

class UStaticMesh;
class UGameplayClockComponent;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API AGAGameState 
	: public AGameStateBase,
	  public IGameTimeInterface
{
	GENERATED_BODY()

public:
	AGAGameState();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UGameplayClockComponent* GameClock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules")
	TMap<UStaticMesh*, TSubclassOf<AActor>> CurrentStaticToDynamicMeshes;

public:
	virtual float GetTimeOfDay_Implementation() override;
	virtual void UpdateGameSpeed_Implementation(float SpeedMultiplier) override;
};
