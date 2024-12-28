// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Actors/Items/PathFollower.h"
#include "Global/Actors/Items/Path.h"

#include "Components/TimelineComponent.h"
#include "Components/SplineComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
APathFollower::APathFollower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("/Game/GameplayAbilities/DataTables/FloatLinear_0-1"));
	check(Curve.Succeeded());
	FloatCurve = Curve.Object;
	if (FloatCurve == NULL) 
	{ 
		UE_LOG(LogTemp, Warning, TEXT("Float curve is null.")); 
		return;
	}
	
	PathTimeline =
		CreateDefaultSubobject<UTimelineComponent>(FName("TimelinePathAnimation"));
	FOnTimelineFloat OnTimelineCallback;
	FOnTimelineEventStatic OnTimelineFinishedCallback;
	/*PathTimeline->CreationMethod =
		EComponentCreationMethod::Native;
	this->BlueprintCreatedComponents.Add(PathTimeline);
	PathTimeline->SetNetAddressable();
	PathTimeline->SetPropertySetObject(this);*/
	PathTimeline->SetDirectionPropertyName(FName("TimelineDirection"));
	PathTimeline->SetLooping(false);
	PathTimeline->SetTimelineLength(5.0f);
	PathTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
	PathTimeline->SetPlaybackPosition(0.0f, false);
	OnTimelineCallback.BindUFunction(this, TEXT("TimelineCallback"));
	OnTimelineFinishedCallback.BindUFunction(this, TEXT("TimelineFinishedCallback"));
	PathTimeline->AddInterpFloat(FloatCurve, OnTimelineCallback);
	PathTimeline->SetTimelineFinishedFunc(OnTimelineFinishedCallback);
}

void APathFollower::BeginPlay()
{
	Super::BeginPlay();
}

void APathFollower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PathTimeline != NULL)
	{
		PathTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
	}
}

void APathFollower::TimelineCallback(float InterpolatedValue)
{
	if (!FollowerActor) return;

	Alpha = InterpolatedValue;
	const FVector ActorCurrentLocation = FollowerActor->GetActorLocation();
	const FRotator ActorCurrentRotation = FollowerActor->GetActorRotation();
	const float Lerp = UKismetMathLibrary::Lerp(0, PathToFollow->HelpfulSpline->GetSplineLength(), Alpha);
	const FVector SplineCurrentLocation = PathToFollow->HelpfulSpline->GetLocationAtDistanceAlongSpline(Lerp, ESplineCoordinateSpace::World);
	const FRotator SplineCurrentRotation = PathToFollow->HelpfulSpline->GetRotationAtDistanceAlongSpline(Lerp, ESplineCoordinateSpace::World);

	const FVector FinalLocation = (SplineCurrentLocation); //+ ActorCurrentLocation);

	const FRotator FinalRotation = SplineCurrentRotation;

	FollowerActor->SetActorLocationAndRotation(FinalLocation, FinalRotation);
}

void APathFollower::TimelineFinishedCallback()
{
	UE_LOG(LogTemp, Warning, TEXT("Timeline finished playback."));
	if (OnPathFinished.IsBound()) UE_LOG(LogTemp, Warning, TEXT("Delegate Is bound."));
	OnPathFinished.Broadcast();
	
}

void APathFollower::PlayTimeline()
{
	if (PathTimeline != NULL)
	{
		PathTimeline->Play();
	}
}

void APathFollower::MoveAlongPath()
{
	if (!PathTimeline) return;
	PathTimeline->SetPlayRate(1 / Duration);
	PathTimeline->SetPlaybackPosition(StartOffset, true);
	PlayTimeline();
}
