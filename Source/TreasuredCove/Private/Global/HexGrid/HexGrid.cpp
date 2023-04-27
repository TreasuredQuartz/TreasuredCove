// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/HexGrid/HexGrid.h"
#include "HexTile.h"
#include "SimplexNoiseBPLibrary.h"
#include "GALibrary.h"
#include "GameplayTileData.h"

// Sets default values
AHexGrid::AHexGrid()
{
	VoxelSize = 200;
	ChunkLineElements = 32;
	SurfaceHeight = 30;

	VerticalOffset = (VoxelSize * .75);
	HorizontalOffset = (DOUBLE_UE_SQRT_3 * (double)VoxelSize);
	OddHorizontalOffset = ((DOUBLE_UE_SQRT_3 * (double)VoxelSize) * .5);
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

	if (!Mesh)
	{
		FString String = "NewVoxel_" + FString::FromInt(ChunkIndex.X) + "_" + FString::FromInt(ChunkIndex.Y);
		FName Name = FName(*String);
		Mesh = NewObject<URuntimeMeshComponentStatic>(this, Name);
		Mesh->RegisterComponent();
		SetRootComponent(Mesh);
		// PMesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
	}

	GenerateChunk();
	UpdateMesh();
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
		Tiles.SetNumZeroed(ChunkTotalElements);
		
		// bShouldInitializeTiles = false;
	}
}

void AHexGrid::BeforeChunkGenerated()
{
	InitializeNoise();
	InitializeTiles();
}

void AHexGrid::GenerateHexGrid()
{
	BeforeChunkGenerated();
	const auto Half = ChunkLineElements * 0.5;
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
	if (CurrentLocation.Z > (SurfaceHeight + Height[i])) Types[i] = 0;

	// Offset coordinates
	const bool oddRow = y & 1;
	const float yPos = (Location.Y + y) * VerticalOffset;
	const float xPos = oddRow ? ((Location.X + x) * HorizontalOffset) + OddHorizontalOffset : (Location.X + x) * HorizontalOffset;

	const FVector Pos = FVector(xPos, yPos, z);

	Tiles[i] = AddTile(Pos, Types[i]);
	// AddTileDelegate.Broadcast(Pos, Types[i]);
	// GenerateSurface();
}

void AHexGrid::AfterChunkGenerated()
{

}

const FVector CubeVoxelMask[] = { 
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

void AHexGrid::UpdateMesh()
{
	TArray<FProceduralMeshSection> MeshSections;
	MeshSections.SetNum(6);
	int32 Index = -1;
	for (const UHexTile* Tile : Tiles)
	{
		++Index;
		// Nullptr possibility...
		if (!Tile) continue;

		// Out of bounds check...
		if (Index < 0 || Index >= Types.Num()) continue;
		uint8 TileIndex = Types[Index];
			
		if (TileIndex > 0)
		{
			--TileIndex;

			// Vertices, Faces, UVs, Normals generator
			int Triangle_Num = 0;
			for (int i = 0; i < 6; ++i)
			{
				bool Flag = false;

				if (i > 1)
				{
					for (int d = 0; d < 6; ++d)
					{
						int y = Tile->GetTileLocation().Y;
						const bool parity = (y & 1);
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
					}
				}
				else
				{
					const int UpIndex = Index + (CubeVoxelMask[i].Z * ChunkLineElementsP2Ext);
					if (UpIndex >= 0 && UpIndex < Types.Num())
					{
						if (Types[UpIndex] == 0) Flag = true;
					}
				}
				// Draw face
				if (Flag) Triangle_Num = UGALibrary::CreateFace(MeshSections[TileIndex], HexTileData, Tile->GetTileLocation(), i, Triangle_Num, VoxelSize, 1);
			}
			MeshSections[TileIndex].ElementID += Triangle_Num;
			MeshSections[TileIndex].bEnableCollision = false;
			if (Materials.IsValidIndex(TileIndex)) MeshSections[TileIndex].Material = Materials[TileIndex];
		}
	}

	Mesh->ClearSection(0, 0);

	// For each MeshSection, Create a new mesh section
	for (int i = 0; i < MeshSections.Num(); i++)
	{
		if (MeshSections[i].Vertices.Num() > 0)
		{
			Mesh->CreateSectionFromComponents(0, i, i, MeshSections[i].Vertices, MeshSections[i].Triangles, MeshSections[i].Normals, MeshSections[i].UVs, MeshSections[i].VertexColors, MeshSections[i].Tangents);
			if (Materials.IsValidIndex(i)) Mesh->SetMaterial(i, MeshSections[i].Material);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("--Section has 0 vertices..."));
		}
	}
}

UHexTile* AHexGrid::AddTile_Implementation(const FVector& TileLocation, uint8 TileType)
{
	UHexTile* NewTile = NewObject<UHexTile>();
	if (NewTile)
	{
		NewTile->SetTileType(TileType);
		NewTile->SetTileLocation(TileLocation);
	}
	return NewTile;
}

TArray<uint8> AHexGrid::CalcTypes_Implementation() const
{
	uint8 x, y, z;
	float LocX, LocY;
	TArray<uint8> Value;

	Value.Reserve(ChunkTotalElements);
	for (z = 0; z < ChunkZElements; ++z)
	for (y = 0; y < ChunkLineElementsExt; ++y)
	for (x = 0; x < ChunkLineElementsExt; ++x)
	{
		LocX = (x + (ChunkIndex.X * ChunkLineElements * 0.5));
		LocY = (y + (ChunkIndex.Y * ChunkLineElements));
		float Noise = ((((USimplexNoiseBPLibrary::SimplexNoise2D(LocX, LocY) + 1) * 0.5) // 0 - 1 Range
			* 6) // 0 - 6
			+ 1); // 1 - 7

		Value.Add((uint8)Noise);
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

