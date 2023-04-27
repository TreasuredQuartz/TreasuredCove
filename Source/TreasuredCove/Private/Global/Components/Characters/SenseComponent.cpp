// Fill out your copyright notice in the Description page of Project Settings.


#include "SenseComponent.h"
#include "GACharacter.h"

#include "Perception/PawnSensingComponent.h"

// Sets default values for this component's properties
USenseComponent::USenseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	PawnSense =
		CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSense"));

	OwningCharacter = GetOwner<AGACharacter>();
	if (GetOwner<AGACharacter>())
	{
		// PawnSense->OnSeePawn.AddDynamic(OwningCharacter, &AGACharacter::OnPawnSeen);
		// PawnSense->OnHearNoise.AddDynamic(OwningCharacter, &AGACharacter::OnNoiseHeard);
	}

	PawnSense->SetPeripheralVisionAngle(45);
	// PawnSense->RegisterComponent();
}

float USenseComponent::GetFoV()
{
	return PawnSense->GetPeripheralVisionAngle();
}
