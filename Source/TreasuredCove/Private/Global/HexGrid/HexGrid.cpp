// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/HexGrid/HexGrid.h"
#include "Global/HexGrid/HexTile.h"
#include "Kismet/KismetMathLibrary.h"
#include "SimplexNoiseBPLibrary.h"
#include "Global/Libraries/GALibrary.h"
#include "GameplayTileData.h"

// #include "RealtimeMeshDataTypes.h"
#include "RealtimeMeshComponent.h"
#include "RealtimeMeshSimple.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Sampling/SphericalFibonacci.h"

static const FVector CubeVoxelMask[] = {
	FVector(0.00000, 0.00000, 1.00000),
	FVector(0.00000, 0.00000, -1.00000),
	FVector(0.00000, 1.00000, 0.00000),
	FVector(0.00000, -1.00000, 0.00000),
	FVector(1.00000, 0.00000, 0.00000),
	FVector(-1.00000, 0.00000, 0.00000)
};

static const FVector2D HexMask[2][6] = {
	// even cols
	{FVector2D(1, 1),FVector2D(1, 0),FVector2D(0, -1),
	FVector2D(-1, 0),FVector2D(-1, 1),FVector2D(0, 1)},
	// odd cols
	{FVector2D(1,0),FVector2D(1, -1),FVector2D(0, -1),
	FVector2D(-1, -1),FVector2D(-1, 0),FVector2D(0, 1)}
};

// Sets default values
AHexGrid::AHexGrid()
{
	VoxelSize = 200;
	ChunkLineElements = 32;
	SurfaceHeight = 30;

	VerticalOffset = (VoxelSize * .75);
	HorizontalOffset = (DOUBLE_UE_SQRT_3 * (double)VoxelSize);
	OddHorizontalOffset = ((DOUBLE_UE_SQRT_3 * (double)VoxelSize) * .5);

	Scale3D = FVector(0.2, 0.2, 0.1);

	Scene =
		CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(Scene);

	MeshCollisionInstances = 
		CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Collision Meshes"));
	MeshCollisionInstances->SetupAttachment(RootComponent);

	Mesh =
		CreateDefaultSubobject<URealtimeMeshComponent>(TEXT("RealtimeMeshComponent"));
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	Mesh->SetupAttachment(RootComponent);
}

AHexGrid::~AHexGrid()
{
}

void AHexGrid::PostLoad()
{
	Super::PostLoad();
}

void AHexGrid::PostActorCreated()
{
	Super::PostActorCreated();
}

void AHexGrid::Destroyed()
{
	Super::Destroyed();
}

void AHexGrid::PreRegisterAllComponents()
{
	Super::PreRegisterAllComponents();
}

void AHexGrid::PostUnregisterAllComponents()
{
	Super::PostUnregisterAllComponents();
}

#if WITH_EDITOR
void AHexGrid::PostEditUndo()
{
	Super::PostEditUndo();
}

void AHexGrid::OnConstruction(const FTransform& Transform)
{
	Initialize(RandomSeed, VoxelSize, ChunkLineElements, ChunkIndex);

	if (GetWorld()) RootComponent->SetWorldTransform(Transform);

	Super::OnConstruction(Transform);
}

void AHexGrid::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	if (e.Property == NULL) return;

	if (e.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AHexGrid, ChunkLineElements) || 
		e.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AHexGrid, ChunkLineElements) || 
		e.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AHexGrid, VerticalOffset) || 
		e.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AHexGrid, HorizontalOffset) || 
		e.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AHexGrid, OddHorizontalOffset))
	{
		bShouldInitializeNoise = true;
		bShouldInitializeTiles = true;
	}
	else if (e.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AHexGrid, Types))
	{
		bShouldInitializeNoise = true;
	}

	// Initialize(RandomSeed, VoxelSize, ChunkLineElements, ChunkIndex);

	Super::PostEditChangeProperty(e);
}
#endif

void AHexGrid::ExecuteRebuildGeneratedMeshIfPending()
{
	if (!bGeneratedMeshRebuildPending ||
		!IsValid(Mesh))
	{
		return;
	}

	if (bResetOnRebuild)
	{
		Mesh->SetRealtimeMesh(nullptr);
	}

	FEditorScriptExecutionGuard Guard;

	OnGenerateMesh();

	bGeneratedMeshRebuildPending = false;
}

