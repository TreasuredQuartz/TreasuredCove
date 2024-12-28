// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshExtension/MeshExtensionLibrary.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "RawMesh.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMesh.h"
#include "PhysicsEngine/ConvexElem.h"
#include "PhysicsEngine/BodySetup.h"
//#include "SkeletalMeshMerge.h"
//#include "Engine/SkeletalMeshSocket.h"
//#include "Engine/SkeletalMesh.h"
//#include "Animation/Skeleton.h"
//#include "Engine/Engine.h"
//#include "Math/Range.h"
//#include "MeshExtension/Delaunauy.h"
#include "GeomTools.h"

//UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent();
void UMeshExtensionLibrary::CopyProceduralMeshFromStaticMesh(UStaticMesh* StaticMesh, int32 LODIndex, UProceduralMeshComponent* ProcMeshComponent, bool bCreateCollision)
{
	//// MESH DATA

	int32 NumSections = StaticMesh->GetNumSections(LODIndex);
	for (int32 SectionIndex = 0; SectionIndex < NumSections; SectionIndex++)
	{
		// Buffers for copying geom data
		TArray<FVector> Vertices;
		TArray<int32> Triangles;
		TArray<FVector> Normals;
		TArray<FVector2D> UVs;
		TArray<FVector2D> UVs1;
		TArray<FVector2D> UVs2;
		TArray<FVector2D> UVs3;
		TArray<FProcMeshTangent> Tangents;

		// Get geom data from static mesh
		UKismetProceduralMeshLibrary::GetSectionFromStaticMesh(StaticMesh, LODIndex, SectionIndex, Vertices, Triangles, Normals, UVs, Tangents);

		// Create section using data
		TArray<FLinearColor> DummyColors;
		ProcMeshComponent->CreateMeshSection_LinearColor(SectionIndex, Vertices, Triangles, Normals, UVs, UVs1, UVs2, UVs3, DummyColors, Tangents, bCreateCollision);
	}

	//// SIMPLE COLLISION

	// Clear any existing collision hulls
	ProcMeshComponent->ClearCollisionConvexMeshes();

	if (StaticMesh->GetBodySetup() != nullptr)
	{
		// Iterate over all convex hulls on static mesh..
		const int32 NumConvex = StaticMesh->GetBodySetup()->AggGeom.ConvexElems.Num();
		for (int ConvexIndex = 0; ConvexIndex < NumConvex; ConvexIndex++)
		{
			// Copy convex verts to ProcMesh
			FKConvexElem& MeshConvex = StaticMesh->GetBodySetup()->AggGeom.ConvexElems[ConvexIndex];
			ProcMeshComponent->AddCollisionConvexMesh(MeshConvex.VertexData);
		}
	}

	//// MATERIALS

	for (int32 MatIndex = 0; MatIndex < StaticMesh->GetStaticMaterials().Num(); MatIndex++)
	{
		ProcMeshComponent->SetMaterial(MatIndex, StaticMesh->GetMaterial(MatIndex));
	}
}

