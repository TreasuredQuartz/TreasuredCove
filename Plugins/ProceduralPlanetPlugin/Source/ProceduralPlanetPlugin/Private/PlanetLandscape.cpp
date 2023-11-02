// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetLandscape.h"
#include "PlanetQuadTree.h"
#include "RealtimeMeshComponent.h"
#include "RealtimeMeshSimple.h"

#include "DrawDebugHelpers.h"

UPlanetLandscape::UPlanetLandscape() :
	ShapeGenerator(FPlanetShapeGenerator()),
	MeshSettings(FPlanetMeshSettings()),
	localUp(FVector(0,0,0)),
	axisA(FVector(0,0,0)),
	axisB(FVector(0,0,0))
{
	RuntimeMesh =
		CreateDefaultSubobject<URealtimeMeshComponent>(TEXT("RuntimeMesh"));
}

void UPlanetLandscape::Initialize(FPlanetShapeGenerator inGenerator, FPlanetMeshSettings inMeshSettings, 
	URealtimeMeshComponent& inMesh, FVector inLocalUp)
{
	this->ShapeGenerator = inGenerator;
	this->MeshSettings = inMeshSettings;
	this->RuntimeMesh = &inMesh;
	this->localUp = inLocalUp;
	this->bInitialized = false;

	axisA = FVector(localUp.Y, localUp.Z, localUp.X);
	axisB = FVector::CrossProduct(localUp, axisA);
}

// @desc See Sebastian Leagure (Procedural Planets)
void UPlanetLandscape::ConstructMesh(const FVector& NewLocation)
{
	URealtimeMeshSimple* RTMesh = NewObject<URealtimeMeshSimple>();


	if (RuntimeMesh)
	{
		int32 i, cur;
		if (!MeshSections.IsEmpty())
		{
			for (i = 0; i < MeshSections.Num(); ++i)
			{
				{ // Clear Section
					FRealtimeMeshSectionKey SectionKey;
					FRealtimeMeshSimpleCompletionCallback CompletionCallback;
					RTMesh->RemoveSection(SectionKey, CompletionCallback);
				}
				// RuntimeMesh->ClearSection(0, i);
				MeshSections[i].ClearAllData();
			}
		}
		else
		{
			// RuntimeMesh->RemoveAllSectionsForLOD(0);
		}

		if (QuadTree == nullptr)
		{
			QuadTree = new UPlanetQuadTree(MeshSettings, NewLocation, localUp, axisA, axisB, 1, 0);
			QuadTree->ActorLocation = PlanetLocation;
			QuadTree->ConstructQuadTree();
		}

		// UE_LOG(LogTemp, Warning, TEXT("UPlanetLandscape: \n -- Constructing Mesh! \n\n"), )
		// UE_LOG(LogTemp, Warning, TEXT("UPlanetLandscape: \n ==== Location: %s ===="), *NewLocation.ToString());

		cur = 0;
		const auto& Tree = QuadTree->GetVisibleChildren();
		MeshSections.SetNum(Tree.Num());
		for (const auto& Child : Tree)
		{
			TArray<FVector> inV;
			TArray<int> inT;
			TArray<FColor> inC;

			// UE_LOG(LogTemp, Warning, TEXT("Current Visible Child: %s"), *Child->ToString() );
			Child->CalculateMesh(inV, inT, inC, MeshSections[cur].NumTriangles); // We used to use triangleOffset here before we started using seperate sections for each QuadTree.
		
			// DrawDebugPoint(GetWorld(), PlanetLocation + (Child->position.GetSafeNormal() * MeshSettings.radius), 25, FColor::Green, false, 0.5);
			// DrawDebugPoint(GetWorld(), NewLocation, 50, FColor::Yellow, false, 1);

			MeshSections[cur].Vertices.Append(inV);
			MeshSections[cur].Triangles.Append(inT);
			MeshSections[cur].VertexColors.Append(inC);
			MeshSections[cur].Normals.Append(inV);
			MeshSections[cur].bEnableCollision = Child->detailLevel == 11;

			/* Before we would pass parameters in directly
			// But triangleOffset would grow as vertices would grow
			// Causing the last quadtree to dissappear
			*/
			MeshSections[cur].NumTriangles += inV.Num();
			++cur;
		}

		i = 0;
		for (FPlanetMeshSection MeshSection : MeshSections)
		{
			if (MeshSection.NumTriangles == 0 || MeshSection.Vertices.Num() < 3)
			{
				// UE_LOG(LogTemp, Error, TEXT("UPlanetLandscape: \n -- Mesh Section has no triangles! \n\n"));
			}
			else
			{
				{ // Create Section
					FRealtimeMeshSectionKey SectionKey;
					FRealtimeMeshSectionConfig Config;
					FRealtimeMeshStreamRange StreamRange;
					bool bShouldCreateCollision = true;
					FRealtimeMeshSimpleCompletionCallback CompletionCallback;

					// RTMesh->CreateSection(0, 0, 0, VertexBuffer.GetVertices(), VertexBuffer.GetTriangles(), VertexBuffer.GetNormals(), VertexBuffer.GetUVs(), VertexBuffer.GetColors(), VertexBuffer.GetTangents());
					RTMesh->CreateSection(SectionKey, Config, StreamRange, bShouldCreateCollision, CompletionCallback);
				}
				// RuntimeMesh->CreateSectionFromComponents(0, i, 0,
				//	MeshSection.Vertices, MeshSection.Triangles, MeshSection.Normals, MeshSection.UVs, MeshSection.VertexColors, MeshSection.Tangents,
				//	MeshSection.UpdateFrequency, MeshSection.bEnableCollision);
			}

			++i;
		}
	}
}