void AHexGrid::Tick(float DeltaTime)
{
	if (IsValid(this) && IsValid(GetLevel()))
	{
		ExecuteRebuildGeneratedMeshIfPending();
	}
}

void AHexGrid::BeginPlay()
{
	Initialize(RandomSeed, VoxelSize, ChunkLineElements, ChunkIndex);
	
	Super::BeginPlay();
}

void AHexGrid::Initialize(int32 inRandomSeed, int32 inVoxelSize, int32 inChunkLineElements, FIntVector inChunkIndex)
{
	if (!this) return;

	UE_LOG(LogTemp, Warning, TEXT("Intialize..."));
	AGameplayVoxel::Initialize(inRandomSeed, inVoxelSize, inChunkLineElements, inChunkIndex);

	ChunkLineElementsP3 = ChunkLineElementsP2 * ChunkLineElements;

	// GenerateChunk();
	GenerateHexGrid();
	// GenerateHexSphere();
	OnGenerateMesh();
	// ConstructMesh();

	// bGeneratedMeshRebuildPending = true;
}

void AHexGrid::InitializeNoise()
{
	USimplexNoiseBPLibrary::setNoiseSeed(RandomSeed);
	Height = CalcHeight();
	Types = CalcTypes();

	// Set properties and initialize grid
	if (Types.Num() <= 0 || bShouldInitializeNoise)
	{
		
		// bShouldInitializeNoise = false;
	}
}

void AHexGrid::InitializeTiles()
{
	if (Tiles.Num() <= 0 || bShouldInitializeTiles)
	{
		Tiles.Empty(ChunkTotalElements);
		TileLocations.Empty(ChunkTotalElements);
		
		// bShouldInitializeTiles = false;
	}
}

void AHexGrid::BeforeChunkGenerated()
{
	MeshCollisionInstances->ClearInstances();
	InitializeNoise();
	InitializeTiles();
}

void subdivide(const FVector3d& v1, const FVector3d& v2, const FVector3d& v3, TArray<FVector3d>& spherePoints, const unsigned int depth) {
	if (depth == 0) {
		spherePoints.Push(v1);
		spherePoints.Push(v2);
		spherePoints.Push(v3);
		return;
	}
	const FVector3d v12 = (v1 + v2).GetUnsafeNormal();
	const FVector3d v23 = (v2 + v3).GetUnsafeNormal();
	const FVector3d v31 = (v3 + v1).GetUnsafeNormal();
	subdivide(v1, v12, v31, spherePoints, depth - 1);
	subdivide(v2, v23, v12, spherePoints, depth - 1);
	subdivide(v3, v31, v23, spherePoints, depth - 1);
	subdivide(v12, v23, v31, spherePoints, depth - 1);
}

void initialize_sphere(TArray<FVector3d>& spherePoints, const unsigned int depth) {
	const double X = 0.525731112119133606;
	const double Z = 0.850650808352039932;
	const FVector3d vdata[12] = {
		{-X, 0.0, Z}, { X, 0.0, Z }, { -X, 0.0, -Z }, { X, 0.0, -Z },
		{ 0.0, Z, X }, { 0.0, Z, -X }, { 0.0, -Z, X }, { 0.0, -Z, -X },
		{ Z, X, 0.0 }, { -Z, X, 0.0 }, { Z, -X, 0.0 }, { -Z, -X, 0.0 }
	};
	int tindices[20][3] = {
		{0, 4, 1}, { 0, 9, 4 }, { 9, 5, 4 }, { 4, 5, 8 }, { 4, 8, 1 },
		{ 8, 10, 1 }, { 8, 3, 10 }, { 5, 3, 8 }, { 5, 2, 3 }, { 2, 7, 3 },
		{ 7, 10, 3 }, { 7, 6, 10 }, { 7, 11, 6 }, { 11, 0, 6 }, { 0, 1, 6 },
		{ 6, 1, 10 }, { 9, 0, 11 }, { 9, 11, 2 }, { 9, 2, 5 }, { 7, 2, 11 }
	};
	for (int i = 0; i < 20; i++)
		subdivide(vdata[tindices[i][0]], vdata[tindices[i][1]], vdata[tindices[i][2]], spherePoints, depth);

	/*
	for (uint8 i = 0; i < depth; ++i)
	{
		// Local Declarations
		double x = 0;
		double y = 0;
		double z = 0;
		double yDelta = 0;
		double curRatio = 0;

		// Logic

		y = 1 - 2 * ((i + .5f) / (depth - 1));

		yDelta = FMath::Sqrt(1 - FMath::Square(y));

		curRatio = i * UE_DOUBLE_GOLDEN_RATIO;

		x = FMath::Cos(curRatio) * yDelta;
		z = FMath::Sin(curRatio) * yDelta;

		spherePoints.Add(FVector(x, y, z));
	} // */
}