UStaticMesh* UMeshExtensionLibrary::CopyStaticMeshFromProceduralMesh(UProceduralMeshComponent* ProcMesh, FString SavePath, FString Name)
{
	UProceduralMeshComponent* ProcMeshComp = ProcMesh;
	if (ProcMeshComp != nullptr)
	{
		FString PackageName = SavePath;
		FRawMesh RawMesh;
		TArray<UMaterialInterface*> MeshMaterials;
		const int32 NumSections = ProcMeshComp->GetNumSections();
		int32 VertexBase = 0;
		for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
		{
			FProcMeshSection* ProcSection = ProcMeshComp->GetProcMeshSection(SectionIdx);
			// Copy verts
			for (FProcMeshVertex& Vert : ProcSection->ProcVertexBuffer)
			{
				RawMesh.VertexPositions.Add(FVector3f(Vert.Position));
			}
			// Copy 'wedge' info
			int32 NumIndices = ProcSection->ProcIndexBuffer.Num();
			for (int32 IndexIdx = 0; IndexIdx < NumIndices; IndexIdx++)
			{
				int32 Index = ProcSection->ProcIndexBuffer[IndexIdx];
				RawMesh.WedgeIndices.Add(Index + VertexBase);
				FProcMeshVertex& ProcVertex = ProcSection->ProcVertexBuffer[Index];
				FVector3f TangentX = FVector3f(ProcVertex.Tangent.TangentX);
				FVector3f TangentZ = FVector3f(ProcVertex.Normal);
				FVector3f TangentY = FVector3f(
					(TangentX ^ TangentZ).GetSafeNormal() * (ProcVertex.Tangent.bFlipTangentY ? -1.f : 1.f));
				RawMesh.WedgeTangentX.Add(TangentX);
				RawMesh.WedgeTangentY.Add(TangentY);
				RawMesh.WedgeTangentZ.Add(TangentZ);
				RawMesh.WedgeTexCoords[0].Add(FVector2f(ProcVertex.UV0));
				RawMesh.WedgeColors.Add(ProcVertex.Color);
			}
			// copy face info
			int32 NumTris = NumIndices / 3;
			for (int32 TriIdx = 0; TriIdx < NumTris; TriIdx++)
			{
				RawMesh.FaceMaterialIndices.Add(SectionIdx);
				RawMesh.FaceSmoothingMasks.Add(0); // Assume this is ignored as bRecomputeNormals is false
			}
			// Remember material
			MeshMaterials.Add(ProcMeshComp->GetMaterial(SectionIdx));
			// Update offset for creating one big index/vertex buffer
			VertexBase += ProcSection->ProcVertexBuffer.Num();
		}
		// If we got some valid data.
		if (RawMesh.VertexPositions.Num() > 3 && RawMesh.WedgeIndices.Num() > 3)
		{
			// Then find/create it.
			UPackage* Package = CreatePackage(*PackageName);
			check(Package);
			// Create StaticMesh object
			UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, FName(*Name), RF_Public | RF_Standalone);
			StaticMesh->InitResources();
			FGuid::NewGuid() = StaticMesh->GetLightingGuid();
			//StaticMesh->GetLightingGuid() = FGuid::NewGuid();
			// Create a Source Model then set it to variable
			StaticMesh->AddSourceModel();
			FStaticMeshSourceModel& SrcModel = StaticMesh->GetSourceModel(0);
			// Add source to new StaticMesh
			SrcModel.BuildSettings.bRecomputeNormals = false;
			SrcModel.BuildSettings.bRecomputeTangents = false;
			SrcModel.BuildSettings.bRemoveDegenerates = false;
			SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
			SrcModel.BuildSettings.bUseFullPrecisionUVs = false;
			SrcModel.BuildSettings.bGenerateLightmapUVs = true;
			SrcModel.BuildSettings.SrcLightmapIndex = 0;
			SrcModel.BuildSettings.DstLightmapIndex = 1;
			SrcModel.RawMeshBulkData->SaveRawMesh(RawMesh);
			// Copy materials to new mesh
			for (UMaterialInterface* Material : MeshMaterials)
			{
				StaticMesh->GetStaticMaterials().Add(FStaticMaterial(Material));
			}
			//Set the Imported version before calling the build
			StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;
			// Build mesh from source
			StaticMesh->Build(false);
			StaticMesh->PostEditChange();
			// Notify asset registry of new asset
			FAssetRegistryModule::AssetCreated(StaticMesh);
			return StaticMesh;
		}
	}
	return nullptr;
}

