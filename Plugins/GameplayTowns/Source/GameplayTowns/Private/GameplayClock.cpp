// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayClock.h"
#include "Library/GameplayLibrary.h"
#include "Misc/DateTime.h"
#include "Engine/Engine.h"

// Sets default values
UGameplayClockComponent::UGameplayClockComponent()
	: bShouldCalcSpeed(true)
	, GameTime(0)
	, GameSpeed(150)
	, DefaultGameSpeed(0.05)
	, DayCounter(0)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	for (uint8 i = 0; (ETimeType)i < ETimeType::Num; ++i)
	{
		switch ((ETimeType)i)
		{
		case ETimeType::Milliseconds:
		case ETimeType::Seconds:
		case ETimeType::Minutes:
			GameDate[i] = 0;
			break;
		case ETimeType::Hours:
			GameDate[i] = 12;
			break;
		case ETimeType::Days:
		case ETimeType::Months:
			GameDate[i] = 1;
			break;
		case ETimeType::Years:
		case ETimeType::Centuries:
		case ETimeType::Eras:
			GameDate[i] = 0;
			break;
		default:
			GameDate[i] = 0;
			break;
		}
	}
}

// Called when the game starts or when spawned
void UGameplayClockComponent::BeginPlay()
{
	Super::BeginPlay();

	//if(GEngine)
		//GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Black, FString::FromInt(Seconds));
	
	InitialGameTime = GetSolarTime();
	GameTime = InitialGameTime;
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
	float Val = (GameDate[(uint8)ETimeType::Hours] + (GameDate[(uint8)ETimeType::Minutes] * 0.016f) + (GameDate[(uint8)ETimeType::Seconds] * 0.00027f) + (GameDate[(uint8)ETimeType::Milliseconds] * 0.0000027f));
	return Val;
}

void UGameplayClockComponent::SetClock()
{
	float Val = GameTime * 360000;
	GameDate[(uint8)ETimeType::Milliseconds] = uint8(fmodf(Val, 100));
	Val = Val / 100;
	GameDate[(uint8)ETimeType::Seconds] = uint8(fmodf(Val, 60));
	Val = Val / 60;
	GameDate[(uint8)ETimeType::Minutes] = uint8(fmodf(Val, 60));
	Val = Val / 60;
	GameDate[(uint8)ETimeType::Hours] = uint8(fmodf(Val, 24));
}

void UGameplayClockComponent::SetCalendar()
{
	GameDate[(uint8)ETimeType::Days] += DayCounter;
	if (GameDate[(uint8)ETimeType::Days] > FDateTime::DaysInMonth(GameDate[(uint8)ETimeType::Years], GameDate[(uint8)ETimeType::Months]) )
	{
		GameDate[(uint8)ETimeType::Days] = 1;
		GameDate[(uint8)ETimeType::Months] += 1;
		if (GameDate[(uint8)ETimeType::Months] > 12)
		{
			GameDate[(uint8)ETimeType::Months] = 1;
			GameDate[(uint8)ETimeType::Years] += 1;
		}
	}
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
