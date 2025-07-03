// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Components/FirstPersonComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values for this component's properties
UFirstPersonComponent::UFirstPersonComponent()
	: bFirstPerson(false)
	, bLockedViewpoint(false)
	, bDetectedCrouch(false)
	, ThirdPersonArmLength(300.f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UFirstPersonComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	ChangeViewpoint(bFirstPerson);
}

// Called every frame
void UFirstPersonComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bFirstPerson || bLockedViewpoint)
	{
		// Owner has to be valid.
		APawn* LocalOwner = GetOwner<APawn>();
		if (!LocalOwner) return;

		// We only run on locally controlled machines.
		if (LocalOwner->GetLocalRole() <= ROLE_SimulatedProxy) return;

		// Movement has to be valid.
		UPawnMovementComponent* LocalMove = LocalOwner->GetMovementComponent();
		if (!LocalMove) return;

		// Movement has to be on the ground.
		// if (!LocalMove->IsMovingOnGround()) return;

		USkeletalMeshComponent* MeshComp = LocalOwner->GetComponentByClass<USkeletalMeshComponent>();
		CheckCrouching(LocalMove, MeshComp);
		// if (!bDetectedCrouch) return;

		// We have to be moving.
		// FVector LocalVelocity = LocalOwner->GetVelocity();
		// if (LocalVelocity.Size() <= 0) return;

		// We have to be moving forward.
		// if (LocalVelocity.ForwardVector.X > 0) return;

		// 
		LocalOwner->bUseControllerRotationYaw = true;
	}
	else
	{
		GetOwner<APawn>()->bUseControllerRotationYaw = bFirstPerson;
	}
}

void UFirstPersonComponent::OnRegister()
{
	Super::OnRegister();

	OwnerMeshComponent =
		GetOwner()->GetComponentByClass<USkeletalMeshComponent>();

	SpringArmComponent =
		GetOwner()->GetComponentByClass<USpringArmComponent>();
}

void UFirstPersonComponent::OnUnregister()
{
	Super::OnUnregister();

	OwnerMeshComponent = nullptr;
	SpringArmComponent = nullptr;
}

void UFirstPersonComponent::ChangeViewpoint(bool bInFirstPerson)
{
	bFirstPerson = bInFirstPerson;

	if (GetOwner<APawn>()->IsLocallyControlled()/* && GetOwner()->GetLocalRole() > ROLE_SimulatedProxy */)
	{
		OwnerMeshComponent->SetSkeletalMesh(
			bFirstPerson ? FirstPersonMesh : ThirdPersonMesh
		);

		OwnerMeshComponent->SetRelativeRotation(
			bFirstPerson ? FRotator::ZeroRotator : FRotator(0, -90, 0)
		); // */

		SpringArmComponent->TargetArmLength = 
			bFirstPerson ? 0 : ThirdPersonArmLength;

		OwnerMeshComponent->SetAnimInstanceClass(
			bFirstPerson ? FirstPersonAnimClass : ThirdPersonAnimClass
		);
	}
}

void UFirstPersonComponent::CheckCrouching(const UPawnMovementComponent* InMoveComp, USkeletalMeshComponent* InMeshComp)
{
	if (!InMeshComp || !InMoveComp) return;

	if (InMoveComp->IsCrouching())
	{
		if (!bDetectedCrouch)
		{
			FVector MeshRelativeLocation = InMeshComp->GetRelativeLocation();
			MeshRelativeLocation.Z -= 45;
			InMeshComp->SetRelativeLocation(MeshRelativeLocation);
		}

		bDetectedCrouch = true;
	}
	else
	{
		bDetectedCrouch = false;
	}
}

