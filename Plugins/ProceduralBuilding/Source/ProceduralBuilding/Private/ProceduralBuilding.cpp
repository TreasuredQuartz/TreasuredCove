// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralBuilding.h"

// Sets default values
UProceduralBuilding::UProceduralBuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = false;
	// Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Real Mesh"));

	NumFloors = 1;
	NumWidth = 1;
	NumDepth = 1;

	ZTile = 1;
	YTile = 1;
	XTile = 1;

	WallSize = 16;
	WallSizeHalf = 8;
	WallSizeQuarter = 4;
}

void UProceduralBuilding::UpdateMesh(const FTransform& Transform)
{
	UStaticMesh* wallMesh = nullptr;
	// TArray<FProceduralMeshSection> MeshSections;

	// Front Side
	for (int32 ZIndex = 0; NumFloors - 1 > ZIndex; ZIndex++)
	for (int32 XIndex = 0; NumWidth - 1 > XIndex; XIndex++)
	{
		if (ZIndex == 0)
		{
			// First Floor
			if (XIndex == DoorPosition - 1)
			{
				// Door
				wallMesh = DoorMesh;
			}
			else
			{
				// First Floor walls
				wallMesh = FirstFloorWallMesh;
			}
		}
		else if (ZIndex == NumFloors - 1)
		{
			// Last Floor
			wallMesh = RoofWallMesh;
		}
		else
		{
			// Every floor except the first and last
			wallMesh = DefaultWallMesh;
		}

		if (!wallMesh) return;
		// GetVerticesFromStaticMesh(MeshSections[0].Vertices, FTransform(FRotator(), FVector(XTile * XIndex, 0, ZTile * ZIndex), FVector()), wallMesh);
	}

	// Right Side
	for (int32 ZIndex = 0; NumFloors - 1 > ZIndex; ZIndex++)
	for (int32 YIndex = 0; NumDepth - 1 > YIndex; YIndex++)
	{
		if (ZIndex == 0)
		{
			// First Floor
			wallMesh = FirstFloorWallMesh;
		}
		else if (ZIndex == NumFloors - 1)
		{
			// Last Floor
			wallMesh = RoofWallMesh;
		}
		else
		{
			// Every floor except the first and last
			wallMesh = DefaultWallMesh;
		}

		if (!wallMesh) return;

		// GetVerticesFromStaticMesh(MeshSections[0].Vertices, FTransform(FRotator(0, 0, 90), FVector(XTile * (NumWidth - 1), YTile * YIndex, ZTile * ZIndex), FVector()), wallMesh);
	}

	// Left Side
	for (int32 ZIndex = 0; NumFloors - 1 > ZIndex; ZIndex++)
	for (int32 YIndex = 0; NumDepth - 1 > YIndex; YIndex++)
	{
		if (ZIndex == 0)
		{
			// First Floor
			wallMesh = FirstFloorWallMesh;
		}
		else if (ZIndex == NumFloors - 1)
		{
			// Last Floor
			wallMesh = RoofWallMesh;
		}
		else
		{
			// Every floor except the first and last
			wallMesh = DefaultWallMesh;
		}

		if (!wallMesh) return;

		// GetVerticesFromStaticMesh(MeshSections[0].Vertices, FTransform(FRotator(0, 0, -90), FVector(0, YTile * YIndex, ZTile * ZIndex), FVector()), wallMesh);
	}

	// Back Side
	for (int32 ZIndex = 0; NumFloors - 1 > ZIndex; ZIndex++)
	for (int32 XIndex = 0; NumWidth - 1 > XIndex; XIndex++)
	{
		if (ZIndex == 0)
		{
			// First Floor
			if (XIndex == DoorPosition - 1)
			{
				// Door
				wallMesh = DoorMesh;
			}
			else
			{
				// First Floor walls
				wallMesh = FirstFloorWallMesh;
			}
		}
		else if (ZIndex == NumFloors - 1)
		{
			// Last Floor
			wallMesh = RoofWallMesh;
		}
		else
		{
			// Every floor except the first and last
			wallMesh = DefaultWallMesh;
		}

		if (!wallMesh) return;
		// GetVerticesFromStaticMesh(MeshSections[0].Vertices, FTransform(FRotator(), FVector(XTile * XIndex, YTile * (NumDepth - 1), ZTile * ZIndex), FVector()), wallMesh);
	}

	/*
	for (int32 z = 0; NumFloors - 1 > z; z++)
	for (int32 y = 0; NumDepth - 1 > y; y++)
	for (int32 x = 0; NumWidth - 1 > x; x++)
	{
		int Elm_Num = 0;
		int Triangle_Num = 0;
		for (int i = 0; i < 6; i++)
		{
			MeshSections[0].Vertices.Add(FVector(WallSizeHalf + (x * WallSize), -WallSizeHalf + (y * WallSize), WallSizeHalf + (z * WallSize)));
			MeshSections[0].Vertices.Add(FVector(WallSizeHalf + (x * WallSize), -WallSizeHalf + (y * WallSize), -WallSizeHalf + (z * WallSize)));
			MeshSections[0].Vertices.Add(FVector(WallSizeHalf + (x * WallSize), WallSizeHalf + (y * WallSize), -WallSizeHalf + (z * WallSize)));
			MeshSections[0].Vertices.Add(FVector(WallSizeHalf + (x * WallSize), WallSizeHalf + (y * WallSize), WallSizeHalf + (z * WallSize)));

			const FVector VoxelNormals4[] = { FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0) };
			const FVector2D VoxelUVs[] = { FVector2D(0.00000, 0.00000), FVector2D(0.00000, 1.00000), FVector2D(1.00000, 1.00000), FVector2D(1.00000, 0.00000) };
			MeshSections[0].Normals.Append(VoxelNormals4, UE_ARRAY_COUNT(VoxelNormals4));
			MeshSections[0].UVs.Append(VoxelUVs, UE_ARRAY_COUNT(VoxelUVs));

			FColor Color = FColor(255, 255, 255, i);
			MeshSections[0].VertexColors.Add(Color);
			MeshSections[0].VertexColors.Add(Color);
			MeshSections[0].VertexColors.Add(Color);
			MeshSections[0].VertexColors.Add(Color);
		}
		Elm_Num += Triangle_Num;
		MeshSections[0].ElementID += Triangle_Num;
	}

	// For each MeshSection, Create a new mesh section
	for (int i = 0; i < MeshSections.Num(); i++)
	{
		if (MeshSections[i].Vertices.Num() > 0)
		{
			Mesh->CreateMeshSection(i, MeshSections[i].Vertices, MeshSections[i].Triangles, MeshSections[i].Normals, MeshSections[i].UVs, MeshSections[i].VertexColors, MeshSections[i].Tangents, false);
			Mesh->SetMaterial(i, MeshSections[i].Material);
		}
	}
	// */
}

void UProceduralBuilding::GetVerticesFromStaticMesh(TArray<FVector> &OutVertices, FTransform Transform, UStaticMesh* InMesh)
{
	if (!InMesh) return;
	if (!(InMesh->GetRenderData()->LODResources.Num() > 0)) return;

	// FTangentsVertexBuffer* TangentBuffer = &InMesh->RenderData->LODResources[0].VertexBuffers.StaticMeshVertexBuffer.TangentsVertexBuffer;
	FPositionVertexBuffer* VertexBuffer = &InMesh->GetRenderData()->LODResources[0].VertexBuffers.PositionVertexBuffer;
	if (!VertexBuffer) return;

	const int32 VertexCount = VertexBuffer->GetNumVertices();
	for (int32 Index = 0; Index < VertexCount; Index++)
	{
		const FVector VertexLocation = Transform.GetLocation(); // + GetTransform().TransformVector(FVector3d(VertexBuffer->VertexPosition(Index)));
		OutVertices.Add(VertexLocation);
	}

	return;
}

