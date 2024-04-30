// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/LaunchingComponent.h"
#include "ProjectilePathComponent.h"

// Sets default values for this component's properties
ULaunchingComponent::ULaunchingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	ProjectilePath =
		CreateDefaultSubobject<UProjectilePathComponent>(TEXT("Projectile Path Indicator"));

	bShowProjectilePath = true;
}


// Called when the game starts
void ULaunchingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void ULaunchingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (DeltaTime < SMALL_NUMBER) return;

	if (bLaunching)
	{
		switch(LaunchType)
		{
		case ELaunchType::Instant:
			Speed = Acceleration;
			break;
		case ELaunchType::BuildUp:
			Speed += Acceleration * DeltaTime;
			break;
		}

		if (AttachedActor && AttachedActor->GetInstigatorController())
		{
			const FVector& ActorVelocity = AttachedActor->GetVelocity();
			Location = AttachedActor->GetActorLocation() + Offset;

			if (ActorVelocity.Dot(FVector::RightVector) < 0.5)
				Location += FVector(0,50,0);
			else if (ActorVelocity.Dot(FVector::RightVector) > 0.5)
				Location += FVector(0,-50,0);

			Direction = (AttachedActor->GetActorForwardVector() + 
				ActorVelocity.GetSafeNormal() + 
				AttachedActor->GetInstigatorController()->GetControlRotation().Vector()
				).GetSafeNormal();

			Direction = Direction.RotateAngleAxis(-45, AttachedActor->GetActorRightVector());

			switch (LaunchType)
			{
			case ELaunchType::Instant:
				Speed = Acceleration + 
					(100 * 
						(1 -
							((ActorVelocity.Dot(FVector::BackwardVector) + 1) / 2)
							)
						);
				break;
			}
		}

		if (bShowProjectilePath)
		{
			ProjectilePath->UpdatePrediction(Location, Direction * Speed);
		}

		if (bShowProjectileAreaOfEffect)
		{

		}
	}
}

void ULaunchingComponent::BeginLaunch(const FVector& InLocation, const FVector& InDirection, float InAcceleration, ELaunchType InLaunchType)
{
	AttachedActor = nullptr;
	Location = InLocation;
	Direction = InDirection;
	Acceleration = InAcceleration;
	LaunchType = InLaunchType;

	if (bShowProjectilePath) ProjectilePath->BeginPrediction_Client(Location, Direction * Speed);
	bLaunching = true;
}

void ULaunchingComponent::BeginLaunch_Attached(AActor* InAttachActor, const FVector& OffsetLocation, float InAcceleration, ELaunchType InLaunchType)
{
	if (!InAttachActor) return;

	Location = InAttachActor->GetActorLocation();
	Offset = OffsetLocation;
	AttachedActor = InAttachActor;
	Direction = InAttachActor->GetActorForwardVector().RotateAngleAxis(-45, InAttachActor->GetActorRightVector());
	Acceleration = InAcceleration;
	LaunchType = InLaunchType;

	if (bShowProjectilePath) ProjectilePath->BeginPrediction_Client(Location, Direction * Speed);
	bLaunching = true;
}

void ULaunchingComponent::EndLaunch(FVector& LaunchVelocity)
{
	if (bShowProjectilePath) ProjectilePath->StopPrediction();

	LaunchVelocity = Direction * Speed;
	bLaunching = false;
	
	ResetLaunch();
}

void ULaunchingComponent::ResetLaunch()
{
	AttachedActor = nullptr;
	Speed = 0;
	Acceleration = 0;
	Location = FVector::ZeroVector;
	Direction = FVector::ZeroVector;
}

void ULaunchingComponent::SetLaunchType(ELaunchType InLaunchType)
{
	LaunchType = InLaunchType;
}

