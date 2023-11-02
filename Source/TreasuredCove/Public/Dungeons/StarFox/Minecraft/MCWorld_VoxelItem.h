// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Global/Actors/Items/GAPickup.h"
#include "Global/Structs/ProceduralMeshSectionStruct.h"
#include "MCWorld_VoxelItem.generated.h"

class UGameplayTileData;
class URealtimeMeshComponent;
class URotatingMovementComponent;
class UBobbingMovementComponent;

/** This class originally inherrited from AGAActor, but now
 * The structure of the project has changed. AGAWeapon was empty
 * but for now contains the original properties of AGAActor.
 */
UCLASS()
class TREASUREDCOVE_API AMCWorld_VoxelItem : 
	public AGAPickup
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VoxelItem", Meta = (AllowPrivateAccess = "true"))
	URealtimeMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VoxelItem", Meta = (AllowPrivateAccess = "true"))
	URotatingMovementComponent* Rotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VoxelItem", Meta = (AllowPrivateAccess = "true"))
	UBobbingMovementComponent* Bobbing;

public:
	AMCWorld_VoxelItem();

	FProceduralMeshSection MeshSection;

	FVector PMeshScale;
	FVector PMeshLocation;
	FRotator PMeshRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Material;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGameplayTileData* BlockData;
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