void AHexGrid::GenerateHexSphere()
{
	BeforeChunkGenerated();
	/*const auto Half = ChunkLineElements * 0.5;
	const double F[] = {
		3.0 / 2.0, 0.0, sqrt(3.0) / 2.0, sqrt(3.0),
		2.0 / 3.0, 0.0, -1.0 / 3.0, sqrt(3.0) / 3.0,
		0.0
	};

	int i = 0;
	for (int q = -Half; q < Half; ++q)
	{
		int r1 = FMath::Max(-Half, -q - Half);
		int r2 = FMath::Min(Half, -q + Half);
		for (int r = r1; r <= r2; ++r)
		{
			Tiles[i] = AddTile(FVector(F[0] * q + F[1] * r, F[2] * q + F[3] * r, Height[i]), Types[i]);
			++i;
		}
	}*/

	UE_LOG(LogTemp, Warning, TEXT("===== Hex Grid Generating ======"));

	TArray<FVector3d> spherePoints;
	initialize_sphere(spherePoints, 3);  // where DEPTH should be the subdivision depth
	int i = 0;
	for (const FVector3d& point : spherePoints)
	{
		/*if (!Tiles.IsValidIndex(i) || !Types.IsValidIndex(i)) 
			break;*/

		const FVector3d point_tmp = point * 2;
		Tiles.Add(AddTile(point_tmp, i));

		// UE_LOG(LogTemp, Warning, TEXT("Sphere Points for loop: %d"), i);
		++i;
	}

	if (i != 0 && Tiles.IsValidIndex(--i))
		UE_LOG(LogTemp, Warning, TEXT("===== Hex Grid Generation Success! ======"));

	AfterChunkGenerated();
}

const double GetInnerRadius(const double outerRadius) {
	return outerRadius * 0.866025404;
}

