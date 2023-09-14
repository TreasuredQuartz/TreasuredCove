// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KismetProceduralMeshLibrary.h"
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

	static UStaticMesh* CopyStaticMeshFromProceduralMesh(UProceduralMeshComponent* ProcMesh, FString SavePath, FString Name);

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

	static void HolePunchProceduralMesh(UProceduralMeshComponent* InProcMesh, FVector EnterPoint, FVector EnterNormal, FVector ExitPoint, float HoleRadius, EProcMeshSliceCapOption CapOption, UMaterialInterface* CapMaterial, int32 Steps);

	static void DentProceduralMesh(UProceduralMeshComponent* InProcMesh, FVector Center, float Radius, float ExpFalloff);

	/** Util that returns 1 if on positive side of plane, -1 if negative, or 0 if split by plane */
	static int32 BoxPlaneCompareLimited(FBox InBox, const FPlane& InPlane, float Radius, FVector Center);

	/** Take two static mesh verts and interpolate all values between them */
	static struct FProcMeshVertex InterpolateVert(const FProcMeshVertex& V0, const FProcMeshVertex& V1, float Alpha);

	/** Transform triangle from 2D to 3D static-mesh triangle. */
	static void Transform2DPolygonTo3D(const struct FUtilPoly2D& InPoly, const FMatrix& InMatrix, TArray<FProcMeshVertex>& OutVerts, FBox& OutBox);

	/** Given a polygon, decompose into triangles. */
	static bool TriangulatePoly(TArray<uint32>& OutTris, const TArray<FProcMeshVertex>& PolyVerts, int32 VertBase, const FVector& PolyNormal);

	/** Util to slice a convex hull with a plane */
	static void SliceConvexElem(const FKConvexElem& InConvex, const FPlane& SlicePlane, TArray<FVector>& OutConvexVerts);
};

