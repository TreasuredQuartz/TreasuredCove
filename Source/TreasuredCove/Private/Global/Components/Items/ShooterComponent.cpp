// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Components/Items/ShooterComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UShooterComponent::UShooterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UShooterComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UShooterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UShooterComponent::FireAtTarget(AActor* TargetActor, float Accuracy)
{
	UWorld* World = GetWorld();
	if (World == nullptr || TargetActor == nullptr) return;

	FHitResult Hit;
	{
		const FVector& Start = GetOwner()->GetActorLocation();
		const FVector& TargetLocation = TargetActor->GetActorLocation();
		const FVector& TargetDirection = TargetLocation - Start;
		const FVector& EndDelta = GetVectorWithVarience(TargetDirection, GetRandomVarience(Accuracy));
		const FVector& End = TargetLocation + EndDelta;
		ECollisionChannel Channel = ECC_Camera;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetOwner());
		World->LineTraceSingleByChannel(Hit, Start, End, Channel, Params);
	}

	if (Hit.bBlockingHit)
	{

	}
}

float UShooterComponent::GetCurrentVarience() const
{
	// GetDiscipline();
	return Control;
}

FVector2D UShooterComponent::GetRandomVarience(float Varience) const
{
	float x = FMath::RandRange(-Varience, Varience);
	float y = FMath::RandRange(-Varience, Varience);

	return FVector2D(x, y);
}

FVector UShooterComponent::GetVectorWithVarience(const FVector& Normal, const FVector2D& Varience) const
{
	FVector XAxis;
	FVector YAxis;
	Normal.FindBestAxisVectors(XAxis, YAxis);

	return (XAxis * Varience.X)
		 + (YAxis * Varience.Y);
}

FVector2D UShooterComponent::GetBulletVarience()
{
	FVector2D BulletVarience;
	SpreadPattern.Dequeue(BulletVarience);
	return BulletVarience;
}

FVector UShooterComponent::GetFinalLocation(const FVector& TargetLocation)
{
	const FVector& Start = GetOwner()->GetActorLocation();
	const FVector& TargetDirection = TargetLocation - Start;
	const FVector2D& Varience = GetRandomVarience(GetCurrentVarience());
	const FVector2D& BulletVarience = GetBulletVarience();
	const FVector& CurrentVarience = GetVectorWithVarience(TargetDirection, Varience) + GetVectorWithVarience(TargetDirection, BulletVarience);

	return TargetLocation +
		CurrentVarience;
}

