// Fill out your copyright notice in the Description page of Project Settings.


#include "GAVehicle.h"
#include "GASystemComponent.h"
#include "GACharacter.h"
#include "VehicleMovementComponent.h"

#include "GameFramework/Controller.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

// Sets default values
AGAVehicle::AGAVehicle()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//
	RootComponent =
		CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// 
	Collision =
		CreateDefaultSubobject<UBoxComponent>("Collision");
	Collision->SetupAttachment(RootComponent);

	//
	AbilitySystem =
		CreateDefaultSubobject<UGASystemComponent>(TEXT("Ability System"));

	//
	//Movement =
	//	CreateDefaultSubobject<UVehicleMovementComponent>("Movement");
	//Movement->UpdatedComponent = Mesh;

	MaxPassengers = 0;
}

UAbilitySystemComponent* AGAVehicle::GetAbilitySystemComponent() const
{
	return Cast<UAbilitySystemComponent>(AbilitySystem);
}

UPawnMovementComponent* AGAVehicle::GetMovementComponent() const
{
	return Cast<UPawnMovementComponent>(Movement);
}

void AGAVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Primary", IE_Pressed, this, &AGAVehicle::PrimaryPressed);
	PlayerInputComponent->BindAction("Primary", IE_Released, this, &AGAVehicle::PrimaryReleased);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AGAVehicle::Throttle);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AGAVehicle::Exit);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGAVehicle::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGAVehicle::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AGAVehicle::LookUp);
	PlayerInputComponent->BindAxis("LookRight", this, &AGAVehicle::LookRight);
}

void AGAVehicle::InteractedWith_Implementation(AActor* OtherActor)
{
	AGACharacter* Character = Cast<AGACharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	int32 Index = 0;
	bool bIsPassenger = false;

	for (AGACharacter* Passenger : Passengers)
	{
		if (Passenger == Character)
		{
			bIsPassenger = true;

			if (bIsPassenger)
			{
				Index = Passengers.Find(Character);
			}
		}
	}

	if (!bIsPassenger)
	{
		// Some Pawn wants in
		if (Driver)
		{
			// We already have a driver
			Ride(Character);
		}
		else
		{
			// We do not have a driver
			Drive(Character);
		}
	}
	else
	{
		ExitPassenger(Index);
	}
}

void AGAVehicle::Drive(AGACharacter* Character)
{
	AController* C = Character->GetController();
	C->UnPossess();
	C->Possess(this);

	Driver = Character;

	if (VehicleEnterMontages[0] && CharacterEnterMontages[0])
	{
		Character->PlayAnimMontage(CharacterEnterMontages[0]);
		// Mesh->GetAnimInstance()->Montage_Play(VehicleEnterMontages[0]);
	}
}

void AGAVehicle::Ride(AGACharacter* Character)
{
	if (Passengers.Num() < MaxPassengers)
	{

		if (VehicleEnterMontages[Passengers.Num() + 1] && CharacterEnterMontages[Passengers.Num() + 1])
		{
			Character->PlayAnimMontage(CharacterEnterMontages[Passengers.Num() + 1]);
			// Mesh->GetAnimInstance()->Montage_Play(VehicleEnterMontages[Passengers.Num() + 1]);

			Passengers.Add(Character);
		}
	}
}

void AGAVehicle::ExitPassenger(int32 Index)
{
	if (CharacterExitMontages[Index])
	{
		Passengers[Index]->PlayAnimMontage(CharacterExitMontages[Index]);
		// Mesh->GetAnimInstance()->Montage_Play(VehicleExitMontages[Index]);

		Passengers.RemoveAt(Index);
	}
}

void AGAVehicle::Exit()
{
	AController* C = GetController();
	C->UnPossess();
	C->Possess(Driver);

	// 
	if (VehicleExitMontages[0])
	{
		Driver->PlayAnimMontage(CharacterExitMontages[0]);
		// Mesh->GetAnimInstance()->Montage_Play(VehicleExitMontages[0]);
	}
}

void AGAVehicle::PrimaryPressed_Implementation()
{
}

void AGAVehicle::PrimaryReleased_Implementation()
{
}

void AGAVehicle::SecondaryPressed_Implementation()
{
}

void AGAVehicle::SecondaryReleased_Implementation()
{
}

void AGAVehicle::TertiaryPressed_Implementation()
{
}

void AGAVehicle::TertiaryReleased_Implementation()
{
}

void AGAVehicle::MoveForward_Implementation(float AxisValue)
{
	if (AxisValue != 0)
	{
		// Our control rotation
		FRotator Rotation = GetActorRotation();

		// This essentially gets from our control rotation a Up Vector
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Z);

		// We want to move in this direction
		AddMovementInput(Direction, AxisValue);
	}
}

void AGAVehicle::MoveRight_Implementation(float AxisValue)
{
	if (AxisValue != 0)
	{
		// Our rotation
		FRotator Rotation = GetActorRotation();

		// This essentially gets from our control rotation a Right Vector
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);

		// We want to move in this direction
		AddMovementInput(Direction, AxisValue);
	}
}

void AGAVehicle::LookUp_Implementation(float AxisValue)
{
	if (AxisValue != 0)
	{
		AddControllerPitchInput(AxisValue);
	}
}

void AGAVehicle::LookRight_Implementation(float AxisValue)
{
	if (AxisValue != 0)
	{
		AddControllerYawInput(AxisValue);
	}
}

void AGAVehicle::Throttle_Implementation()
{
	
}

void AGAVehicle::Brake_Implementation()
{
	
}