void AHexGrid::GenerateHexGrid()
{
	// We need three vectors that are perpendicular to the normal Vector.

	BeforeChunkGenerated();

	// Mesh->SetRelativeRotation(NormalVector.ToOrientationQuat());

	int32 x, y, z, i;
	// double Total;
	const FVector Position = NormalVector;
	FVector axisA;
	FVector axisB;
	FVector axisC;

	// Built-In utility function for returning a perpendicular plane to the given Vector
	NormalVector.FindBestAxisVectors(axisA, axisB);

	// axisA and axisB are 90 degrees apart when each vector should be 30 degrees apart.
	axisA = axisA.RotateAngleAxis(60.0f, NormalVector);
	axisC = axisA.RotateAngleAxis(60.0f, NormalVector);
	axisB = axisB.ProjectOnToNormal(NormalVector);

	axisA.Normalize();
	axisB.Normalize();
	axisC.Normalize();

	/* i = 0;
	int32 Total = ChunkLineElements - 1 > 0 ? ChunkLineElements - 1 : 1; // Don't Divide by Zero
	int32 Half = ChunkLineElements;
	for (z = -Half; z <= Half; ++z)
	{
		double LocZ = ((z * 1.0) / (Half * 1.0));
		for (y = -Half; y <= Half; ++y)
		{
			double LocY = ((y * 1.0) / (Half * 1.0));
			for (x = -Half; x <= Half; ++x)
			{
				// z = (-x - y);
				if (x + y + z != 0) continue;

				double LocX = ((x * 1.0) / (Half * 1.0));
				FVector Offset = 
					(LocX * axisA) +
					(LocY * axisB) +
					(LocZ * axisC) * 10 * VoxelSize;

				FVector pointOnUnitCube =
					(Position * 2 + Offset);

				FVector Pos = pointOnUnitCube.GetSafeNormal() * 2;

				if (!TileLocations.Contains(Pos))
				{
					Tiles.Add(AddTile(Pos, i));
					TileLocations.Add(Pos);
					++i;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Tried to add: %s \n But it was already added."), *Pos.ToString());
				}
			}
		}
	} // */

	i = 0;
	int32 N = ChunkLineElements;
	for (x = -N; x <= N; x++) {
		int32 y1 = FMath::Max(-N, -x - N);
		int32 y2 = FMath::Min(N, -x + N);
		for (y = y1; y <= y2; y++) {
			z = -x - y;

			auto CorrectSpacing = [](int32 value, int32 max) {
				double mod = 1.7;
				return FMath::Lerp(value * 1.0, value > 0 ? (value * mod - FMath::Log2(value + 1.0)) : (value * mod + FMath::Log2(-value + 1.0)), FMath::Abs(value / (max * 1.0)));
			};

			double LocX = CorrectSpacing(x, N);
			double LocY = CorrectSpacing(y, N);
			double LocZ = CorrectSpacing(z, N);
			FVector Offset =
				// FVector(col * axis * 0.1 + axis * row * 0.1);
				// FVector(x * 0.05, y * 0.05, z * 0.05);
				// FVector(x, y, z).ProjectOnToNormal(NormalVector.GetSafeNormal());
				// FVector::PointPlaneProject(FVector(x, y, z), FPlane(axisA, axisB, axisC));
				// FVector::PointPlaneProject(FVector(x * 1.0, y * 1.0, z * 1.0), FPlane(NormalVector));
				(LocX * axisA) +
				(LocY * axisB) +
				(LocZ * axisC); // */

			FVector pointCube =
				(Position * N * 1.4 + Offset);

			FVector pointSphere =
				pointCube.GetSafeNormal() * N * 2;

			FVector Pos = pointSphere;

			if (!TileLocations.Contains(Pos))
			{
				Tiles.Add(AddTile(Pos, i));
				TileLocations.Add(Pos);
				++i;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Tried to add: %s \n But it was already added."), *Pos.ToString());
			}
		}
	} // */

	/* i = 0;
	z = 0;
	for (x = 0; x < ChunkLineElements; x++)
	{
		for (y = 0; y < ChunkLineElements; y++)
		{
			FVector position;

			double size = VoxelSize;
			position.X = x * 10.0; // ((double)x + ((double)y * 0.5) - ((double)y / 2)) * (GetInnerRadius(size) * 2.0);
			position.Y = y * 10.0; // (double)y * (size * 1.5);
			position.Z = 0.0;

			FVector Pos = position;

			if (!TileLocations.Contains(Pos))
			{
				Tiles.Add(AddTile(Pos, i));
				TileLocations.Add(Pos);
				++i;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Tried to add: %s \n But it was already added."), *Pos.ToString());
			}
		}
	} // */
	

	AfterChunkGenerated();
}

void AHexGrid::ChunkGenerating(const FIntVector& CurrentLocation, int32 Index)
{
	const int Half = (ChunkLineElementsExt * 0.5);
	const int x = CurrentLocation.X - Half;
	const int y = CurrentLocation.Y - Half;
	const int z = CurrentLocation.Z; // -(ChunkZElements * 0.5);
	const int i = Index;

	// const int r = FMath::Max(-ChunkLineElements);
	if (!Tiles.IsValidIndex(i) || !Types.IsValidIndex(i)) return;
	if (x > Half * 0.5 || x < -Half * 0.5) return;
	if (CurrentLocation.Z > (SurfaceHeight + Height[i])) Types[i] = Types[i];

	const bool OddRow = y & 1;
	// FVector Percent = FVector(x, y, z) / Total;
	// double LocY = ((Percent.X - 0.5f) * 2) * VerticalOffset;
	// double LocX = ((Percent.Y - 0.5f) * 2) * HorizontalOffset + (OddHorizontalOffset * OddRow);
	/*FVector pointOnUnitCube = (Position +
		((LocX * Scale3D.X * axisA) +
			(LocY * Scale3D.Y * axisB)) * VoxelSize);
			
	FVector axisA = FVector(ChunkIndex.Y, ChunkIndex.Z, ChunkIndex.X);
	FVector axisB = FVector::CrossProduct(FVector(ChunkIndex), axisA);
	FVector axisC = FVector();

	/*FVector Pos = FVector(ChunkIndex) +
		(yPos * axisA) +
		(xPos * axisB) * 100;

	Tiles.Add(AddTile(Pos.GetSafeNormal(), i));
	*/
}