//static void ToMergeParams(const TArray<FSkeleMeshMergeSectionMapping_BP>& InSectionMappings, TArray<FSkelMeshMergeSectionMapping>& OutSectionMappings)
//{
//	if (InSectionMappings.Num() > 0)
//	{
//		OutSectionMappings.AddUninitialized(InSectionMappings.Num());
//		for (int32 i = 0; i, InSectionMappings.Num(); ++i)
//		{
//			OutSectionMappings[i].SectionIDs = InSectionMappings[i].SectionIDs;
//		}
//	}
//}
//
//static void ToMergeParams(const TArray<FSkelMeshMergeUVTransformMapping>& InUVTransformsPerMesh, TArray<FSkelMeshMergeUVTransforms>& OutUVTransformsPerMesh)
//{
//	if (InUVTransformsPerMesh.Num() > 0)
//	{
//		OutUVTransformsPerMesh.Empty();
//		OutUVTransformsPerMesh.AddUninitialized(InUVTransformsPerMesh.Num());
//		for (int32 i = 0; i < InUVTransformsPerMesh.Num(); ++i)
//		{
//			TArray<TArray<FTransform>>& OutUVTransforms = OutUVTransformsPerMesh[i].UVTransformsPerMesh;
//			const TArray<FSkelMeshMergeUVTransform>& InUVTransforms = InUVTransformsPerMesh[i].UVTransformsPerMesh;
//			if (InUVTransforms.Num() > 0)
//			{
//				OutUVTransforms.Empty();
//				OutUVTransforms.AddUninitialized(InUVTransforms.Num());
//				for (int32 j = 0; j < InUVTransforms.Num(); j++)
//				{
//					OutUVTransforms[j] = InUVTransforms[j].UVTransforms;
//				}
//			}
//		}
//	}
//}
//
//USkeletalMesh* UMeshExtensionLibrary::MergeMeshes(const FSkeletalMeshMergeParams& Params)
//{
//	TArray<USkeletalMesh*> MeshesToMergeCopy = Params.MeshesToMerge;
//	MeshesToMergeCopy.RemoveAll([](USkeletalMesh* InMesh)
//	{
//		return InMesh == nullptr;
//	});
//
//	if (MeshesToMergeCopy.Num() <= 1)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Must provide multiple valid Skeletal Meshes in order to perform a merge."));
//	}
//
//	EMeshBufferAccess BufferAccess = Params.bNeedsCPUAccess ?
//		EMeshBufferAccess::ForceCPUAndGPU :
//		EMeshBufferAccess::Default;
//	TArray<FSkelMeshMergeSectionMapping> SectionMappings;
//	TArray<FSkelMeshMergeUVTransforms> UVTransforms;
//	ToMergeParams(Params.MeshSectionMappings, SectionMappings);
//	ToMergeParams(Params.UVTransformsPerMesh, UVTransforms);
//	bool bRunDuplicateCheck = false;
//	USkeletalMesh* BaseMesh = NewObject<USkeletalMesh>();
//	if (Params.Skeleton && Params.bSkeletonBefore)
//	{
//		BaseMesh->Skeleton = Params.Skeleton;
//		bRunDuplicateCheck = true;
//		for (USkeletalMeshSocket* Socket : BaseMesh->GetMeshOnlySocketList())
//		{
//			if (Socket)
//			{
//				UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocket: %s"), *(Socket->SocketName.ToString()));
//			}
//		}
//		for (USkeletalMeshSocket* Socket : BaseMesh->Skeleton->Sockets)
//		{
//			if (Socket)
//			{
//				UE_LOG(LogTemp, Warning, TEXT("SkelSocket: %s"), *(Socket->SocketName.ToString()));
//			}
//		}
//	}
//
//	FSkeletalMeshMerge Merger(BaseMesh, MeshesToMergeCopy, SectionMappings, Params.StripLODS, BufferAccess, UVTransforms.GetData());
//	if (!Merger.DoMerge())
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Merge Failed!"));
//		return nullptr;
//	}
//
//	if (Params.Skeleton && !Params.bSkeletonBefore)
//	{
//		BaseMesh->Skeleton = Params.Skeleton;
//	}
//
//	if (bRunDuplicateCheck)
//	{
//		TArray<FName> SkelMeshSockets;
//		TArray<FName> SkelSockets;
//		for (USkeletalMeshSocket* Socket : BaseMesh->GetMeshOnlySocketList())
//		{
//			if (Socket)
//			{
//				SkelMeshSockets.Add(Socket->GetFName());
//				UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocket: %s"), *(Socket->SocketName.ToString()));
//			}
//		}
//		for (USkeletalMeshSocket* Socket : BaseMesh->Skeleton->Sockets)
//		{
//			if (Socket)
//			{
//				SkelSockets.Add(Socket->GetFName());
//				UE_LOG(LogTemp, Warning, TEXT("SkelSocket: %s"), *(Socket->SocketName.ToString()));
//			}
//		}
//
//		TSet<FName> UniqueSkelMeshSockets;
//		TSet<FName> UniqueSkelSockets;
//		UniqueSkelMeshSockets.Append(SkelMeshSockets);
//		UniqueSkelSockets.Append(SkelSockets);
//		int32 Total = SkelSockets.Num() + SkelMeshSockets.Num();
//		int32 UniqueTotal = UniqueSkelMeshSockets.Num() + UniqueSkelSockets.Num();
//		UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocketCount: %d | SkelSocketCount: %d | Combined: %d"), SkelMeshSockets.Num(), SkelSockets.Num(), Total);
//		UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocketCount: %d | SkelSocketCount: %d | Combined: %d"), UniqueSkelMeshSockets.Num(), UniqueSkelSockets.Num(), UniqueTotal);
//		UE_LOG(LogTemp, Warning, TEXT("Found Duplicates: %s"), *((Total != UniqueTotal) ? FString("True") : FString("False")));
//	}
//
//	return BaseMesh;
//}
//
//void addSouthPoleToMesh(int southPoleID, TArray<size_t>& triangles, TArray<size_t>& halfEdges)
//{
//	int numSides = triangles.Num();
//	auto s_next_s = [](int s) { return (s % 3 == 2) ? s - 2 : s + 1; };
//
//	int numUnpairedSides = 0,
//		firstUnpairedSide = -1;
//	TArray<int> pointIdToSideId;
//	pointIdToSideId.Init(0, numSides);
//	for (int s = 0; s < numSides; s++)
//	{
//		if (halfEdges[s] == -1) {
//			++numUnpairedSides;
//			pointIdToSideId[triangles[s]] = s;
//			firstUnpairedSide = s;
//		}
//	}
//
//	TArray<int> newTriangles;
//	TArray<int> newEdges;
//
//	newTriangles.Init(0, numSides + 3 * numUnpairedSides);
//	newEdges.Init(0, numSides + 3 * numUnpairedSides);
//
//	for (int i = 0; i < numSides; ++i)
//	{
//		newTriangles[i] = triangles[i];
//		newEdges[i] = halfEdges[i];
//	}
//
//	for (int i = 0, s = firstUnpairedSide; i < numUnpairedSides; ++i, s = pointIdToSideId[newTriangles[s_next_s(s)]])
//	{
//		int newSide = numSides + 3 * i;
//		newEdges[s] = newSide;
//		newEdges[newSide] = s;
//		newTriangles[newSide] = newTriangles[s_next_s(s)];
//
//		newTriangles[newSide + 1] = newTriangles[s];
//		newTriangles[newSide + 2] = southPoleID;
//
//		int k = numSides + (3 * i + 4) % (3 * numUnpairedSides);
//
//		newEdges[newSide + 2] = k;
//		newEdges[k] = newSide + 2;
//	}
//	triangles.Reset();
//	halfEdges.Reset();
//
//	for (int newTriangle : newTriangles)
//	{
//		triangles.Push(newTriangle);
//	}
//
//	for (int newEdge : newEdges)
//	{
//		halfEdges.Push(newEdge);
//	}
//}
//
//TArray<int>& generateDelaunayGeometry(TArray<FVector>const& xyz, Delaunay::Delaunay const& delaunay)
//{
//	auto triangles = delaunay.triangles;
//	TArray<int>* geometry = new TArray<int>();
//	int numTriangles = triangles.Num() / 3,
//		i = 0;
//	double a, b, c;
//	geometry->Reserve(triangles.Num());
//
//	for (i; i < numTriangles; ++i)
//	{
//		a = triangles[3*i];
//		b = triangles[3*i + 1];
//		c = triangles[3*i + 2];
//
//		geometry->Push(xyz[a].X); geometry->Push(xyz[a].Y); geometry->Push(xyz[a].Z);
//		geometry->Push(xyz[b].X); geometry->Push(xyz[b].Y); geometry->Push(xyz[b].Z);
//		geometry->Push(xyz[c].X); geometry->Push(xyz[c].Y); geometry->Push(xyz[c].Z);
//	}
//
//	return *geometry;
//}
//
//void UMeshExtensionLibrary::DelauneyTriangulation_WatsonBowyer(UPARAM(ref) TArray<FVector>& Points, float radius, 
//	TArray<int>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UVs)
//{
//	// return Triangles
//	// return Normals
//	// return UVs
//}

