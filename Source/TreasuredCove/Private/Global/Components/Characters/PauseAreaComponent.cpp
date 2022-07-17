// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseAreaComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "ProceduralMiniMap.h"

#include "GameFramework/Character.h"

// Sets default values for this component's properties
UPauseAreaComponent::UPauseAreaComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	SceneCapture = 
		CreateDefaultSubobject<USceneCaptureComponent2D>("Scene Capture");
}


// Called when the game starts
void UPauseAreaComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	// Everything not needing our owner to be valid goes before here...
	if (!GetOwner()) return;
	// After this we know our owner is valid

	// Everything that does not cast our owner goes before here...
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character) return;
	// Our Owner succeeded the ACharacter cast

	// Final check to ensure that our owner's mesh exists and has a valid mesh...
	if (Character->GetMesh() && Character->GetMesh()->SkeletalMesh)
		CharacterMesh = (Character->GetMesh());
}
