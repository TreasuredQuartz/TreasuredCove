// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
//#include "SkeletalMeshMergeParams.h"
#include "MeshExtensionLibrary.generated.h"

class UProceduralMeshComponent;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UMeshExtensionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void CopyProceduralMeshFromStaticMesh(UStaticMesh* StaticMesh, int32 LODIndex, UProceduralMeshComponent* ProcMeshComponent, bool bCreateCollision);

//	/**
//	* Merges the given meshes into a single mesh.
//	* @return the merged mesh (will be invalid if the merge failed).
//	*/
//	UFUNCTION(BlueprintCallable, Category = "Mesh Merge", meta = (UnsafeDuringActorConstruction = "true"))
//	static class USkeletalMesh* MergeMeshes(const FSkeletalMeshMergeParams& Params);
//
//	/** 
//	* Takes the given points and returns the triangles.
//	* Performs a Deluaney Triangulation algorithm (Watson Bowyer)
//	* 
//	*/
//	UFUNCTION(BlueprintCallable, Category = "Mesh Creation")
//	static void DelauneyTriangulation_WatsonBowyer(UPARAM(ref) TArray<FVector>& Points, float radius, 
//	TArray<int>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UVs);
};