void AHexGrid::AfterChunkGenerated()
{

}

void AHexGrid::OnGenerateMesh()
{
	if (bResetOnRebuild)
	{
		Mesh->SetRealtimeMesh(nullptr);
	}

	ConstructMesh();
}

void AHexGrid::ConstructMesh()
{
	RTMesh = Mesh->InitializeRealtimeMesh<URealtimeMeshSimple>();

	TArray<FProceduralMeshSection> MeshSections;
	MeshSections.SetNum(6);

	if (Tiles.IsEmpty())
		UE_LOG(LogTemp, Warning, TEXT("No Tiles"));

	for (UHexTile* Tile : Tiles)
	{
		// Nullptr possibility...
		if (!Tile) continue;

		uint8 TileType = uint8(Tile->GetTileType());

		// Out of bounds check...
		// if (TileType < 1 || TileType > 6 || Tile->IsTileHiddenInGame())
		//	continue;
			
		{
			--TileType;
			FProceduralMeshSection TileSection = FProceduralMeshSection(); // = MeshSections[TileType];
			TileSection.ElementID = MeshSections[TileType].ElementID;

			// Vertices, Faces, UVs, Normals generator
			int Triangle_Num = 0;
			for (int i = 0; i < 6; ++i)
			{
				bool bDrawFace = true;

				// Draws the current side's face
				if (bDrawFace)
				{
					FQuat Quatarian = (-Tile->GetTileLocation().GetSafeNormal()).ToOrientationQuat();
					
					Triangle_Num = UGALibrary::CreateFaceFromTransform(TileSection, MeshSectionData, FTransform(Quatarian, Tile->GetTileLocation(), Scale3D), i, Triangle_Num, VoxelSize);
				}
			}

			// TileSection.VertexColors.Init(Tile->GetTileColor().ToFColor(true), TileSection.Vertices.Num());
			TileSection.NumTriangles += Triangle_Num;
			TileSection.bEnableCollision = false;

			// Tile->SetTileMeshSection(TileSection);
			MeshSections[TileType].Append(TileSection); // TileIndex renamed to TileType

			UE_LOG(LogTemp, Warning, TEXT("--[%d] Section has [%d] vertices..."), TileType, MeshSections[TileType].Vertices.Num());
		}
	}

	/*for (FProceduralMeshSection& Section : MeshSections)
	{
		for (Index = 0; Index < Section.Vertices.Num(); ++Index)
		{
			Section.Normals[Index] = Section.Vertices[Index].GetSafeNormal();
		}
	}*/

	// URealtimeMeshSimple* RTMesh = NewObject<URealtimeMeshSimple>();

	{ // Clear Section
		const FRealtimeMeshSectionGroupKey SectionGroupKey = FRealtimeMeshSectionGroupKey::Create(0, FName("HexGrid"));
		FRealtimeMeshSimpleCompletionCallback CompletionCallback;
		RTMesh->RemoveSectionGroup(SectionGroupKey, CompletionCallback);
	}
	// Mesh->ClearSection(0, 0);
	// Setup the two material slots
	if (!Materials.IsEmpty()) 
	{
		for (int i = 0; i < Materials.Num(); ++i)
		{
			FName MaterialName = "Material_" + i;
			RTMesh->SetupMaterialSlot(i, MaterialName, Materials[i]);
			Mesh->SetMaterial(i, Materials[i]);
		}
		// Mesh->SetMaterial(i, Materials[i]);
		// RTMesh->SetupMaterialSlot(0, FName(), Materials[0]);
	}

	// For each MeshSection, Create a new mesh section
	for (int i = 0; i < MeshSections.Num(); ++i)
	{
		if (MeshSections[i].Vertices.Num() > 0)
		{
			{ // Create Section
				FRealtimeMeshStreamSet StreamSet;
				
				// FRealtimeMeshStream PositionStream  = FRealtimeMeshStream::Create<FVector>(FRealtimeMeshStreams::Position);
				// FRealtimeMeshStream TangentsStream  = FRealtimeMeshStream::Create<FRealtimeMeshTangentsNormalPrecision>(FRealtimeMeshStreams::Tangents);
				// FRealtimeMeshStream TexCoordsStream = FRealtimeMeshStream::Create<FVector2D>(FRealtimeMeshStreams::TexCoords);
				// FRealtimeMeshStream ColorsStream    = FRealtimeMeshStream::Create<FLinearColor>(FRealtimeMeshStreams::Color);
				// FRealtimeMeshStream TrianglesStream = FRealtimeMeshStream::Create<int32>(FRealtimeMeshStreams::Triangles);

				TRealtimeMeshBuilderLocal<uint16, FPackedNormal, FVector2DHalf, 1> Builder(StreamSet);

				// We can decide what vertex elements are enabled.
				Builder.EnableTangents();
				Builder.EnableTexCoords();
				Builder.EnableColors();
				Builder.EnablePolyGroups();

				Builder.ReserveNumVertices(MeshSections[i].Vertices.Num());
				for (int32 j = 0; j < MeshSections[i].Vertices.Num(); j++) 
				{
					// We can add a vertex, and optionally set things like the tangents, color, texcoords.
					// We can then get the new index from it to use later.
					auto V = Builder.AddVertex(FVector3f(MeshSections[i].Vertices[j]));
					V.SetNormalAndTangent(FVector3f(MeshSections[i].Normals[j]), FVector3f(MeshSections[i].Tangents[j]));
					V.SetColor(MeshSections[i].VertexColors[j]);
					V.SetTexCoord(FVector2f(MeshSections[i].UVs[j]));
				}

				for (int32 j = 0; j < MeshSections[i].Triangles.Num(); j+=3)
				{
					// Now we can add a triangle giving it the indices of the vertices for the 3 corners, as well as optionally supplying the polygroup
					Builder.AddTriangle(MeshSections[i].Triangles[j], MeshSections[i].Triangles[j+1], MeshSections[i].Triangles[j+2], i);
				}

				// Now create the group key. This is a unique identifier for the section group
				// A section group contains one or more sections that all share the underlying buffers
				// these sections can overlap the used vertex/index ranges depending on use case.
				const FRealtimeMeshSectionGroupKey SectionGroupKey = FRealtimeMeshSectionGroupKey::Create(0, "HexGrid_" + i);

				// Now create the section key, this is a unique identifier for a section within a group
				// The section contains the configuration for the section, like the material slot,
				// and the draw type, as well as the range of the index/vertex buffers to use to render.
				// Here we're using the version to create the key based on the PolyGroup index
				const FRealtimeMeshSectionKey PolyGroup0SectionKey = FRealtimeMeshSectionKey::CreateForPolyGroup(SectionGroupKey, i);

				// This will create a new section group named "TestBox" at LOD 0, with the created stream data above. This will create the sections associated with the polygroup
				RTMesh->CreateSectionGroup(SectionGroupKey, StreamSet, FRealtimeMeshSectionGroupConfig(ERealtimeMeshSectionDrawType::Static));

				// Update the configuration of both the polygroup sections.
				RTMesh->UpdateSectionConfig(PolyGroup0SectionKey, FRealtimeMeshSectionConfig(i));

				// RTMesh->CreateSection(0, 0, 0, VertexBuffer.GetVertices(), VertexBuffer.GetTriangles(), VertexBuffer.GetNormals(), VertexBuffer.GetUVs(), VertexBuffer.GetColors(), VertexBuffer.GetTangents());
			}

			// Mesh->CreateSectionFromComponents(0, i, i, MeshSections[i].Vertices, MeshSections[i].Triangles, MeshSections[i].Normals, MeshSections[i].UVs, MeshSections[i].VertexColors, MeshSections[i].Tangents, ERuntimeMeshUpdateFrequency::Infrequent, false);
			
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("--Section has 0 vertices..."));
		}
	}
}

