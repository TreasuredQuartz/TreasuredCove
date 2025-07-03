// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Components/WeaponGeneratorComponent.h"
#include "Global/Framework/RandomGunGenerator.h"
#include "Global/Actors/Items/GAWeapon.h"

// Sets default values for this component's properties
UWeaponGeneratorComponent::UWeaponGeneratorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	GunGenerator = CreateDefaultSubobject<URandomGunGenerator>(TEXT("Random Gun Generator"));
}


// Called when the game starts
// void UWeaponGeneratorComponent::BeginPlay()
// {
//		Super::BeginPlay();
//
//		// ...
// }


// Called every frame
// void UWeaponGeneratorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
// {
//		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//		// ...
// }

UWeaponGenerator::UWeaponGenerator()
{
}

AGAWeapon* UWeaponGenerator::SpawnNewWeaponActor(const FTransform& SpawnTransform)
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AGAWeapon* NewActor = GetWorld()->SpawnActor<AGAWeapon>(AGAWeapon::StaticClass(), SpawnTransform, Params);

	GenerateNewWeapon(NewActor);

	return NewActor;
}

void UWeaponGenerator::GenerateNewWeapon(AActor* NewWeapon)
{
}
