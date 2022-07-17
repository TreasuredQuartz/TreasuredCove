

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KismetProceduralMeshLibrary.h"
#include "BulletStruct.h"
#include "ItemKeyStruct.h"
#include "StaticLibrary.generated.h"

UCLASS()
class TREASUREDCOVE_API UStaticLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FName GetTagLeafName(const struct FGameplayTag& Tag);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal FBulletStruct", CompactNodeTitle = "==", Keywords = "== equal"), Category = "Math")
	static bool EqualEqual_FBulletStructFBulletStruct(const FBullet& arg1, const FBullet& arg2);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal FBulletStruct", CompactNodeTitle = "==", Keywords = "== equal"), Category = "Math")
	static bool EqualEqual_FItemKeyFItemKey(const FItemKey& arg1, const FItemKey& arg2);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Find FItemKey", CompactNodeTitle = "FIND", Keywords = "find"), Category = "Arrays")
	static int Find_FItemKeyFItemKey(const TArray<FItemKey> Array, const FItemKey ItemToFind);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Add FItemKey", CompactNodeTitle = "ADD", Keywords = "+ Add Addition add addition"), Category = "Math")
	static FItemKey Addition_FItemKeyFItemKey(const FItemKey Item1, const FItemKey Item2);

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

	/** Get Current Planet through Game State. Null if none or invalid game state cast. */
	static TObjectPtr<UObject> GetCurrentPlanet(TObjectPtr<UObject> CallingObject);
};