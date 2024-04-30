// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Components/Items/AimOffsetComponent.h"
#include "Components/TimelineComponent.h"

// Sets default values for this component's properties
UAimOffsetComponent::UAimOffsetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	AimOffsetTimeline = 
		CreateDefaultSubobject<UTimelineComponent>(TEXT("Aim Offset Timeline"));
}

// Called when the game starts
void UAimOffsetComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (AimOffsetTimelineCurve)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindDynamic(this, &UAimOffsetComponent::UpdateAimOffsetTimeline);
		AimOffsetTimeline->AddInterpFloat(AimOffsetTimelineCurve, TimelineCallback);

		AimOffsetTimeline->SetLooping(false);
		AimOffsetTimeline->SetTimelineLength(1.f);
		AimOffsetTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
		AimOffsetTimeline->SetPlaybackPosition(0.f, false);
	}
}

// Called every frame
void UAimOffsetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (AimOffsetTimeline)
	{
		if (AimOffsetTimeline->IsPlaying())
		{
			AimOffsetTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
		}
		else if (AimOffsetTimeline->GetPlaybackPosition() == 0.f)
		{

		}
	}
}

void UAimOffsetComponent::StartAimOffset()
{
	AimOffsetTimeline->Play();
	AimOffsetTimeline->SetPlayRate(1/Speed);
}

void UAimOffsetComponent::UpdateAimOffsetTimeline(float progress)
{
	Alpha = progress;
}

void UAimOffsetComponent::EndAimOffset()
{
	if (AimOffsetTimeline->IsPlaying())
	{
		AimOffsetTimeline->Reverse();
	}
	else
	{
		AimOffsetTimeline->ReverseFromEnd();
	}
}

