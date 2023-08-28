// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/HexGrid/HexGrid.h"
#include "HexTile.h"
#include "Kismet/KismetMathLibrary.h"
#include "SimplexNoiseBPLibrary.h"
#include "GALibrary.h"
#include "GameplayTileData.h"

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

	MeshCollisionInstances = 
		CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Collision Meshes"));
	MeshCollisionInstances->SetupAttachment(RootComponent);
}

#if WITH_EDITOR
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

	Initialize(RandomSeed, VoxelSize, ChunkLineElements, ChunkIndex);

	Super::PostEditChangeProperty(e);
}
#endif

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

	if (!Mesh)
	{
		FString String = "NewVoxel_" + FString::FromInt(ChunkIndex.X) + "_" + FString::FromInt(ChunkIndex.Y);
		FName Name = FName(*String);
		Mesh = NewObject<URuntimeMeshComponentStatic>(this, Name);
		Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
		Mesh->RegisterComponent();
		// PMesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
	}

	// GenerateChunk();
	GenerateHexGrid();
	// GenerateHexSphere();
	ConstructMesh();
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

void AHexGrid::GenerateHexGrid()
{
	// We need three vectors that are perpendicular to the normal Vector.

	BeforeChunkGenerated();

	int32 x, y, z, i, Total;
	const FVector Position = NormalVector;
	FVector axisA;
	FVector axisB;
	FVector axisC;

	// Built-In utility function for returning a perpendicular plane to the given Vector
	NormalVector.FindBestAxisVectors(axisA, axisB);

	// axisA and axisB are 90 degrees apart when each vector should be 30 degrees apart.
	axisA = axisA.RotateAngleAxis(30.0f, NormalVector);
	axisC = axisB.RotateAngleAxis(60.0f, NormalVector);

	axisA.Normalize();
	axisB.Normalize();
	axisC.Normalize();

	i = 0;
	Total = ChunkLineElements - 1 > 0 ? ChunkLineElements - 1 : 1; // Don't Divide by Zero
	int32 Half = ChunkLineElements * 0.5;
	z = 0;
	for (z = -Half; z <= Half; ++z)
	{
		for (y = -Half; y <= Half; ++y)
		{
			for (x = -Half; x <= Half; ++x)
			{
				// z = (-x - y);
				// if (x + y + z != 0)
				//	continue;

				FVector Percent = FVector(x, y, z) / Total;
				double LocX = ((Percent.X - 0.5f) * 2);
				double LocY = ((Percent.Y - 0.5f) * 2);
				double LocZ = ((Percent.Z - 0.5f) * 2);

				FVector pointOnUnitCube = (Position * 2.1 +
					((x * axisA) +
					(y * axisB) +
					(z * axisC)) * 0.2);

				FVector Pos = pointOnUnitCube;

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
	}

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
			*/
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

void AHexGrid::ConstructMesh()
{
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
		if (TileType < 1 || TileType > 6 || Tile->IsTileHiddenInGame())
			continue;
			
		{
			--TileType;
			FProceduralMeshSection TileSection = FProceduralMeshSection(); // = MeshSections[TileType];
			TileSection.ElementID = MeshSections[TileType].ElementID;

			// Vertices, Faces, UVs, Normals generator
			int Triangle_Num = 0;
			for (int i = 0; i < 6; ++i)
			{
				bool Flag = false;

				if (i > 1)
				{
					Flag = true;
					/*for (int d = 0; d < 6; ++d)
					{
						int y = Tile->GetTileLocation().Y;
						const int parity = (y & 1);
						const int NewIndex = Index + HexMask[parity][d].X +
							(HexMask[parity][d].Y * ChunkLineElementsExt);

						if (NewIndex >= 0 && NewIndex < Types.Num())
						{
							if (Types[NewIndex] == 0)
							{
								Flag = true;
								break;
							}
						}
					}*/
				}
				else if (i == 1) Flag = true;
				/*{
					const int UpIndex = Index + (CubeVoxelMask[i].Z * ChunkLineElementsP2Ext);
					if (UpIndex >= 0 && UpIndex < Types.Num())
					{
						if (Types[UpIndex] == 0) true;
					}
				}*/

				// Draw face
				if (Flag)
				{
					FQuat Quatarian = (FVector::ZeroVector - Tile->GetTileLocation()).ToOrientationQuat();
					Triangle_Num = UGALibrary::CreateFaceFromTransform(TileSection, MeshSectionData, FTransform(Quatarian, Tile->GetTileLocation(), Scale3D), i, Triangle_Num, VoxelSize);
				}
			}

			// TileSection.VertexColors.Init(Tile->GetTileColor().ToFColor(true), TileSection.Vertices.Num());
			TileSection.NumTriangles += Triangle_Num;
			TileSection.bEnableCollision = false;

			// Tile->SetTileMeshSection(TileSection);
			MeshSections[TileType].Append(TileSection); // TileIndex renamed to TileType
		}
	}

	/*for (FProceduralMeshSection& Section : MeshSections)
	{
		for (Index = 0; Index < Section.Vertices.Num(); ++Index)
		{
			Section.Normals[Index] = Section.Vertices[Index].GetSafeNormal();
		}
	}*/

	Mesh->ClearSection(0, 0);

	// For each MeshSection, Create a new mesh section
	for (int i = 0; i < MeshSections.Num(); ++i)
	{
		if (MeshSections[i].Vertices.Num() > 0)
		{
			Mesh->CreateSectionFromComponents(0, i, i, MeshSections[i].Vertices, MeshSections[i].Triangles, MeshSections[i].Normals, MeshSections[i].UVs, MeshSections[i].VertexColors, MeshSections[i].Tangents, ERuntimeMeshUpdateFrequency::Infrequent, false);
			if (Materials.IsValidIndex(i)) Mesh->SetMaterial(i, Materials[i]);
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

	Mesh->ClearSection(0, MeshSectionIndex);
	Mesh->CreateSectionFromComponents(0, MeshSectionIndex, MeshSectionIndex, MeshSections[MeshSectionIndex].Vertices, MeshSections[MeshSectionIndex].Triangles, MeshSections[MeshSectionIndex].Normals, MeshSections[MeshSectionIndex].UVs, MeshSections[MeshSectionIndex].VertexColors, MeshSections[MeshSectionIndex].Tangents); 
	if (Materials.IsValidIndex(MeshSectionIndex)) Mesh->SetMaterial(MeshSectionIndex, Materials[MeshSectionIndex]);
}

void AHexGrid::UpdateMesh()
{
	if (Tiles.IsEmpty())
		UE_LOG(LogTemp, Warning, TEXT("No Tiles"));

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
			Mesh->UpdateSectionFromComponents(0, i, MeshSections[i].Vertices, MeshSections[i].Triangles, MeshSections[i].Normals, MeshSections[i].UVs, MeshSections[i].VertexColors, MeshSections[i].Tangents);
		}
	}
}

UHexTile* AHexGrid::AddTile_Implementation(const FVector& TileLocation, int32 CurrentTileNum)
{
	FRotator Direction = (FVector::ZeroVector - TileLocation).ToOrientationQuat().Rotator();
	FRotator Quatarian = FRotator(-90, 0, 0) + Direction;
	// FVector Loc = Quatarian.RotateVector(UKismetMathLibrary::InverseTransformLocation(FTransform(FRotator(90, 0, 0), FVector::ZeroVector), TileLocation * VoxelSize));
	if (MeshCollisionInstances)
		MeshCollisionInstances->AddInstance(FTransform(Quatarian,(TileLocation * VoxelSize), Scale3D * FVector(0.3, 0.3, 0.5)));

	UHexTile* NewTile = NewObject<UHexTile>();
	if (NewTile)
	{
		NewTile->SetTileType(Types[CurrentTileNum]);
		NewTile->SetTileLocation(TileLocation);
		NewTile->SetTileIndex(CurrentTileNum + (ChunkIndex.X * ChunkLineElementsP3));
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
	Value.Reserve(ChunkTotalElements);
	for (z = 0; z < ChunkLineElementsExt; ++z)
	{
		LocZ = (z + (ChunkIndex.Z * ChunkLineElementsP2));
		for (y = 0; y < ChunkLineElementsExt; ++y)
		{
			LocY = (y + (ChunkIndex.Y * ChunkLineElements)); // Chunks are offset by ChunkLineElements. ChunkLineElementsExt contains neighbor chunks.
			for (x = 0; x < ChunkLineElementsExt; ++x)
			{
				LocX = (x + (ChunkIndex.X * ChunkLineElements));
				float Noise = ((((USimplexNoiseBPLibrary::SimplexNoise3D(LocX, LocY, LocZ) // -1 - 1
					+ 1)	// 0 - 2
					* 0.5)	// 0 - 1
					* 6)	// 0 - 6
					+ 1);	// 1 - 7

				Value.Add((uint8)FMath::Clamp(Noise, 1, 7));
			}
		}
	}

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

