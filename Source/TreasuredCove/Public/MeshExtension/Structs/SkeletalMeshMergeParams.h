// Fill out your copyright notice in the Description page of Project Settings.

//#pragma once
//
//#include "CoreMinimal.h"
//
//#include "SkeleMeshMergeSectionMapping_BP.h"
//#include "SkelMeshMergeUVTransform.h"
//#include "SkelMeshMergeUVTransformMapping.h"
//
//#include "SkeletalMeshMergeParams.generated.h"
//
//
//
///**
//* Struct containing all parameters used to perform a Skeletal Mesh Merge
//*/
//USTRUCT(BlueprintType)
//struct TREASUREDCOVE_API FSkeletalMeshMergeParams
//{
//	GENERATED_BODY()
//	
//		FSkeletalMeshMergeParams()
//	{
//		MeshSectionMappings.SetNumUninitialized(0);
//		UVTransformsPerMesh.SetNumUninitialized(0);
//		StripLODS = 0;
//		bNeedsCPUAccess = false;
//		bSkeletonBefore = false;
//		Skeleton = nullptr;
//	}
//
//	// An optional array to map sections from the source meshes to merged section entries.
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FSkeleMeshMergeSectionMapping_BP> MeshSectionMappings;
//
//	// An optional array to transform the UVs in each mesh.
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FSkelMeshMergeUVTransformMapping> UVTransformsPerMesh;
//
//	// The List of Skeletal Meshes to Merge.
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<class USkeletalMesh*> MeshesToMerge;
//
//	// The Number of high LODs to remove from input meshes.
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 StripLODS;
//
//	// whether or not the resulting mesh needs to be accessed by the CPU for any reason (e.g. for spawing particle effects).
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		uint32 bNeedsCPUAccess : 1;
//
//	// Update Skeleton before merge. Otherwise, update after.
//	// Skeleton must also be provided
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		uint32 bSkeletonBefore : 1;
//
//	// Skeleton that will be used for the merged mesh.
//	// Leave empty if the generated skeleton is ok.
//	UPROPERTY(EditAnywhere, BlueprintReadOnly)
//		class USkeleton* Skeleton;
//};
