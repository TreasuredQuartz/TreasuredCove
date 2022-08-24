// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"

#include "PlanetSettings.h"
#include "PlanetShapeGenerator.h"
#include "PlanetMaterialGenerator.h"
#include "ProceduralPlanet.generated.h"

class UCameraComponent;
class UPlanetLandscape;

UCLASS()
class PROCEDURALPLANETPLUGIN_API AProceduralPlanet : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FPlanetSettings Settings;

	FPlanetShapeGenerator ShapeGenerator;
	FPlanetMaterialGenerator MaterialGenerator;
private:
	bool bIsValid;
	FVector playerLocation;
	FVector playerDirection;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "false"))
	TArray<UProceduralMeshComponent*> Meshs;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "false"))
	TArray<UPlanetLandscape*> Landscapes;

public:
	void OnValidate();
	void Initialize();
	void GeneratePlanet();
	void GenerateMesh();
	void GenerateHeight();
	void GenerateMaterial();

	void OnShapeSettingsUpdated();
	void OnMaterialSettingsUpdated();

	void checkCurrentView();
	void CheckLOD();
	bool LODLineTrace();

public:	
	// Sets default values for this actor's properties
	AProceduralPlanet();

	// Called in editor and on begin play
	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	// Called on editor property values changed
	void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;

	// Called on editor Array Property value changed
	void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e) override;

	virtual bool ShouldTickIfViewportsOnly() const override;
#endif

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/* Components */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "false"))
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Settings | Level-Of-Detail")
	UCameraComponent* LODCamera;
};