int32 UMeshExtensionLibrary::BoxPlaneCompareLimited(FBox InBox, const FPlane& InPlane, float Radius, FVector Center)
{
	FVector BoxCenter, BoxExtents;
	InBox.GetCenterAndExtents(BoxCenter, BoxExtents);

	// Find distance of box center from plane
	float BoxCenterDist = InPlane.PlaneDot(BoxCenter);

	// See size of box in plane normal direction
	float BoxSize = FVector::BoxPushOut(InPlane, BoxExtents);

	if (BoxCenterDist > BoxSize)
	{
		return 1;
	}
	else if (BoxCenterDist < -BoxSize)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

FProcMeshVertex UMeshExtensionLibrary::InterpolateVert(const FProcMeshVertex& V0, const FProcMeshVertex& V1, float Alpha)
{
	FProcMeshVertex Result;

	// Handle dodgy alpha
	if (FMath::IsNaN(Alpha) || !FMath::IsFinite(Alpha))
	{
		Result = V1;
		return Result;
	}

	Result.Position = FMath::Lerp(V0.Position, V1.Position, Alpha);

	Result.Normal = FMath::Lerp(V0.Normal, V1.Normal, Alpha);

	Result.Tangent.TangentX = FMath::Lerp(V0.Tangent.TangentX, V1.Tangent.TangentX, Alpha);
	Result.Tangent.bFlipTangentY = V0.Tangent.bFlipTangentY; // Assume flipping doesn't change along edge...

	Result.UV0 = FMath::Lerp(V0.UV0, V1.UV0, Alpha);

	Result.Color.R = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.R), float(V1.Color.R), Alpha)), 0, 255);
	Result.Color.G = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.G), float(V1.Color.G), Alpha)), 0, 255);
	Result.Color.B = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.B), float(V1.Color.B), Alpha)), 0, 255);
	Result.Color.A = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.A), float(V1.Color.A), Alpha)), 0, 255);

	return Result;
}

