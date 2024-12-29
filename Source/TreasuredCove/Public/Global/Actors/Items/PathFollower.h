// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathFollower.generated.h"

// Delegates accept the same specifiers as UFUNCTIONs
// UFUNCTION(BlueprintImplementableEvent)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPathFinishedDelegate);

class APath;
class UCurveFloat;
class UTimelineComponent;

namespace ETimelineDirection
{
	enum Type : int;
}

UCLASS()
class TREASUREDCOVE_API APathFollower : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	APathFollower();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* FollowerActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APath* PathToFollow;

	float Alpha;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PathDeviationAmount = 50.0f;

	UFUNCTION(BlueprintCallable)
	void MoveAlongPath();

	UPROPERTY(BlueprintAssignable)
	FOnPathFinishedDelegate OnPathFinished;

	UFUNCTION()
	void TimelineFinishedCallback();
	UFUNCTION()
	void TimelineCallback(float InterpolatedValue);
	UFUNCTION()
	void PlayTimeline();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTimelineComponent* PathTimeline;
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* FloatCurve;
	UPROPERTY()
	TEnumAsByte<ETimelineDirection::Type> TimelineDirection;
};