void AHexGrid::UpdateMeshSection(int32 MeshSectionIndex)
{
	if (Tiles.IsEmpty())
		UE_LOG(LogTemp, Warning, TEXT("No Tiles"));

	TArray<FProceduralMeshSection> MeshSections;
	MeshSections.SetNum(6);

	for (UHexTile* Tile : Tiles)
	{
		// Nullptr, wrong section, or hidden
		if (!Tile || 
			Tile->GetTileType() - 1 != MeshSectionIndex || 
			Tile->IsTileHiddenInGame()) continue;

		FProceduralMeshSection& TileSection = Tile->GetTileMeshSection();

		MeshSections[MeshSectionIndex].Append(TileSection);
	}

	// URealtimeMeshSimple* RTMesh = NewObject<URealtimeMeshSimple>();

	{ // Clear Section

	}
	// Mesh->ClearSection(0, MeshSectionIndex);
	
	{// Create Section
		// RTMesh->CreateSection(0, 0, 0, VertexBuffer.GetVertices(), VertexBuffer.GetTriangles(), VertexBuffer.GetNormals(), VertexBuffer.GetUVs(), VertexBuffer.GetColors(), VertexBuffer.GetTangents());
	}
	// Mesh->CreateSectionFromComponents(0, MeshSectionIndex, MeshSectionIndex, MeshSections[MeshSectionIndex].Vertices, MeshSections[MeshSectionIndex].Triangles, MeshSections[MeshSectionIndex].Normals, MeshSections[MeshSectionIndex].UVs, MeshSections[MeshSectionIndex].VertexColors, MeshSections[MeshSectionIndex].Tangents); 
	
	if (Materials.IsValidIndex(MeshSectionIndex)) Mesh->SetMaterial(MeshSectionIndex, Materials[MeshSectionIndex]);
}

