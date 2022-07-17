// Fill out your copyright notice in the Description page of Project Settings.


//#include "MeshExtensionLibrary.h"
//#include "SkeletalMeshMerge.h"
//#include "Engine/SkeletalMeshSocket.h"
//#include "Engine/SkeletalMesh.h"
//#include "Animation/Skeleton.h"
//#include "Engine/Engine.h"
//#include "Math/Range.h"
//#include "MeshExtension/Delaunauy.h"
//
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