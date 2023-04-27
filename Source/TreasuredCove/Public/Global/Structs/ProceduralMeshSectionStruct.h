// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/RuntimeMeshComponentStatic.h"
#include "ProceduralMeshSectionStruct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FProceduralMeshSection
{
	GENERATED_BODY()

public:
	FProceduralMeshSection() : Vertices(TArray<FVector>()),
		Triangles(TArray<int32>()),
		Normals(TArray<FVector>()),
		UVs(TArray<FVector2D>()),
		Tangents(TArray<struct FRuntimeMeshTangent>()),
		VertexColors(TArray<FColor>()),
		Material(nullptr)
	{}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> Vertices;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Triangles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> Normals;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector2D> UVs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<struct FRuntimeMeshTangent> Tangents;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FColor> VertexColors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableCollision = false;
	uint32 ElementID = 0;
	uint32 NumTriangles = 0;

	void Clear()
	{
		Vertices.Empty();
		Triangles.Empty();
		Normals.Empty();
		UVs.Empty();
		Tangents.Empty();
		VertexColors.Empty();
		Material = nullptr;
		bEnableCollision = false;
		ElementID = 0;
		NumTriangles = 0;
	}

	// UFUNCTION(BlueprintCallable, CallInEditor)
		void AddTriangle(TArray<FVector> InVertices, TArray<int32> InTriangle) {
		++NumTriangles;
	}

	// UFUNCTION(BlueprintCallable, CallInEditor)
		void AddQuad() {
		NumTriangles += 2;
	}
};
