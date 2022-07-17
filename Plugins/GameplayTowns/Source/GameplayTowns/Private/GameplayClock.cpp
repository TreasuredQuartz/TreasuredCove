// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayClock.h"
#include "GameplayLibrary.h"
#include "Misc/DateTime.h"
#include "Engine/Engine.h"

// Sets default values
UGameplayClockComponent::UGameplayClockComponent()
	: bShouldCalcSpeed(true)
	, GameTime(0)
	, GameSpeed(150)
	, DefaultGameSpeed(0.05)
	, DayCounter(0)
	, Milliseconds(0)
	, Seconds(0)
	, Minutes(0)
	, Hours(16)
	, Days(0)
	, Months(1)
	, Years(0)
	, Centuries(0)
	, Eras(0)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	GameDate.SetNumUninitialized(5);
}

// Called when the game starts or when spawned
void UGameplayClockComponent::BeginPlay()
{
	Super::BeginPlay();

	//if(GEngine)
		//GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Black, FString::FromInt(Seconds));
	
	InitialGameTime = GetSolarTime();
	GameTime = InitialGameTime;

	InitDate();

	GameSpeed = DefaultGameSpeed;
}

// Called every frame
void UGameplayClockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	bShouldCalcSpeed = true;
	if (bShouldCalcSpeed)
	{ 
		UGameplayLibrary::SetGameTime(DeltaTime, GameSpeed, GameTime, &GameTime, &DayCounter);

		SetClock();
		SetCalendar();
	}
}

float UGameplayClockComponent::GetSolarTime()
{
	float Val = (Hours + (Minutes * 0.016f) + (Seconds * 0.00027f) + (Milliseconds * 0.0000027f));
	return Val;
}

void UGameplayClockComponent::InitDate()
{
	GameDate[0] = Days;
	GameDate[1] = Months;
	GameDate[2] = Years;
	GameDate[3] = Centuries;
	GameDate[4] = Eras;
}

void UGameplayClockComponent::SetClock()
{
	float Val = GameTime * 360000;
	Milliseconds = floorf(fmodf(Val, 100));
	Val = Val / 100;
	Seconds = floorf(fmodf(Val, 60));
	Val = Val / 60;
	Minutes = floorf(fmodf(Val, 60));
	Val = Val / 60;
	Hours = floorf(fmodf(Val, 24));
}

void UGameplayClockComponent::SetCalendar()
{
	Days = Days + DayCounter;
	if ( Days > FDateTime::DaysInMonth(Years, Months) )
	{
		Days = 1;
		Months += 1;
		if (Months > 12)
		{
			Months = 1;
			Years += 1;
		}
	}

	InitDate();
}

// Multiplies Game speed by input. If TownSpeed is 0, instead sets TownSpeed to input.
void UGameplayClockComponent::UpdateGameSpeed_Implementation(float SpeedMultiplier)
{
	if (GameSpeed == 0 && SpeedMultiplier != 0)
	{
		GameSpeed = 1;
	}

	GameSpeed = SpeedMultiplier * GameSpeed;

	OnGameSpeedModified.Broadcast(GameSpeed);
}
