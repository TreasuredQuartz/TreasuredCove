// Fill out your copyright notice in the Description page of Project Settings.


#include "StarField.h"

// Sets default values
AStarField::AStarField()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStarField::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStarField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

