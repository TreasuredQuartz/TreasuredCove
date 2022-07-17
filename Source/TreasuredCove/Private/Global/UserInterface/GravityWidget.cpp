// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityWidget.h"
#include "GAWidget.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"

#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Gameframework/RotatingMovementComponent.h"

// Sets default values
AGravityWidget::AGravityWidget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bSelfUpdateLocation = true;
	

	USceneComponent* Scene =
		CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Scene;

	Widget =
		CreateDefaultSubobject<UWidgetComponent>("Widget");
	Widget->SetupAttachment(RootComponent);
	Widget->SetDrawAtDesiredSize(true);
	Widget->SetWidgetSpace(EWidgetSpace::World);
	Widget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Widget->SetDrawSize(FVector2D(400, 200));
	Widget->SetTwoSided(true);
	Widget->SetTickWhenOffscreen(true);
	Widget->SetWorldTransform(FTransform(Widget->GetComponentRotation(), Widget->GetComponentLocation(), FVector(1, 1, 1)));

	RotatingMovement =
		CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Item Rotation"));
	RotatingMovement->UpdatedComponent = RootComponent;

	// Physics variable initialization
	Mass = 5;
	AirResistance = 0.8;
	Gravity = FVector(0, 0, -9.8);
}

void AGravityWidget::BeginPlay()
{
	Super::BeginPlay();
}

#if WITH_EDITOR
void AGravityWidget::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
#endif

void AGravityWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddForce(Gravity * Mass);

	FVector DesiredMovementThisFrame = ConsumeMovement();
	FVector DesiredLocation;

	if (!DesiredMovementThisFrame.IsNearlyZero())
	{
		Velocity = (Velocity * AirResistance) + DesiredMovementThisFrame;
	}
	else
	{
		Velocity = (Velocity * AirResistance);
	}

	DesiredLocation = (Velocity * DeltaTime) + GetActorLocation();

	SetActorLocation(DesiredLocation);
}

void AGravityWidget::AddForce(FVector Force)
{
	StoredForce += Force;
}

FVector AGravityWidget::ConsumeMovement()
{
	FVector Value = FVector::ZeroVector;

	if (!StoredForce.IsNearlyZero())
	{
		Value = StoredForce;
		StoredForce = FVector::ZeroVector;
	}

	return Value;
}

