// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Global/Actors/Items/GAWeapon.h"
#include "MCWorld_VoxelItem.generated.h"

class URotatingMovementComponent;

/** This class originally inherrited from AGAActor, but now
 * The structure of the project has changed. AGAWeapon was empty
 * but for now contains the original properties of AGAActor.
 */
UCLASS()
class TREASUREDCOVE_API AMCWorld_VoxelItem : 
	public AGAWeapon
{
	GENERATED_BODY()

public:
	AMCWorld_VoxelItem();

	URotatingMovementComponent* Rotation;

	// UPROPERTY()
	// class UProceduralMeshComponent* PMesh;

	FVector PMeshScale;
	FVector PMeshLocation;
	FRotator PMeshRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int VoxelSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int VoxelSizeHalf;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Size;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SizeHalf;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMini;

	void UpdateMesh();
	void AddTriangle(TArray < int32 > &Triangles, int32 V1, int32 V2, int32 V3);
	// void AddFace(TArray< FVector > &Vertices, FVector Vert1, FVector Vert2, FVector Vert3, FVector Vert4);

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
};
