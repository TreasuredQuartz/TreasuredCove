// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameTimeInterface.h"
#include "GameplayClock.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSpeedModified, float, GameSpeed);

UENUM(BlueprintType)
enum class ETimeType : uint8
{
	Milliseconds,
	Seconds,
	Minutes,
	Hours,
	Days,
	Months,
	Years,
	Decades,
	Centuries,
	Eras,
	Num UMETA(Hidden)
};

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

	UPROPERTY(EditDefaultsOnly, EditFixedSize, meta = (EditFixedOrder, ArraySizeEnum = "ETimeType"))
	int GameDate[ETimeType::Num];

public:
	float GetSolarTime();
	void InitDate();

	void SetClock();
	void SetCalendar();
	virtual void UpdateGameSpeed_Implementation(float SpeedMultiplier) override;

	FOnGameSpeedModified OnGameSpeedModified;
};
