// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProceduralMeshSectionStruct.h"
#include "KatanaCreator.generated.h"

class UPhysicalMaterial;

struct TREASUREDCOVE_API FPartVertexData
{
public:
	FPartVertexData();

public:
	FVector Location = FVector::ZeroVector;
	FVector Normal = FVector::ZeroVector;
	FVector2D UV = FVector2D::ZeroVector;
	FLinearColor Color = FLinearColor::Black;
};

struct TREASUREDCOVE_API FPartEdgeData
{
	FPartVertexData A;
	FPartVertexData B;
};

struct TREASUREDCOVE_API FPartTriangleData
{
	FPartVertexData ABC[3];
};

struct TREASUREDCOVE_API FPartTriangleRange
{
	TArray<FPartTriangleData> Minimum;
	TArray<FPartTriangleData> Maximum;

	int32 ElementIndex;
};

UCLASS()
class TREASUREDCOVE_API UPart : public UObject
{
	GENERATED_BODY()

public:
	UPart() {};

private:
	// Recursive option to help simplify complex parts
	TArray<UPart*> InnerParts;

	// The Dimensions of this part. 
	// If containing InnerParts, then this will be used as
	// As a cheaper LOD/SweepTrace.
	FPartTriangleRange SectionRange;

#if WITH_EDITOR
	//
	URuntimeMeshComponentStatic* Mesh;
#endif

	// Metal, Wood, Fungus etc.
	TArray<FName> AvailableMaterialTypes;
public:
	bool HasInnerParts() const { !InnerParts.IsEmpty(); };

public:
	FProceduralMeshSection GenerateSection();
};

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UKatanaCreator : public UObject
{
	GENERATED_BODY()

public:
	UKatanaCreator();

private:
	TArray<UPart*> Parts;

	//
	URuntimeMeshComponentStatic* Mesh;

public:
	void GenerateKatana();
};
