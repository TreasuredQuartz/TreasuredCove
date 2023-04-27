// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectilePathActor.generated.h"

class USplineComponent;
class USplineMeshComponent;

UCLASS()
class TREASUREDCOVE_API AProjectilePathActor : public AActor
{
	GENERATED_BODY()
	
private:
	float ProjectileCollisionRadius;
	FVector LaunchVelocity;
	TArray<USplineMeshComponent*> ProjectilePathSplineMeshes;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileArc")
	UStaticMesh* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileArc")
	UMaterialInterface* MeshMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectileArc")
	USceneComponent* Root;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectileArc")
	USplineComponent* ProjectilePathSpline;

private:
	// Helper function called in tick. Used after spline has been populated
	// with points to create NewObjects "SplineMeshComponents".
	void UpdateSplineMesh();

	// Helper function called in tick. Clears Spline points, destroys
	// Meshes, and then clears mesh array.
	void ClearSpline();

public:
	void SetProjectileCollisionRadius(float NewRadius);
	void SetLaunchVelocity(FVector NewVelocity);

public:	
	// Sets default values for this actor's properties
	AProjectilePathActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
