// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTownState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTownSpeedModified, float, TownSpeed);

UCLASS()
class GAMEPLAYTOWNS_API AGameplayTownState : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameplayTownState();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	bool bShouldCalcSpeed;

	float TownTime;
	float InitialTownTime;
	float TownSpeed;
	float DefaultTownSpeed;
	// float TownSpeedMultiplier;

	int DaysPassed;
	int Milliseconds;
	int Seconds;
	int Minutes;
	int Hours;
	int Days;
	int Months;
	int Years;
	int Centuries;
	int Eras;

	TArray<int> TownDate;
	TMap<FName, float> Resources;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitDate();

	void SetClock();
	void SetCalendar();
	void EventUpdateTownSpeed(float SpeedMultiplier);

	FOnTownSpeedModified OnTownSpeedModified;
};
