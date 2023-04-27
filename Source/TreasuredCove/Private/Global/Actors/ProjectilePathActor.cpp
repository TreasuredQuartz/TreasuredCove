// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Actors/ProjectilePathActor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
AProjectilePathActor::AProjectilePathActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root =
		CreateDefaultSubobject<USceneComponent>("Root");

	ProjectilePathSpline =
		CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));

	SetRootComponent(Root);
	ProjectilePathSpline->SetupAttachment(Root);
}

void AProjectilePathActor::SetProjectileCollisionRadius(float NewRadius)
{
	ProjectileCollisionRadius = NewRadius;
}

void AProjectilePathActor::SetLaunchVelocity(FVector NewVelocity)
{
	LaunchVelocity = NewVelocity;
}

// Called when the game starts or when spawned
void AProjectilePathActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectilePathActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DeltaTime < SMALL_NUMBER) return;

	FPredictProjectilePathParams Params;
	FPredictProjectilePathResult Result;

	Params.ActorsToIgnore.Add(GetOwner());
	Params.StartLocation = GetActorLocation();
	Params.LaunchVelocity = LaunchVelocity;
	Params.ProjectileRadius = ProjectileCollisionRadius;

	UGameplayStatics::PredictProjectilePath(this, Params, Result);

	ClearSpline();

	if (Result.PathData.Num() != ProjectilePathSpline->GetNumberOfSplinePoints())
	{
		for (int i = 0; i < Result.PathData.Num(); ++i)
		{
			ProjectilePathSpline->AddSplinePointAtIndex(Result.PathData[i].Location, i, ESplineCoordinateSpace::World);
			// DrawDebugSphere(GetWorld(), Result.PathData[i].Location, 25, 12, FColor::Green);
		}
	}

	UpdateSplineMesh();
}

void AProjectilePathActor::UpdateSplineMesh()
{
	// Guard against access violations
	if (ProjectilePathSpline->GetNumberOfSplinePoints() < 2) return;

	// Local variable declarations
	FVector Location, LocationNext, Tangent, TangentNext;

	// Reserve memory for array
	ProjectilePathSplineMeshes.Reserve(ProjectilePathSpline->GetNumberOfSplinePoints() - 2);

	// For each spline point, create new spline mesh and set start, end, and tangents.
	for (int i = 0; i < ProjectilePathSpline->GetNumberOfSplinePoints() - 1; ++i)
	{
		// Assign location and tangent variables
		ProjectilePathSpline->GetLocationAndTangentAtSplinePoint(i, Location, Tangent, ESplineCoordinateSpace::Local);
		ProjectilePathSpline->GetLocationAndTangentAtSplinePoint(i + 1, LocationNext, TangentNext, ESplineCoordinateSpace::Local);

		USplineMeshComponent* SplineMesh = Cast<USplineMeshComponent>(AddComponentByClass(USplineMeshComponent::StaticClass(), true, GetActorTransform(), false));

		SplineMesh->SetMobility(EComponentMobility::Movable);
		SplineMesh->SetStaticMesh(Mesh);
		SplineMesh->SetMaterial(0, MeshMaterial);
		SplineMesh->SetStartAndEnd(Location, Tangent, LocationNext, TangentNext);
		SplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		ProjectilePathSplineMeshes.Add(SplineMesh);
	}
}

void AProjectilePathActor::ClearSpline()
{
	ProjectilePathSpline->ClearSplinePoints();

	for (auto& Mesh_itr : ProjectilePathSplineMeshes)
	{
		Mesh_itr->DestroyComponent();
	}

	ProjectilePathSplineMeshes.Empty();
}
