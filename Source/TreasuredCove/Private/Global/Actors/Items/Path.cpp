// Fill out your copyright notice in the Description page of Project Settings.


#include "Path.h"
#include "Components/SplineComponent.h"

// Sets default values
APath::APath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	HelpfulSpline =
		CreateDefaultSubobject<USplineComponent>("SplineComponent");
}

