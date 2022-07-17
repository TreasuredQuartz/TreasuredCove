// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletCreator.generated.h"

UCLASS()
class TREASUREDCOVE_API ABulletCreator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletCreator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Members
	float Length;
	float Length_Head;
	float Length_Cylinder;
	float Length_Tail;

	float Radius_Head;
	float Radius_Cylinder;
	float Radius_Tail;

private:
	// Components
	class UProceduralMeshComponent* BulletMesh;
	class UDrawingCanvas* Canvas;

public:
	// Initializer
	UFUNCTION(BlueprintCallable, Category = "BulletCanvas")
	void InitializeBullet(float inLengthHead, float inLengthCylinder, float inLengthTail, float inRadiusCylinder, float inRadiusTail);

	// Getters
	UFUNCTION(BlueprintCallable, Category = "BulletCanvas")
	UTexture2D* GetBulletTexture(bool& Succeeded);

	// Functions
	UFUNCTION(BlueprintCallable, Category = "BulletCanvas")
	void CreateBulletTexture();
	UFUNCTION(BlueprintCallable, Category = "BulletCanvas")
	void CreateBulletModel();

	int Formula(float x, float y);

	int ConicFormula(float x,float y);
	int SphereicallyBluntedCone(float x, float y, float sphereRadius);

	// uint8 Metal

	// UBulletCartride Cartridge;

	uint8 HeadType;

};
