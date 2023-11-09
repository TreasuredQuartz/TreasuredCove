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
		CreateDefaultSubobject<USphereComponent>(TEXT("Magnet Pickup Collision"));
	PickupMagnetOverlap->SetSphereRadius(PickupRadius);
	PickupMagnetOverlap->OnComponentBeginOverlap.AddDynamic(this, &UPickupMagnetComponent::OnBeginOverlap);
}

void UPickupMagnetComponent::OnRegister()
{
	Super::OnRegister();

	if (!PickupMagnetOverlap) return;

	if (GetOwner())
	{
		if (!GetOwner()->GetRootComponent())
		{
			GetOwner()->SetRootComponent(this);
			PickupMagnetOverlap->SetupAttachment(GetOwner()->GetRootComponent());
		}
		else
		{
			this->SetupAttachment(GetOwner()->GetRootComponent());
			PickupMagnetOverlap->SetupAttachment(this);
		}
	}
	else
	{
		PickupMagnetOverlap->SetupAttachment(this);
	}

	if (GetWorld()) PickupMagnetOverlap->RegisterComponent();
}

// Called when the game starts
void UPickupMagnetComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	PickupMagnetOverlap->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	PickupMagnetOverlap->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupMagnetOverlap->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Overlap);

	PickupMagnetOverlap->SetSphereRadius(PickupRadius);
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

					if (FMath::IsNearlyZero(Distance) || Delta.IsNearlyZero())
					{
						IPickupInterface::Execute_OnPickedUp(Pickup, GetOwner());
						CurrentAnimatingPickups.RemoveSingle(Pickup);
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

void UPickupMagnetComponent::SetPickupRadius(float NewRadius)
{
	PickupRadius = NewRadius;
	PickupMagnetOverlap->SetSphereRadius(NewRadius);
}

void UPickupMagnetComponent::OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (OtherActor->GetClass()->ImplementsInterface(UPickupInterface::StaticClass()))
		{
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
						MC->SetComponentTickEnabled(false);
						MC->Deactivate();
					}
					else if (UPrimitiveComponent* PC = Cast<UPrimitiveComponent>(AC))
					{
						if (PC->IsSimulatingPhysics())
						{
							PC->SetSimulatePhysics(false);
						}
					}
				}
			}
		}
	}
}

