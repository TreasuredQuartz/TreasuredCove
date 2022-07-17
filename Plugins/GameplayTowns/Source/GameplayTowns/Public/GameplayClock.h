// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameTimeInterface.h"
#include "GameplayClock.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSpeedModified, float, GameSpeed);

UCLASS()
class GAMEPLAYTOWNS_API UGameplayClockComponent 
	: public UActorComponent,
	  public IGameTimeInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UGameplayClockComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	bool bShouldCalcSpeed;

	float GameTime;
	float InitialGameTime;
	float GameSpeed;
	float DefaultGameSpeed;
	// float TownSpeedMultiplier;

	int DayCounter;
	int Milliseconds;
	int Seconds;
	int Minutes;
	int Hours;
	int Days;
	int Months;
	int Years;
	int Centuries;
	int Eras;

	TArray<int> GameDate;

public:
	float GetSolarTime();
	void InitDate();

	void SetClock();
	void SetCalendar();
	virtual void UpdateGameSpeed_Implementation(float SpeedMultiplier) override;

	FOnGameSpeedModified OnGameSpeedModified;
};
