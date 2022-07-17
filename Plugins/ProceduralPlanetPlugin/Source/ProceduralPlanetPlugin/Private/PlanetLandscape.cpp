// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetLandscape.h"
#include "PlanetQuadTree.h"
#include "ProceduralMeshComponent.h"

UPlanetLandscape::UPlanetLandscape() :
	ShapeGenerator(FPlanetShapeGenerator()),
	MeshSettings(FPlanetMeshSettings()),
	localUp(FVector(0,0,0)),
	axisA(FVector(0, 0, 0)),
	axisB(FVector(0, 0, 0))
{
	Mesh =
		CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
}

void UPlanetLandscape::Initialize(FPlanetShapeGenerator inGenerator, FPlanetMeshSettings inMeshSettings, 
	UProceduralMeshComponent& inMesh, FVector inLocalUp)
{
	this->ShapeGenerator = inGenerator;
	this->MeshSettings = inMeshSettings;
	this->Mesh = &inMesh;
	this->localUp = inLocalUp;
	this->bInitialized = false;

	axisA = FVector(localUp.Y, localUp.Z, localUp.X);
	axisB = FVector::CrossProduct(localUp, axisA);
}

// @desc See Sebastian Leagure (Procedural Planets)
void UPlanetLandscape::ConstructMesh(FVector inPlayerPosition)
{
	QuadTree = new UPlanetQuadTree(MeshSettings, nullptr, inPlayerPosition, localUp, axisA, axisB, 1, 0);
	QuadTree->ConstructQuadTree();

	int triangleOffset = 0;
	for (UPlanetQuadTree* Child : QuadTree->GetVisibleChildren())
	{
		TArray<FVector> inV;
		TArray<int> inT;
		TArray<FColor> inC;

		// UE_LOG(LogTemp, Warning, TEXT("Current Visible Child: %s"), *Child->ToString() );
		Child->CalculateMesh(inV, inT, inC, triangleOffset);

		vertices.Append(inV);
		triangles.Append(inT);
		colors.Append(inC);

		/* Before we would pass parameters in directly
		// But triangleOffset would grow as vertices would grow
		// Causing the last quadtree to dissappear
		*/
		triangleOffset += inV.Num();
	}

	normals.SetNumUninitialized(vertices.Num());
	int i = 0;
	for (const FVector& vert : vertices)
	{
		normals[i++] = vert.GetSafeNormal();
	}

	TArray<struct FProcMeshTangent> tangents;

	if (Mesh)
	{
		Mesh->ClearAllMeshSections();
		Mesh->CreateMeshSection(0, vertices, triangles, normals, uvs, colors, tangents, false);
	}
}

void UPlanetLandscape::CalculateNoise()
{
	TArray<FVector> newVertsPosition;

	newVertsPosition.SetNumUninitialized(vertices.Num());
	ensure(newVertsPosition.Num() == vertices.Num());

	int i = 0;
	for (FVector& Vert : vertices)
	{
		newVertsPosition[i++] = ShapeGenerator.CalculatePointOnPlanet(Vert) * MeshSettings.radius * PlanetSize;
	}

	TArray<struct FProcMeshTangent> tangents;

	if (Mesh)
	{
		Mesh->UpdateMeshSection(0, newVertsPosition, normals, uvs, colors, tangents);
	}
}

