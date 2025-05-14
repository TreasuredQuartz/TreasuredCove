// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/HexGrid/HexProgressionComponent.h"
#include "kismet/GameplayStatics.h"
#include "Global/HexGrid/HexGridManager.h"
#include "Global/HexGrid/HexGridEditor.h"

// Sets default values for this component's properties
UHexProgressionComponent::UHexProgressionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UHexProgressionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	AActor* NewActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetOwner(), GridManagerClass, GridSpawnTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, GetOwner());
	UGameplayStatics::FinishSpawningActor(NewActor, GridSpawnTransform);
	GridManager = Cast<AHexGridManager>(NewActor);

	GridEditor = NewObject<UHexGridEditor>();
	GridEditor->HexManager = GridManager;
}


// Called every frame
void UHexProgressionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHexProgressionComponent::StartEditing()
{
	if (GridEditor) GridEditor->StartEditing();
}

void UHexProgressionComponent::StopEditing()
{
	if (GridEditor) GridEditor->StopEditing();
}