/** Transform triangle from 2D to 3D static-mesh triangle. */
void UMeshExtensionLibrary::Transform2DPolygonTo3D(const FUtilPoly2D& InPoly, const FMatrix& InMatrix, TArray<FProcMeshVertex>& OutVerts, FBox& OutBox)
{
	FVector PolyNormal = -InMatrix.GetUnitAxis(EAxis::Z);
	FProcMeshTangent PolyTangent(InMatrix.GetUnitAxis(EAxis::X), false);

	for (int32 VertexIndex = 0; VertexIndex < InPoly.Verts.Num(); VertexIndex++)
	{
		const FUtilVertex2D& InVertex = InPoly.Verts[VertexIndex];

		FProcMeshVertex NewVert;

		NewVert.Position = InMatrix.TransformPosition(FVector(InVertex.Pos.X, InVertex.Pos.Y, 0.f));
		NewVert.Normal = PolyNormal;
		NewVert.Tangent = PolyTangent;
		NewVert.Color = InVertex.Color;
		NewVert.UV0 = InVertex.UV;

		OutVerts.Add(NewVert);

		// Update bounding box
		OutBox += NewVert.Position;
	}
}

/** Given a polygon, decompose into triangles. */
bool UMeshExtensionLibrary::TriangulatePoly(TArray<uint32>& OutTris, const TArray<FProcMeshVertex>& PolyVerts, int32 VertBase, const FVector& PolyNormal)
{
	// Can't work if not enough verts for 1 triangle
	int32 NumVerts = PolyVerts.Num() - VertBase;
	if (NumVerts < 3)
	{
		OutTris.Add(0);
		OutTris.Add(2);
		OutTris.Add(1);

		// Return true because poly is already a tri
		return true;
	}

	// Remember initial size of OutTris, in case we need to give up and return to this size
	const int32 TriBase = OutTris.Num();

	// Init array of vert indices, in order. We'll modify this
	TArray<int32> VertIndices;
	VertIndices.AddUninitialized(NumVerts);
	for (int VertIndex = 0; VertIndex < NumVerts; VertIndex++)
	{
		VertIndices[VertIndex] = VertBase + VertIndex;
	}

	// Keep iterating while there are still vertices
	while (VertIndices.Num() >= 3)
	{
		// Look for an 'ear' triangle
		bool bFoundEar = false;
		for (int32 EarVertexIndex = 0; EarVertexIndex < VertIndices.Num(); EarVertexIndex++)
		{
			// Triangle is 'this' vert plus the one before and after it
			const int32 AIndex = (EarVertexIndex == 0) ? VertIndices.Num() - 1 : EarVertexIndex - 1;
			const int32 BIndex = EarVertexIndex;
			const int32 CIndex = (EarVertexIndex + 1) % VertIndices.Num();

			const FProcMeshVertex& AVert = PolyVerts[VertIndices[AIndex]];
			const FProcMeshVertex& BVert = PolyVerts[VertIndices[BIndex]];
			const FProcMeshVertex& CVert = PolyVerts[VertIndices[CIndex]];

			// Check that this vertex is convex (cross product must be positive)
			const FVector ABEdge = BVert.Position - AVert.Position;
			const FVector ACEdge = CVert.Position - AVert.Position;
			const float TriangleDeterminant = (ABEdge ^ ACEdge) | PolyNormal;
			if (TriangleDeterminant > 0.f)
			{
				continue;
			}

			bool bFoundVertInside = false;
			// Look through all verts before this in array to see if any are inside triangle
			for (int32 VertexIndex = 0; VertexIndex < VertIndices.Num(); VertexIndex++)
			{
				const FProcMeshVertex& TestVert = PolyVerts[VertIndices[VertexIndex]];

				if (VertexIndex != AIndex &&
					VertexIndex != BIndex &&
					VertexIndex != CIndex &&
					FGeomTools::PointInTriangle(FVector3f(AVert.Position), FVector3f(BVert.Position), FVector3f(CVert.Position), FVector3f(TestVert.Position)))
				{
					bFoundVertInside = true;
					break;
				}
			}

			// Triangle with no verts inside - its an 'ear'! 
			if (!bFoundVertInside)
			{
				OutTris.Add(VertIndices[AIndex]);
				OutTris.Add(VertIndices[CIndex]);
				OutTris.Add(VertIndices[BIndex]);

				// And remove vertex from polygon
				VertIndices.RemoveAt(EarVertexIndex);

				bFoundEar = true;
				break;
			}
		}

		// If we couldn't find an 'ear' it indicates something is bad with this polygon - discard triangles and return.
		if (!bFoundEar)
		{
			OutTris.SetNum(TriBase, true);
			return false;
		}
	}

	return true;
}

