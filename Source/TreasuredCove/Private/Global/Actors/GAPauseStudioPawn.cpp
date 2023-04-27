// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Actors/GAPauseStudioPawn.h"

// Sets default values
AGAPauseStudioPawn::AGAPauseStudioPawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGAPauseStudioPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGAPauseStudioPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGAPauseStudioPawn::OnEnter_Implementation()
{

}

void AGAPauseStudioPawn::OnExit_Implementation()
{

}