void AHexGrid::UpdateMesh()
{
	if (Tiles.IsEmpty())
		UE_LOG(LogTemp, Warning, TEXT("No Tiles"));

	// URealtimeMeshSimple* RTMesh = NewObject<URealtimeMeshSimple>();
	TArray<FProceduralMeshSection> MeshSections;
	MeshSections.SetNum(6);

	for (UHexTile* Tile : Tiles)
	{
		// Nullptr, wrong section, or hidden
		if (!Tile || 
			Tile->GetTileType() <= 0 ||
			Tile->IsTileHiddenInGame()) continue;

		FProceduralMeshSection& TileSection = Tile->GetTileMeshSection();

		MeshSections[Tile->GetTileType() - 1].Append(TileSection);
	}

	// For each MeshSection, Create a new mesh section
	for (int i = 0; i < MeshSections.Num(); ++i)
	{
		if (MeshSections[i].Vertices.Num() > 0)
		{
			// Mesh->UpdateSectionFromComponents(0, i, MeshSections[i].Vertices, MeshSections[i].Triangles, MeshSections[i].Normals, MeshSections[i].UVs, MeshSections[i].VertexColors, MeshSections[i].Tangents);
		}
	}
}

UHexTile* AHexGrid::AddTile_Implementation(const FVector& TileLocation, int32 CurrentTileNum)
{
	FRotator Quatarian;
	if (TileLocation.GetSafeNormal() == NormalVector)
	{
		Quatarian = NormalVector.Rotation() + FRotator(90, 0, 0);
	}
	else
	{
		Quatarian = UKismetMathLibrary::FindLookAtRotation(TileLocation.GetSafeNormal(), TileLocation) + FRotator(90, 0, 0);
	}

	if (MeshCollisionInstances)
		MeshCollisionInstances->AddInstance(FTransform(Quatarian, (TileLocation * VoxelSize) * 1.01, Scale3D * 0.1));

	UHexTile* NewTile = NewObject<UHexTile>();
	if (NewTile)
	{
		NewTile->SetTileType(Types[CurrentTileNum]);
		NewTile->SetTileLocation(TileLocation);
		NewTile->SetTileIndex(CurrentTileNum + (ChunkIndex.X * ChunkLineElementsP2));
		NewTile->SetTileHiddenInGame(false);
		NewTile->SetTileColor(FLinearColor::White);
	}
	return NewTile;
}

