// Fill out your copyright notice in the Description page of Project Settings.


#include "MaterialTrailComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values for this component's properties
UMaterialTrailComponent::UMaterialTrailComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...

	ComponentTags.Add( FName("GrassAffector") );
	ComponentTags.Add( FName("WaterAffector") );

	
}

