// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Actors/FootprintActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "SensorBase.h"
#include "SenseStimulusComponent.h"
#include "Components/AudioComponent.h"

// Sets default values
AFootprintActor::AFootprintActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root Component
	SceneRoot =
		CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));

	// Static Mesh Component
	FootprintMesh =
		CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Footprint Mesh"));

	// Sense Stimulus Component
	SenseStimulus =
		CreateDefaultSubobject<USenseStimulusComponent>(TEXT("Sense Stimulus"));

	Audio = 
		CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));

	SetRootComponent(SceneRoot);
	FootprintMesh->SetupAttachment(SceneRoot);
}

// Called when the game starts or when spawned
void AFootprintActor::BeginPlay()
{
	Super::BeginPlay();

	if (SenseStimulus && HasAuthority())
	{
		SenseStimulus->OnSensedFromSensor.AddUniqueDynamic(this, &AFootprintActor::OnSensed);
		SenseStimulus->ReportSenseEvent(FName("SensorHearing"));
	}
	
	if (!(FootprintSurfaceData.IsValidIndex(0) && FootprintMasks.IsValidIndex(FootprintType))) 
		return;

	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(FootprintSurfaceData[0]->FootprintSurface, this);
	MID->SetTextureParameterValue(FName("Texture"), FootprintMasks[FootprintType]);
	FootprintMesh->SetMaterial(0, MID);
	Audio->SetSound(FootprintSurfaceData[0]->GetRandomSurfaceSound());
	Audio->Play();
}

// Called every frame
void AFootprintActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// SenseStimulus->SetScore(FName("SensorHearing"), GetLifeSpan() / InitialLifeSpan);
}

void AFootprintActor::SetSurfaceType(int PhysSurfaceType)
{
	if (SurfaceType != PhysSurfaceType)
	{
		SurfaceType = PhysSurfaceType;
		OnSurfaceTypeChanged();
	}
}

void AFootprintActor::SetFootprintType(int NewFootprintType)
{
	if (FootprintType != NewFootprintType)
	{
		FootprintType = NewFootprintType;
	}
}

void AFootprintActor::OnSensed(const USensorBase* Sensor, int32 Channel, EOnSenseEvent SenseEvent)
{
	if (!Sensor || !Sensor->GetSensorOwner()) 
		return;

	APawn* Pawn = GetOwner<APawn>();

	if (Sensor->GetSensorOwner() == GetOwner() || Pawn->GetController() == UGameplayStatics::GetPlayerController(this, 0))
	{
		// UE_LOG(LogTemp, Warning, TEXT("We hear ourselves!"));
		return;
	}

	switch (SenseEvent)
	{
	case EOnSenseEvent::SenseNew:
		if (Sensor->SensorTag.IsEqual(FName("SensorHearing")))
		{
			OnHeard();
		}
		else if (Sensor->SensorTag.IsEqual(FName("SensorSight")))
		{
			OnSeen();
		}

		break;
	case EOnSenseEvent::SenseCurrent:
		break;
	case EOnSenseEvent::SenseLost:
		break;
	case EOnSenseEvent::SenseForget:
		break;
	}
}

void AFootprintActor::OnSurfaceTypeChanged_Implementation()
{

}

void AFootprintActor::OnHeard()
{
	OnHeard_BP();
}

void AFootprintActor::OnSeen_Implementation()
{

}

USoundBase* UFootprintSurfaceConfigData::GetRandomSurfaceSound() const
{
	const uint8 Min = 0;
	const uint8 Max = FootprintSurfaceSounds.Num() - 1;

	return FootprintSurfaceSounds[UKismetMathLibrary::RandomIntegerInRange(Min, Max)];
}