void UPlanetLandscape::CalculateNoise()
{
	int32 cur = -1;
	for (FPlanetMeshSection MeshSection : MeshSections)
	{
		TArray<FVector> newVertsPosition;

		newVertsPosition.SetNumUninitialized(MeshSection.Vertices.Num());
		ensure(newVertsPosition.Num() == MeshSection.Vertices.Num());

		int i = -1;
		for (FVector& Vert : MeshSection.Vertices)
		{
			newVertsPosition[++i] = ShapeGenerator.CalculatePointOnPlanet(Vert) * MeshSettings.radius; // * PlanetSize;
		}

		if (RuntimeMesh)
		{
			URealtimeMeshSimple* RTMesh = NewObject<URealtimeMeshSimple>();
			{
				FRealtimeMeshSectionGroupKey SectionGroupKey;
				URealtimeMeshStreamSet* MeshData = nullptr;
				FRealtimeMeshSimpleCompletionCallback CompletionCallback;
				// Mesh->UpdateSectionFromComponents(0, i, MeshSections[i].Vertices, MeshSections[i].Triangles, MeshSections[i].Normals, MeshSections[i].UVs, MeshSections[i].VertexColors, MeshSections[i].Tangents);
				RTMesh->UpdateSectionGroup(SectionGroupKey, MeshData, CompletionCallback);
			}
			// RuntimeMesh->UpdateSectionFromComponents(0, ++cur,
			//	newVertsPosition, MeshSection.Triangles, MeshSection.Normals, MeshSection.UVs, MeshSection.VertexColors, MeshSection.Tangents);
			// Mesh->UpdateMeshSection(0, newVertsPosition, normals, uvs, colors, tangents);
		}
	}
	
}

void UPlanetLandscape::OnCameraLocationUpdated(const FVector& NewLocation)
{
	/*if (!Mesh->WasRecentlyRendered())
	{
		Mesh->ClearAllMeshSections();
	}
	else*/
	{
		if (QuadTree)
		{
			QuadTree->DestructQuadTree();
			QuadTree->PlayerPosition = NewLocation;
			QuadTree->ConstructQuadTree();
		}

		// if (QuadTree->CheckLOD(NewLocation))
		{
			ConstructMesh(NewLocation);
		}
	}
}

