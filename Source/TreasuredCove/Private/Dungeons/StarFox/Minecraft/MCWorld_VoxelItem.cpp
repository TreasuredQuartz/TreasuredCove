// Fill out your copyright notice in the Description page of Project Settings.


#include "MCWorld_VoxelItem.h"
#include "MCWorld_Voxel.h"

#include "Materials/MaterialInterface.h"
#include "Gameframework/RotatingMovementComponent.h"

const int32 PTriangles[] = { 2, 1, 0, 0, 3, 2 };
const FVector2D PUVs[] = { FVector2D(0.00000, 0.00000), FVector2D(0.00000, 1.00000), FVector2D(1.00000, 1.00000), FVector2D(1.00000, 0.00000) };
const FVector PNormals0[] = { FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1) };
const FVector PNormals1[] = { FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1) };
const FVector PNormals2[] = { FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0) };
const FVector PNormals3[] = { FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0) };
const FVector PNormals4[] = { FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0) };
const FVector PNormals5[] = { FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0) };

AMCWorld_VoxelItem::AMCWorld_VoxelItem()
{
	Mesh = 
		CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("VoxelMesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);

	Rotation =
		CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Item Rotation"));
	Rotation->UpdatedComponent = Mesh;

	VoxelSize = 200;
}

void AMCWorld_VoxelItem::OnConstruction(const FTransform& Transform)
{
	if (!GetMesh<UProceduralMeshComponent>()) return;

	VoxelSizeHalf = VoxelSize / 2;

	if (bMini)
	{
		Size = VoxelSizeHalf / 2;
	}
	else
	{
		Size = VoxelSizeHalf;
	}

	SizeHalf = Size / 2;

	if (!Material) return;

	UpdateMesh();
}

void AMCWorld_VoxelItem::BeginPlay()
{
	UpdateMesh();
}

void AMCWorld_VoxelItem::UpdateMesh()
{
	FProceduralMeshSection MeshSection;

	TArray < FVector > &Vertices = MeshSection.Vertices;
	TArray < int32 > &Triangles = MeshSection.Triangles;
	TArray < FVector > &Normals = MeshSection.Normals;
	TArray < FVector2D > &UVs = MeshSection.UVs;
	TArray < FProcMeshTangent > &Tangents = MeshSection.Tangents;
	TArray< FColor > &VertexColors = MeshSection.VertexColors;
	int32 ElementID = 0;

	int x = PMeshLocation.X;
	int y = PMeshLocation.Y;
	int z = PMeshLocation.Z;

	int Triangle_Num = 0;
	for (int i = 0; i < 6; i++)
	{
		AddTriangle(Triangles, PTriangles[0] + Triangle_Num + ElementID, PTriangles[1] + Triangle_Num + ElementID, PTriangles[2] + Triangle_Num + ElementID);
		AddTriangle(Triangles, PTriangles[3] + Triangle_Num + ElementID, PTriangles[4] + Triangle_Num + ElementID, PTriangles[5] + Triangle_Num + ElementID);

		Triangle_Num += 4;

		// i represents each face of the new voxel we are generating
		switch (i)
		{
		case 0: {
			// Top Face
			Vertices.Add(FVector(-SizeHalf + (x * Size), SizeHalf + (y * Size), SizeHalf + (z * Size)));
			Vertices.Add(FVector(-SizeHalf + (x * Size), -SizeHalf + (y * Size), SizeHalf + (z * Size)));
			Vertices.Add(FVector(SizeHalf + (x * Size), -SizeHalf + (y * Size), SizeHalf + (z * Size)));
			Vertices.Add(FVector(SizeHalf + (x * Size), SizeHalf + (y * Size), SizeHalf + (z * Size)));

			Normals.Append(PNormals0, UE_ARRAY_COUNT(PNormals0));
			break;
		}
		case 1: {
			// Bottom Face
			Vertices.Add(FVector(SizeHalf + (x * Size), -SizeHalf + (y * Size), -SizeHalf + (z * Size)));
			Vertices.Add(FVector(-SizeHalf + (x * Size), -SizeHalf + (y * Size), -SizeHalf + (z * Size)));
			Vertices.Add(FVector(-SizeHalf + (x * Size), SizeHalf + (y * Size), -SizeHalf + (z * Size)));
			Vertices.Add(FVector(SizeHalf + (x * Size), SizeHalf + (y * Size), -SizeHalf + (z * Size)));

			Normals.Append(PNormals1, UE_ARRAY_COUNT(PNormals1));
			break;

		}
		case 2: {
			// Right Face
			Vertices.Add(FVector(SizeHalf + (x * Size), SizeHalf + (y * Size), SizeHalf + (z * Size)));
			Vertices.Add(FVector(SizeHalf + (x * Size), SizeHalf + (y * Size), -SizeHalf + (z * Size)));
			Vertices.Add(FVector(-SizeHalf + (x * Size), SizeHalf + (y * Size), -SizeHalf + (z * Size)));
			Vertices.Add(FVector(-SizeHalf + (x * Size), SizeHalf + (y * Size), SizeHalf + (z * Size)));

			Normals.Append(PNormals2, UE_ARRAY_COUNT(PNormals2));
			break;
		}
		case 3: {
			// Left Face
			Vertices.Add(FVector(-SizeHalf + (x * Size), -SizeHalf + (y * Size), SizeHalf + (z * Size)));
			Vertices.Add(FVector(-SizeHalf + (x * Size), -SizeHalf + (y * Size), -SizeHalf + (z * Size)));
			Vertices.Add(FVector(SizeHalf + (x * Size), -SizeHalf + (y * Size), -SizeHalf + (z * Size)));
			Vertices.Add(FVector(SizeHalf + (x * Size), -SizeHalf + (y * Size), SizeHalf + (z * Size)));

			Normals.Append(PNormals3, UE_ARRAY_COUNT(PNormals3));
			break;
		}
		case 4: {
			// Front Face
			Vertices.Add(FVector(SizeHalf + (x * Size), -SizeHalf + (y * Size), SizeHalf + (z * Size)));
			Vertices.Add(FVector(SizeHalf + (x * Size), -SizeHalf + (y * Size), -SizeHalf + (z * Size)));
			Vertices.Add(FVector(SizeHalf + (x * Size), SizeHalf + (y * Size), -SizeHalf + (z * Size)));
			Vertices.Add(FVector(SizeHalf + (x * Size), SizeHalf + (y * Size), SizeHalf + (z * Size)));

			Normals.Append(PNormals4, UE_ARRAY_COUNT(PNormals4));
			break;
		}
		case 5: {
			// Back Face
			Vertices.Add(FVector(-SizeHalf + (x * Size), SizeHalf + (y * Size), SizeHalf + (z * Size)));
			Vertices.Add(FVector(-SizeHalf + (x * Size), SizeHalf + (y * Size), -SizeHalf + (z * Size)));
			Vertices.Add(FVector(-SizeHalf + (x * Size), -SizeHalf + (y * Size), -SizeHalf + (z * Size)));
			Vertices.Add(FVector(-SizeHalf + (x * Size), -SizeHalf + (y * Size), SizeHalf + (z * Size)));

			Normals.Append(PNormals5, UE_ARRAY_COUNT(PNormals5));
			break;
		}
		}
		UVs.Append(PUVs, UE_ARRAY_COUNT(PUVs));

		FColor Color = FColor(255, 255, 255, i);
		VertexColors.Add(Color);
		VertexColors.Add(Color);
		VertexColors.Add(Color);
		VertexColors.Add(Color);
	}
	ElementID += Triangle_Num;

	GetMesh<UProceduralMeshComponent>()->ClearAllMeshSections();

	GetMesh<UProceduralMeshComponent>()->CreateMeshSection(0, MeshSection.Vertices, MeshSection.Triangles, MeshSection.Normals, MeshSection.UVs, MeshSection.VertexColors, MeshSection.Tangents, true);

	GetMesh<UProceduralMeshComponent>()->SetMaterial(0, Material);
}

void AMCWorld_VoxelItem::AddTriangle(TArray < int32 > &Triangles, int32 V1, int32 V2, int32 V3)
{
	Triangles.Add(V1);
	Triangles.Add(V2);
	Triangles.Add(V3);
}
