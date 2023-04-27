// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SenseSysHelpers.h"
#include "FootprintActor.generated.h"

class UStaticMeshComponent;
class USenseStimulusComponent;
class USensorBase;

UCLASS()
class TREASUREDCOVE_API AFootprintActor : public AActor
{
	GENERATED_BODY()
	
#pragma region PrivateMembers
private:
	// The surface the footprint rests on.
	int SurfaceType;

	// The Footprintmask to use.
	int FootprintType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TArray<UMaterialInterface*> FootprintSurfaces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<UTexture*> FootprintMasks;
#pragma endregion
public:	
	// Sets default values for this actor's properties
	AFootprintActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#pragma region Components
public:
	// Mesh Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneRoot;

	// Mesh Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* FootprintMesh;

	// Stimulus Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USenseStimulusComponent* SenseStimulus;

#pragma endregion
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to change footprint surface type
	void SetSurfaceType(int PhysSurfaceType);

	// Called to change which foot is being used
	void SetFootprintType(int NewFootprintType);

	// Called when SetSurfaceType() is called to change the current surface to a different type.
	UFUNCTION(BlueprintNativeEvent, Category = "Footprint")
	void OnSurfaceTypeChanged();

	// Called when sensed (heard or seen)
	UFUNCTION()
	void OnSensed(const USensorBase* Sensor, int32 Channel, EOnSenseEvent SenseEvent);

	// Called when Heard
	UFUNCTION(BlueprintNativeEvent, Category = "Footprint")
	void OnHeard();

	// Called when Seen
	UFUNCTION(BlueprintNativeEvent, Category = "Footprint")
	void OnSeen();
};