/** Util to slice a convex hull with a plane */
void UMeshExtensionLibrary::SliceConvexElem(const FKConvexElem& InConvex, const FPlane& SlicePlane, TArray<FVector>& OutConvexVerts)
{
	// Get set of planes that make up hull
	TArray<FPlane> ConvexPlanes;
	InConvex.GetPlanes(ConvexPlanes);

	if (ConvexPlanes.Num() >= 4)
	{
		// Add on the slicing plane (need to flip as it culls geom in the opposite sense to our geom culling code)
		ConvexPlanes.Add(SlicePlane.Flip());

		// Create output convex based on new set of planes
		FKConvexElem SlicedElem;
		bool bSuccess = SlicedElem.HullFromPlanes(ConvexPlanes, InConvex.VertexData);
		if (bSuccess)
		{
			OutConvexVerts = SlicedElem.VertexData;
		}
	}
}

void UMeshExtensionLibrary::HolePunchProceduralMesh(UProceduralMeshComponent* InProcMesh, FVector EnterPoint, FVector EnterNormal, FVector ExitPoint, float HoleRadius, EProcMeshSliceCapOption CapOption, UMaterialInterface* CapMaterial, int32 Steps)
{
	if (InProcMesh != nullptr)
	{
		TArray<FPlane> SlicePlanes;
		if (Steps < 4)
		{
			SlicePlanes.SetNumUninitialized(3);
		}
		else
		{
			SlicePlanes.SetNumUninitialized(Steps);
		}

		/*
		FVector North = EnterPoint + FVector(1, 0, 0) * HoleRadius;
		FVector NorthEast = EnterPoint + FVector(0.5, 0.5, 0) * HoleRadius;
		FVector East = EnterPoint + FVector(0, 1, 0) * HoleRadius;
		FVector SouthEast = EnterPoint + FVector(-0.5, 0.5, 0) * HoleRadius;
		FVector South = EnterPoint + FVector(-1, 0, 0) * HoleRadius;
		FVector SouthWest = EnterPoint + FVector(-0.5, -0.5, 0) * HoleRadius;
		FVector West = EnterPoint + FVector(0, -1, 0) * HoleRadius;
		FVector NorthWest = EnterPoint + FVector(0.5, -0.5, 0) * HoleRadius;
		*/

		for (int32 a = 0; a < SlicePlanes.Num(); a++)
		{
			FRotator NormalRotation =
				FRotationMatrix::MakeFromX(EnterNormal).Rotator();
			NormalRotation.Roll += (a + 1 / SlicePlanes.Num()) * 360;

			FTransform ProcCompToWorld = InProcMesh->GetComponentToWorld();
			FVector LocalEnterSpherePos = ProcCompToWorld.InverseTransformPosition(EnterPoint);
			FVector LocalExitSpherePos = ProcCompToWorld.InverseTransformPosition(ExitPoint);

			FVector RandUnitVector =
				FVector(0, FMath::FRand(), FMath::FRand()).GetSafeNormal();

			FRotator LocRotation =
				FRotationMatrix::MakeFromZ(RandUnitVector).Rotator();

			// Location on edge of circle pointing inward
			FVector LocPlanePos =
				LocRotation.RotateVector(RandUnitVector * HoleRadius + LocalEnterSpherePos);

			// Rotation according to impact normal
			FVector LocalPlaneNormal =
				NormalRotation.RotateVector(LocPlanePos).UpVector;

			FPlane SlicePlane(LocPlanePos, LocalPlaneNormal);

			// Set of new edges created by clipping polys by plane
			TArray<FUtilEdge3D> ClipEdges;

			for (int32 SectionIndex = 0; SectionIndex < InProcMesh->GetNumSections(); SectionIndex++)
			{
				FProcMeshSection* BaseSection = InProcMesh->GetProcMeshSection(SectionIndex);
				// If we have a section, and it has some valid geom
				if (BaseSection != nullptr && BaseSection->ProcIndexBuffer.Num() > 0 && BaseSection->ProcVertexBuffer.Num() > 0)
				{
					// Compare bounding box of section with slicing plane
					int32 BoxCompare = BoxPlaneCompareLimited(BaseSection->SectionLocalBox, SlicePlane, HoleRadius, EnterPoint);

					// Box totally clipped, clear section
					if (BoxCompare == -1)
					{
						InProcMesh->ClearMeshSection(SectionIndex);
					}
					// Box totally on one side of plane, leave it alone, do nothing
					else if (BoxCompare == 1)
					{
						// ...
					}
					// Box intersects plane, need to clip some polys!
					else
					{
						// New section for geometry
						FProcMeshSection NewSection;

						// Map of base vert index to sliced vert index
						TMap<int32, int32> BaseToSlicedVertIndex;
						TMap<int32, int32> BaseToOtherSlicedVertIndex;

						const int32 NumBaseVerts = BaseSection->ProcVertexBuffer.Num();

						// Distance of each base vert from slice point
						TArray<float> VertDistance;
						VertDistance.AddUninitialized(NumBaseVerts);

						// Build vertex buffer 
						for (int32 BaseVertIndex = 0; BaseVertIndex < NumBaseVerts; BaseVertIndex++)
						{
							FProcMeshVertex& BaseVert = BaseSection->ProcVertexBuffer[BaseVertIndex];

							// Calc distance from plane
							VertDistance[BaseVertIndex] = SlicePlane.PlaneDot(BaseVert.Position);

							// See if vert is being kept in this section
							if (VertDistance[BaseVertIndex] > 0.f)
							{
								// Copy to sliced v buffer
								int32 SlicedVertIndex = NewSection.ProcVertexBuffer.Add(BaseVert);
								// Update section bounds
								NewSection.SectionLocalBox += BaseVert.Position;
								// Add to map
								BaseToSlicedVertIndex.Add(BaseVertIndex, SlicedVertIndex);
							}
						}


						// Iterate over base triangles (ie 3 indices at a time)
						for (int32 BaseIndex = 0; BaseIndex < BaseSection->ProcIndexBuffer.Num(); BaseIndex += 3)
						{
							int32 BaseV[3]; // Triangle vert indices in original mesh
							int32* SlicedV[3]; // Pointers to vert indices in new v buffer

							// For each vertex..
							for (int32 i = 0; i < 3; i++)
							{
								// Get triangle vert index
								BaseV[i] = BaseSection->ProcIndexBuffer[BaseIndex + i];
								// Look up in sliced v buffer
								SlicedV[i] = BaseToSlicedVertIndex.Find(BaseV[i]);
							}

							// If all verts survived plane cull, keep the triangle
							if (SlicedV[0] != nullptr && SlicedV[1] != nullptr && SlicedV[2] != nullptr)
							{
								NewSection.ProcIndexBuffer.Add(*SlicedV[0]);
								NewSection.ProcIndexBuffer.Add(*SlicedV[1]);
								NewSection.ProcIndexBuffer.Add(*SlicedV[2]);
							}
							// If partially culled, clip to create 1 or 2 new triangles
							else
							{
								int32 FinalVerts[4];
								int32 NumFinalVerts = 0;

								FUtilEdge3D NewClipEdge;
								int32 ClippedEdges = 0;

								float PlaneDist[3];
								PlaneDist[0] = VertDistance[BaseV[0]];
								PlaneDist[1] = VertDistance[BaseV[1]];
								PlaneDist[2] = VertDistance[BaseV[2]];

								for (int32 EdgeIdx = 0; EdgeIdx < 3; EdgeIdx++)
								{
									int32 ThisVert = EdgeIdx;

									// If start vert is inside, add it.
									if (SlicedV[ThisVert] != nullptr)
									{
										check(NumFinalVerts < 4);
										FinalVerts[NumFinalVerts++] = *SlicedV[ThisVert];
									}

									// If start and next vert are on opposite sides, add intersection
									int32 NextVert = (EdgeIdx + 1) % 3;

									if ((SlicedV[EdgeIdx] == nullptr) != (SlicedV[NextVert] == nullptr))
									{
										// Find distance along edge that plane is
										float Alpha = -PlaneDist[ThisVert] / (PlaneDist[NextVert] - PlaneDist[ThisVert]);
										// Interpolate vertex params to that point
										FProcMeshVertex InterpVert = InterpolateVert(BaseSection->ProcVertexBuffer[BaseV[ThisVert]], BaseSection->ProcVertexBuffer[BaseV[NextVert]], FMath::Clamp(Alpha, 0.0f, 1.0f));

										// Add to vertex buffer
										int32 InterpVertIndex = NewSection.ProcVertexBuffer.Add(InterpVert);
										// Update bounds
										NewSection.SectionLocalBox += InterpVert.Position;

										// Save vert index for this poly
										check(NumFinalVerts < 4);
										FinalVerts[NumFinalVerts++] = InterpVertIndex;

										// When we make a new edge on the surface of the clip plane, save it off.
										check(ClippedEdges < 2);
										if (ClippedEdges == 0)
										{
											NewClipEdge.V0 = FVector3f(InterpVert.Position);
										}
										else
										{
											NewClipEdge.V1 = FVector3f(InterpVert.Position);
										}

										ClippedEdges++;
									}
								}

								// Triangulate the clipped polygon.
								for (int32 VertexIndex = 2; VertexIndex < NumFinalVerts; VertexIndex++)
								{
									NewSection.ProcIndexBuffer.Add(FinalVerts[0]);
									NewSection.ProcIndexBuffer.Add(FinalVerts[VertexIndex - 1]);
									NewSection.ProcIndexBuffer.Add(FinalVerts[VertexIndex]);
								}

								check(ClippedEdges != 1); // Should never clip just one edge of the triangle

								// If we created a new edge, save that off here as well
								if (ClippedEdges == 2)
								{
									ClipEdges.Add(NewClipEdge);
								}
							}
						}

						// If we have some valid geometry, update section
						if (NewSection.ProcIndexBuffer.Num() > 0 && NewSection.ProcVertexBuffer.Num() > 0)
						{
							// Assign new geom to this section
							InProcMesh->SetProcMeshSection(SectionIndex, NewSection);
						}
						// If we don't, remove this section
						else
						{
							InProcMesh->ClearMeshSection(SectionIndex);
						}
					}
				}
			}

			// Create cap geometry (if some edges to create it from)
			if (CapOption != EProcMeshSliceCapOption::NoCap && ClipEdges.Num() > 0)
			{
				FProcMeshSection CapSection;
				int32 CapSectionIndex = INDEX_NONE;

				// If using an existing section, copy that info first
				if (CapOption == EProcMeshSliceCapOption::UseLastSectionForCap)
				{
					CapSectionIndex = InProcMesh->GetNumSections() - 1;
					CapSection = *InProcMesh->GetProcMeshSection(CapSectionIndex);
				}
				// Adding new section for cap
				else
				{
					CapSectionIndex = InProcMesh->GetNumSections();
				}

				// Project 3D edges onto slice plane to form 2D edges
				TArray<FUtilEdge2D> Edges2D;
				FUtilPoly2DSet PolySet;
				FGeomTools::ProjectEdges(Edges2D, PolySet.PolyToWorld, ClipEdges, SlicePlane);

				// Find 2D closed polygons from this edge soup
				FGeomTools::Buid2DPolysFromEdges(PolySet.Polys, Edges2D, FColor(255, 255, 255, 255));

				// Remember start point for vert and index buffer before adding and cap geom
				int32 CapVertBase = CapSection.ProcVertexBuffer.Num();
				int32 CapIndexBase = CapSection.ProcIndexBuffer.Num();

				// Triangulate each poly
				for (int32 PolyIdx = 0; PolyIdx < PolySet.Polys.Num(); PolyIdx++)
				{
					// Generate UVs for the 2D polygon.
					FGeomTools::GeneratePlanarTilingPolyUVs(PolySet.Polys[PolyIdx], 64.f);

					// Remember start of vert buffer before adding triangles for this poly
					int32 PolyVertBase = CapSection.ProcVertexBuffer.Num();

					// Transform from 2D poly verts to 3D
					Transform2DPolygonTo3D(PolySet.Polys[PolyIdx], PolySet.PolyToWorld, CapSection.ProcVertexBuffer, CapSection.SectionLocalBox);

					// Triangulate this polygon
					TriangulatePoly(CapSection.ProcIndexBuffer, CapSection.ProcVertexBuffer, PolyVertBase, LocalPlaneNormal);
				}

				// Set geom for cap section
				InProcMesh->SetProcMeshSection(CapSectionIndex, CapSection);

				// If creating new section for cap, assign cap material to it
				if (CapOption == EProcMeshSliceCapOption::CreateNewSectionForCap)
				{
					InProcMesh->SetMaterial(CapSectionIndex, CapMaterial);
				}
			}

			// Array of sliced collision shapes
			TArray< TArray<FVector> > SlicedCollision;
			TArray< TArray<FVector> > OtherSlicedCollision;

			UBodySetup* ProcMeshBodySetup = InProcMesh->GetBodySetup();

			for (int32 ConvexIndex = 0; ConvexIndex < ProcMeshBodySetup->AggGeom.ConvexElems.Num(); ConvexIndex++)
			{
				FKConvexElem& BaseConvex = ProcMeshBodySetup->AggGeom.ConvexElems[ConvexIndex];

				int32 BoxCompare = BoxPlaneCompareLimited(BaseConvex.ElemBox, SlicePlane, HoleRadius, EnterPoint);
				// If box totally valid, just keep mesh as is
				if (BoxCompare == 1)
				{
					SlicedCollision.Add(BaseConvex.VertexData);
				}
				// Need to actually slice the convex shape
				else
				{
					TArray<FVector> SlicedConvexVerts;
					SliceConvexElem(BaseConvex, SlicePlane, SlicedConvexVerts);
					// If we got something valid, add it
					if (SlicedConvexVerts.Num() >= 4)
					{
						SlicedCollision.Add(SlicedConvexVerts);
					}
				}
			}

			// Update collision of proc mesh
			InProcMesh->SetCollisionConvexMeshes(SlicedCollision);
		}
	}
}

void UMeshExtensionLibrary::DentProceduralMesh(UProceduralMeshComponent* InProcMesh, FVector Center, float Radius, float ExpFalloff)
{

}