TArray<uint8> AHexGrid::CalcTypes_Implementation() const
{
	uint8 x, y, z;
	float LocX, LocY, LocZ;
	TArray<uint8> Value;
	z = 0;
	Value.Reserve(ChunkLineElementsP3);
	for (z = 0; z < ChunkLineElementsExt; ++z)
	{
		LocZ = (z + (ChunkIndex.Z * ChunkLineElements));
		for (y = 0; y < ChunkLineElementsExt; ++y)
		{
			LocY = (y + (ChunkIndex.Y * ChunkLineElements)); // Chunks are offset by ChunkLineElements. ChunkLineElementsExt contains neighbor chunks.
			for (x = 0; x < ChunkLineElementsExt; ++x)
			{
				LocX = (x + (ChunkIndex.X * ChunkLineElements));
				// float Noise = 0;
				float Noise = ((((USimplexNoiseBPLibrary::SimplexNoise3D(LocX, LocY, LocZ, 0.5f) // -1 -> 1
					+ 1.f)	// 0 - 2
					* 0.5f)	// 0 - 1
					* 6.f)	// 0 - 6
					+ 1.f);	// 1 - 7 */

				Value.Add(Noise > 6 ? 6 : (int32)Noise);
			}
		}
	} // */

	return Value;
}

TArray<int32> AHexGrid::CalcHeight_Implementation() const
{
	int32 x, y, z;
	float LocX, LocY;
	TArray<int32> Value;
	Value.Reserve(ChunkTotalElements);
	
	for (z = 0; z < ChunkZElements; ++z)
	for (y = 0; y < ChunkLineElementsExt; ++y)
	for (x = 0; x < ChunkLineElementsExt; ++x)
	{
		LocX = (x + (ChunkIndex.X * ChunkLineElements * 0.5));
		LocY = (y + (ChunkIndex.Y * ChunkLineElements));

		float Noise1 = ((USimplexNoiseBPLibrary::SimplexNoise2D(LocX * 0.25, LocY * 0.25) + 1) * 0.5);
		float Noise2 = ((USimplexNoiseBPLibrary::SimplexNoise2D(LocX * 0.025, LocY * 0.025) + 1) * 0.5) * 6;

		Value.Add((int32)(Noise1 + Noise2));
	}

	/*const auto Half = ChunkLineElements * 0.5;
	const auto HalfExt = ChunkLineElementsExt * 0.5;
	
	for (int q = -HalfExt; q < HalfExt; ++q)
	{
		int r1 = FMath::Max(-HalfExt, -q - HalfExt);
		int r2 = FMath::Min(HalfExt, -q + HalfExt);
		for (int r = r1; r <= r2; ++r)
		{
			float Noise1 = ((USimplexNoiseBPLibrary::SimplexNoise2D(q * 0.25, r * 0.25) + 1) * 0.5);
			float Noise2 = ((USimplexNoiseBPLibrary::SimplexNoise2D(q * 0.025, r * 0.025) + 1) * 0.5) * 6;

			Value.Add((uint8)(Noise1 + Noise2));
		}
	}*/

	return Value;
}

FTransform AHexGrid::GetTileTransform(int32 TileIndex) const
{
	FTransform InstanceTransform = FTransform();

	MeshCollisionInstances->GetInstanceTransform(TileIndex, InstanceTransform, false);
	return InstanceTransform;
}

void AHexGrid::SetTileVertexColor(const FLinearColor& Color, int32 TileIndex)
{
	// Guard against array out-of-bounds access error
	if (!Tiles.IsValidIndex(TileIndex) || Tiles[TileIndex]->GetTileType() == 0) return;

	int32 MeshSectionIndex = Tiles[TileIndex]->GetTileType() - 1;

	// Set Tile color
	Tiles[TileIndex]->SetTileColor(Color);

	// Update Mesh Section with new Vertex Colors
	// ConstructMesh();
}

void AHexGrid::SetTileHiddenInGame(bool NewHidden, int32 TileIndex)
{
	// Guard against array out-of-bounds access error
	if (!Tiles.IsValidIndex(TileIndex)) return;

	// Set Tile visible
	Tiles[TileIndex]->SetTileHiddenInGame(NewHidden);

	// ConstructMesh();
}

