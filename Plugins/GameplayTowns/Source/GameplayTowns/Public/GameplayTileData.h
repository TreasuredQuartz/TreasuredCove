// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTileData.generated.h"

/* 3D Cube information
const int32 VoxelTriangles[] = { 2, 1, 0, 0, 3, 2 };
const FVector2D VoxelUVs[] = { FVector2D(0.00000, 0.00000), FVector2D(0.00000, 1.00000), FVector2D(1.00000, 1.00000), FVector2D(1.00000, 0.00000) };
const FVector VoxelNormals0[] = { FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1) };
const FVector VoxelNormals1[] = { FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1) };
const FVector VoxelNormals2[] = { FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0) };
const FVector VoxelNormals3[] = { FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0) };
const FVector VoxelNormals4[] = { FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0) };
const FVector VoxelNormals5[] = { FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0) };
*/

UENUM(BlueprintType)
enum class EFaceOrientation : uint8
{
	Top,
	Bottom,
	Right,
	Left,
	Front,
	Back,
	FaceOrientation_MAX UMETA(Hidden)
}; constexpr uint8 FaceEnum_MAX = static_cast<uint8>(EFaceOrientation::FaceOrientation_MAX);

DECLARE_ENUM_TO_STRING(EFaceOrientation);

USTRUCT()
struct GAMEPLAYTOWNS_API FVertexData
{
	GENERATED_BODY()

public:
	FVertexData() : Position(FVector::ZeroVector),
		Normal(FVector::ZeroVector),
		UVPosition(FVector2D::ZeroVector),
		Color(FLinearColor::Black)
	{}

public:
	UPROPERTY(EditDefaultsOnly, Meta=(ClampMin=-1, ClampMax=1))
	FVector Position;
	UPROPERTY(EditDefaultsOnly, Meta=(ClampMin = -1, ClampMax = 1))
	FVector Normal;
	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = 0, ClampMax = 1))
	FVector2D UVPosition;
	UPROPERTY(EditDefaultsOnly)
	FLinearColor Color;
};

USTRUCT()
struct GAMEPLAYTOWNS_API FTriangle
{
	GENERATED_BODY()

public:
	FTriangle() : VertIndices(FIntVector())
	{}

public:
	UPROPERTY(EditDefaultsOnly, Meta=(ClampMin=0))
	FIntVector VertIndices;
};

USTRUCT(BlueprintType)
struct GAMEPLAYTOWNS_API FQuad
{
	GENERATED_BODY()

public:
	FQuad() : Triangles(TArray<FTriangle>()),
		VerticesData(TArray<FVertexData>())
	{}

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FTriangle> Triangles;
	UPROPERTY(EditDefaultsOnly)
	TArray<FVertexData> VerticesData;
};

/**
 * 
 */
UCLASS()
class GAMEPLAYTOWNS_API UGameplayTileData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UGameplayTileData();

#define FaceOrientation_MAX static_cast<uint8>(EFaceOrientation::FaceOrientation_MAX)
	UPROPERTY(EditDefaultsOnly, EditFixedSize, meta = (EditFixedOrder))
	FQuad StaticFaces[FaceOrientation_MAX];
#undef FaceOrientation_MAX

	FPrimaryAssetId GetPrimaryAssetId() const override;
	
};


