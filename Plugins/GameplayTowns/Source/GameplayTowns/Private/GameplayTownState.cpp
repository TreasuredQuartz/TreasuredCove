// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayTownState.h"
#include "GameplayLibrary.h"

// Sets default values
AGameplayTownState::AGameplayTownState()
	: bShouldCalcSpeed()
	, TownTime(0)
	, TownSpeed(150)
	, DefaultTownSpeed(150)
	, DaysPassed(0)
	, Milliseconds(0)
	, Seconds(0)
	, Minutes(0)
	, Hours(0)
	, Days(0)
	, Months(0)
	, Years(0)
	, Centuries(0)
	, Eras(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TownDate.SetNumUninitialized(5);
}

// Called when the game starts or when spawned
void AGameplayTownState::BeginPlay()
{
	Super::BeginPlay();
	
	InitialTownTime = Hours + (Minutes / 60  + (Seconds / 3600 + (Milliseconds / 360000)));
	TownTime = InitialTownTime;

	InitDate();

	TownSpeed = DefaultTownSpeed;
}

// Called every frame
void AGameplayTownState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldCalcSpeed)
	{
		UGameplayLibrary::SetGameTime(DeltaTime, TownSpeed, InitialTownTime, &TownTime, &DaysPassed);

	}

}

void AGameplayTownState::InitDate()
{
	TownDate[0] = Days;
	TownDate[1] = Months;
	TownDate[2] = Years;
	TownDate[3] = Centuries;
	TownDate[4] = Eras;
}

void AGameplayTownState::SetClock()
{
	float Val = TownTime * 360000;
	Milliseconds = floor(fmodf(Val, 100));
	Val = Val / 100;
	Seconds = floor(fmodf(Val, 60));
	Val = Val / 60;
	Minutes = floor(fmodf(Val, 60));
	Val = Val / 60;
	Hours = floor(fmodf(Val, 24));

	InitDate();
}

// Multiplies town speed by input. If TownSpeed is 0, instead sets TownSpeed to input.
void AGameplayTownState::EventUpdateTownSpeed(float SpeedMultiplier)
{
	if (TownSpeed == 0 && SpeedMultiplier != 0)
	{
		TownSpeed = 1;
	}

	TownSpeed = SpeedMultiplier * TownSpeed;

	OnTownSpeedModified.Broadcast(TownSpeed);
}
