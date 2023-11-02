// Fill out your copyright notice in the Description page of Project Settings.


#include "MCWorld_VoxelItem.h"
#include "MCWorld_Voxel.h"
#include "DTMCBlockInfoStruct.h"
#include "GameplayTileData.h"
#include "Global/Libraries/GALibrary.h"
#include "Global/Components/Items/BobbingMovementComponent.h"

#include "RealtimeMeshComponent.h"
#include "RealtimeMeshSimple.h"

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
	// Rotating
	Rotation =
		CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Item Rotation"));
	// Bobbing
	Bobbing =
		CreateDefaultSubobject<UBobbingMovementComponent>(TEXT("Item Bobbing"));

	SetRootComponent(Root);

	VoxelSize = 200;
	VoxelSizeHalf = VoxelSize / 2;
	Size = bMini ? VoxelSizeHalf / 4 : VoxelSizeHalf / 2;
	SizeHalf = Size / 2;
}

void AMCWorld_VoxelItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (!Mesh)
	{
		Mesh =
			NewObject<URealtimeMeshComponent>(this, FName("VoxelItem_Mesh"));
		FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);

		Mesh->SetupAttachment(RootComponent);
		Mesh->RegisterComponent();
		Mesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	}

	UpdateMesh();
}

void AMCWorld_VoxelItem::BeginPlay()
{
	Rotation->UpdatedComponent = Mesh;

	Super::BeginPlay();

	UpdateMesh();
}

void AMCWorld_VoxelItem::UpdateMesh()
{
	URealtimeMeshSimple* RTMesh = NewObject<URealtimeMeshSimple>();
	if (Mesh)
	{
		if (MeshSection.Vertices.Num() > 2)
		{ // Clear Section
			// Mesh->ClearSection(0, 0);
			FRealtimeMeshSectionKey SectionKey;
			FRealtimeMeshSimpleCompletionCallback CompletionCallback;
			RTMesh->RemoveSection(SectionKey, CompletionCallback);
		}

		MeshSection.Clear();

		int x = PMeshLocation.X;
		int y = PMeshLocation.Y;
		int z = PMeshLocation.Z;

		int Triangle_Num = 0;
		for (int i = 0; i < 6; i++)
		{
			Triangle_Num = UGALibrary::CreateFace(MeshSection, BlockData, FVector(x, y, z), i, Triangle_Num, Size, 1);
		}
		MeshSection.ElementID += Triangle_Num;

		{ // Create Section
			FRealtimeMeshSectionKey SectionKey;
			FRealtimeMeshSectionConfig Config;
			FRealtimeMeshStreamRange StreamRange;
			bool bShouldCreateCollision = true;
			FRealtimeMeshSimpleCompletionCallback CompletionCallback;

			// RTMesh->CreateSection(0, 0, 0, VertexBuffer.GetVertices(), VertexBuffer.GetTriangles(), VertexBuffer.GetNormals(), VertexBuffer.GetUVs(), VertexBuffer.GetColors(), VertexBuffer.GetTangents());
			RTMesh->CreateSection(SectionKey, Config, StreamRange, bShouldCreateCollision, CompletionCallback);
		}
		// Mesh->CreateSectionFromComponents(0, 0, 0, MeshSection.Vertices, MeshSection.Triangles, MeshSection.Normals, MeshSection.UVs, MeshSection.VertexColors, MeshSection.Tangents, ERuntimeMeshUpdateFrequency::Infrequent, true);
		Mesh->SetMaterial(0, Material);
	}
}

void AMCWorld_VoxelItem::AddTriangle(TArray < int32 > &Triangles, int32 V1, int32 V2, int32 V3)
{
	Triangles.Add(V1);
	Triangles.Add(V2);
	Triangles.Add(V3);
}
