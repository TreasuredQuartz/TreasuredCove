// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupMagnetComponent.h"
#include "RepositoryComponent.h"
#include "InteractionInterface.h"
#include "RepositoryInterface.h"
#include "Components/SphereComponent.h"
#include "Gameframework/MovementComponent.h"
#include "Engine/Engine.h"
#include "Interfaces/PickupInterface.h"

// Sets default values for this component's properties
UPickupMagnetComponent::UPickupMagnetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	PickupRadius = 100;

	// ...
	PickupMagnetOverlap =
		CreateDefaultSubobject<USphereComponent>(TEXT("Magnet Collision"));
	PickupMagnetOverlap->OnComponentBeginOverlap.AddDynamic(this, &UPickupMagnetComponent::OnBeginOverlap);

	if (GetOwner())
	{
		if (!GetOwner()->GetRootComponent())
		{
			GetOwner()->SetRootComponent(this);
			PickupMagnetOverlap->SetupAttachment(GetOwner()->GetRootComponent());
		}
		else 
		{
			PickupMagnetOverlap->SetupAttachment(GetOwner()->GetRootComponent());
		}
	}
}

// Called when the game starts
void UPickupMagnetComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	PickupMagnetOverlap->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	PickupMagnetOverlap->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupMagnetOverlap->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Overlap);
}

#if WITH_EDITOR
void UPickupMagnetComponent::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;

	//if (PropertyName == GET_MEMBER_NAME_CHECKED(UCustomClass, PropertyName))
	//{
	//	// Do Stuff to Property
	//}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UPickupMagnetComponent, PickupRadius))
	{
		PickupMagnetOverlap->SetSphereRadius(PickupRadius);
	}

	Super::PostEditChangeProperty(e);
}
#endif

// Called every frame
void UPickupMagnetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// ...
	if (DeltaTime < SMALL_NUMBER) return;

	if (CurrentAnimatingPickups.Num() > 0)
	{
		for (int i = 0; i < CurrentAnimatingPickups.Num(); ++i)
		{
			if (CurrentAnimatingPickups.IsValidIndex(i))
			{
				AActor* Pickup = CurrentAnimatingPickups[i];

				if (Pickup != nullptr)
				{
					const FVector Start = Pickup->GetActorLocation();
					const FVector End = GetOwner()->GetActorLocation();
					const FVector Delta = End - Start;
					const FVector Direction = Delta.GetSafeNormal();
					const FVector::FReal Distance = Delta.Size();
					const FVector::FReal MaxStep = 500 * DeltaTime;

					if (FMath::IsNearlyZero(Distance))
					{
						CurrentAnimatingPickups.RemoveSingle(Pickup);
						IPickupInterface::Execute_OnPickedUp(Pickup, GetOwner());
						continue;
					}

					if (Distance > MaxStep)
					{
						if ( MaxStep > 0.f )
						{
							const FVector ForceDirection = FVector::CrossProduct(FVector::UpVector, Direction);
							const FVector ForwardPoint = (Start + (ForceDirection * MaxStep));
							const FVector TowardGoalPoint = (Start + (Direction  * MaxStep));
							const FVector newLocation =  Start + (((TowardGoalPoint - ForwardPoint)));

							Pickup->SetActorLocation(newLocation);
							continue;
						}
						else
						{
							continue;
						}
					}

					Pickup->SetActorLocation(End);
					continue;

				}
			}
		}
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPickupMagnetComponent::OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Magnet Overlap..."));

	if (OtherActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("...Other actor is valid..."));

		if (OtherActor->GetClass()->ImplementsInterface(UPickupInterface::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("...Other actor implements pickup interface..."));
			if (IPickupInterface::Execute_CanBePickedUp(OtherActor, GetOwner()))
			{
				CurrentAnimatingPickups.Add(OtherActor);
				FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);
				OtherActor->AttachToActor(GetOwner(), Rules);
				TInlineComponentArray<UActorComponent*> Components(OtherActor);
				OtherActor->GetComponents(UMovementComponent::StaticClass(), Components);
				for (UActorComponent* AC : Components)
				{
					if (UMovementComponent* MC = Cast<UMovementComponent>(AC))
					{
						MC->StopMovementImmediately();
					}
				}
			} else UE_LOG(LogTemp, Warning, TEXT("...Other actor denies pickup..."));
		}
	}
}

