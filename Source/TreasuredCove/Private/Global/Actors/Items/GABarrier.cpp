// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Actors/Items/GABarrier.h"
#include "Global/Actors/Items/GAProjectile.h"
#include "Global/Actors/GACharacter.h"

#include "Components/BoxComponent.h"

// Sets default values
AGABarrier::AGABarrier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Scene =
		CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	RootComponent = Scene;

	// 
	CollisionComp =
		CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionComp->
		OnComponentBeginOverlap.AddDynamic(this, &AGABarrier::OnOverlapped);
	CollisionComp->SetupAttachment(RootComponent);
}

void AGABarrier::OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGAProjectile* Projectile = Cast<AGAProjectile>(OtherActor);

	if (Projectile)
	{

	}
	else
	{
		AGACharacter* Character = Cast<AGACharacter>(OtherActor);

		if (Character)
		{

		}
	}
}
