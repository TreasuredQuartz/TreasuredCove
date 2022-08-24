// Fill out your copyright notice in the Description page of Project Settings.


#include "MCWorld_Voxel.h"
#include "MCWorld_VoxelItem.h"
#include "MCWorld_Instance.h"
#include "WaterBodyProceduralActor.h"
#include "GameplayTileData.h"
#include "SimplexNoiseBPLibrary.h"
#include "MCWorld_BuildingHelperMacros.h"

#include "Engine/AssetManager.h"
#include "Engine/Engine.h"
#include "Engine/Selection.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GALibrary.h"

#pragma region Constants
const FVector VoxelMask[] = { FVector(0.00000, 0.00000, 1.00000), FVector(0.00000, 0.00000, -1.00000), FVector(0.00000, 1.00000, 0.00000), FVector(0.00000, -1.00000, 0.00000), FVector(1.00000, 0.00000, 0.00000), FVector(-1.00000, 0.00000, 0.00000) };

CONSTEXPR int GRASSTALL{ -2 };
CONSTEXPR int GRASSSHORT{ -1 };
CONSTEXPR int AIR{ 0 };
CONSTEXPR int WATER{ 38 };
CONSTEXPR int BEDROCK{ 39 };

CONSTEXPR int GRASS{ 1 };
CONSTEXPR int DIRT{ 2 };

CONSTEXPR int STONE{ 3 };
CONSTEXPR int STONESLAB{ 4 };
CONSTEXPR int STONESTAIRS{ 5 };

CONSTEXPR int STONEBRICK{ 6 };
CONSTEXPR int STONEBRICKSLAB{ 7 };
CONSTEXPR int STONEBRICKSTAIRS{ 8 };

CONSTEXPR int COBBLE{ 9 };
CONSTEXPR int COBBLESLAB{ 10 };
CONSTEXPR int COBBLESTAIRS{ 11 };

CONSTEXPR int COALORE{ 12 };
CONSTEXPR int IRONORE{ 13 };
CONSTEXPR int GOLDORE{ 14 };
CONSTEXPR int REDSTONEORE{ 15 };
CONSTEXPR int LAPISLAZULIORE{ 16 };
CONSTEXPR int DIAMONDORE{ 17 };
CONSTEXPR int EMERALDORE{ 18 };

CONSTEXPR int SAND{ 19 };
CONSTEXPR int SANDSTONE{ 20 };
CONSTEXPR int SANDSTONESLAB{ 21 };
CONSTEXPR int SANDSTONESTAIRS{ 22 };

CONSTEXPR int OAKLEAVES{ 23 };
CONSTEXPR int OAKLOG{ 24 };
CONSTEXPR int OAKPLANKS{ 25 };
CONSTEXPR int OAKSTAIRS{ 26 };
CONSTEXPR int OAKDOOR{ 27 };
CONSTEXPR int OAKFENCE{ 28 };
// CONSTEXPR int OAKPRESSUREPLATE{ 29 };

CONSTEXPR int BIRCHLEAVES{ 29 };
CONSTEXPR int BIRCHLOG{ 30 };
CONSTEXPR int BIRCHPLANKS{ 31 };

CONSTEXPR int ELMLEAVES{ 32 };
CONSTEXPR int ELMLOG{ 33 };
CONSTEXPR int ELMPLANKS{ 34 };

CONSTEXPR int CHEST{ 35 };
CONSTEXPR int CRAFTINGTABLE{ 36 };
CONSTEXPR int FURNACE{ 37 };
#pragma endregion

// Sets default values
AMCWorld_Voxel::AMCWorld_Voxel()
{
	VoxelSize = 200;
	ChunkLineElements = 32;
	XMult = 0.5f;
	YMult = 0.5f;
	ZMult = 1.f;
	Weight = 1.f;
	Freq = 1.f;
	SurfaceHeight = 30;
	// BlockData.SetNum(BEDROCK);
	SelectionMesh =
		CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Selected Location"));
	InitBuildings();

#if WITH_EDITOR
	USelection::SelectObjectEvent.AddUObject(this, &AMCWorld_Voxel::OnObjectSelected);
#endif
}

#if WITH_EDITOR
// Called in editor and before begin play
void AMCWorld_Voxel::PostInitProperties()
{
	Super::PostInitProperties();
}

void AMCWorld_Voxel::OnConstruction(const FTransform& Transform)
{
	OnConstruction_DoOnce();

	if (GetWorld()) RootComponent->SetWorldTransform(Transform);

	Super::OnConstruction(Transform);
}

void AMCWorld_Voxel::OnConstruction_DoOnce()
{
	if (!bConstructionDoOnce)
	{
		bConstructionDoOnce = true;
		ChunkFields = TArray<int32>();
		Initialize(RandomSeed, VoxelSize, ChunkLineElements, ChunkIndex);
	}
}

void AMCWorld_Voxel::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	if (e.Property == NULL) return;

	FName MemberPropertyName = e.MemberProperty->GetFName();
	FName PropertyName = e.Property->GetFName();

	UE_LOG(LogTemp, Warning, TEXT("MC_Voxel: %s"), *PropertyName.ToString());

	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Orange, PropertyName.ToString());
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Purple, MemberPropertyName.ToString());

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AMCWorld_Voxel, ChunkLineElements) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(AMCWorld_Voxel, ChunkZElements) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(AMCWorld_Voxel, VoxelSize) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(AMCWorld_Voxel, RandomSeed) ||
		MemberPropertyName == GET_MEMBER_NAME_CHECKED(AMCWorld_Voxel, ChunkIndex))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Yellow, "Reinitializing!");
		ChunkFields = TArray<int32>();
		Initialize(RandomSeed, VoxelSize, ChunkLineElements, ChunkIndex);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(AMCWorld_Voxel, XMult) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(AMCWorld_Voxel, YMult) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(AMCWorld_Voxel, ZMult) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(AMCWorld_Voxel, Weight) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(AMCWorld_Voxel, Freq))
	{
		ChunkFields = TArray<int32>();
		Initialize(RandomSeed, VoxelSize, ChunkLineElements, ChunkIndex);
	}

	Super::PostEditChangeProperty(e);
}

void AMCWorld_Voxel::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e)
{
	Super::PostEditChangeChainProperty(e);
}

void AMCWorld_Voxel::OnObjectSelected(UObject* Object)
{
	if (Object == this)
	{
		bSelectedInEditor = true;
		OnSelected_Editor();
	}
	else if (bSelectedInEditor && !IsSelected())
	{
		bSelectedInEditor = false;
		OnUnSelected_Editor();
	}
}

void AMCWorld_Voxel::OnSelected_Editor()
{
	if (!GetWorld() || !GEditor) return;

	if (auto Client = (FEditorViewportClient*)(GEditor->GetActiveViewport()->GetClient()))
	{
		const FVector Start = FVector(Client->GetCursorWorldLocationFromMousePos().GetCursorPos());
		const FVector Direction = Client->GetCursorWorldLocationFromMousePos().GetDirection();
		const FVector End = Start + (Direction * 10000);
		FHitResult Hit;

		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility))
		{
			if (Hit.Component == PMesh)
			{
				SelectedLocation = Hit.Location / VoxelSize;
				SelectionMesh->SetWorldLocation(SelectedLocation);
			}
		}
	}
}

void AMCWorld_Voxel::OnUnSelected_Editor()
{
	SelectedLocation = FVector::ZeroVector;
}

void AMCWorld_Voxel::SetSelectedVoxel(int32 Value)
{
	if (bSelectedInEditor)
	{
		SetVoxel(SelectedLocation, Value);
	}
}
#endif

#pragma region Initialization
void AMCWorld_Voxel::Initialize(int32 inRandomSeed, int32 inVoxelSize, int32 inChunkLineElements, FIntVector inChunkIndex)
{
	if (!this) return;

	AGameplayVoxel::Initialize(inRandomSeed, inVoxelSize, inChunkLineElements, inChunkIndex);

	if (!PMesh)
	{
		FString String = "Voxel_" + FString::FromInt(ChunkIndex.X) + "_" + FString::FromInt(ChunkIndex.Y);
		FName Name = FName(*String);
		PMesh = NewObject<UProceduralMeshComponent>(this, Name);
		PMesh->RegisterComponent();
		SetRootComponent(PMesh);
		// PMesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
	}

	/*if (UAssetManager* AssetManager = UAssetManager::GetIfValid())
	{
		AssetManager->LoadPrimaryAssets(FPrimaryAssetId);
	}*/

	GenerateChunk();
	UpdateChunkFromData();
	UpdateMesh();
	UpdateExtras();
}

void AMCWorld_Voxel::InitBuildings()
{
	InitSmallHouse();
	InitLargeHouse();
	InitBlacksmith();
}

void AMCWorld_Voxel::InitSmallHouse()
{
	TMap<FIntVector, int32> HouseSmall;
	SMALLHOUSE_FULL
	FMCWorld_Building LocHouse = FMCWorld_Building(HouseSmall);
	HouseStructures.Add(LocHouse);
}

void AMCWorld_Voxel::InitLargeHouse()
{
	TMap<FIntVector, int32> HouseLarge;
	// LARGEHOUSE_FULL;
	FMCWorld_Building LocHouse = FMCWorld_Building(HouseLarge);
	HouseStructures.Add(LocHouse);
}

void AMCWorld_Voxel::InitBlacksmith()
{
	TMap<FIntVector, int32> Blacksmith;
	BLACKSMITH_FULL
	FMCWorld_Building LocHouse = FMCWorld_Building(Blacksmith);
	HouseStructures.Add(LocHouse);
}

void AMCWorld_Voxel::InitTower()
{

}
#pragma endregion

#pragma region Generation
TMap<FIntVector, int32> GenerateCircle(int radius)
{
	TMap<FIntVector, int32> Value;
	if (radius == 0)
		return Value;

	for (int x = -radius; x <= radius; ++x)
		for (int y = -radius; y <= radius; ++y)
		{
			if (x + y <= radius)
			{

			}
		}

	return Value;
}

void AMCWorld_Voxel::GenerateSmallHouse(FIntVector TownCenter)
{
	for (int32 x = -3; x < 3; x++)
		for (int32 y = -3; y < 3; y++)
			for (int32 z = 0; z < 6; z++)
			{
				int Index = TownCenter.X + x + ((TownCenter.Y + y) * ChunkLineElementsExt) + ((TownCenter.Z + z) * ChunkLineElementsP2Ext);

				FIntVector LocIntVector = FIntVector(x, y, z);

				if (HouseStructures[0].Blocks.Contains(LocIntVector))
					ChunkFields[Index] = HouseStructures[0].Blocks[LocIntVector];
				else
					ChunkFields[Index] = AIR;
			}
}

void AMCWorld_Voxel::GenerateBlacksmith(FIntVector TownCenter)
{
	for (int32 x = -5; x < 5; x++)
	{
		for (int32 y = -3; y < 3; y++)
		{
			for (int32 z = 0; z < 7; z++)
			{
				int Index = TownCenter.X + x + ((TownCenter.Y + y) * ChunkLineElementsExt) + ((TownCenter.Z + z) * ChunkLineElementsP2);

				FIntVector LocIntVector = FIntVector(x, y, z);

				if (HouseStructures[2].Blocks.Contains(LocIntVector))
				{
					ChunkFields[Index] = HouseStructures[1].Blocks[LocIntVector];
				}
				else
				{
					ChunkFields[Index] = AIR;
				}

			}
		}
	}
}

void AMCWorld_Voxel::GenerateTemple(FIntVector TownCenter)
{
}

void AMCWorld_Voxel::GenerateTower(FIntVector TowerCenter)
{

}

void AMCWorld_Voxel::GenerateTowns()
{
	for (FIntVector TownCenter : TownCenters)
	{
		GenerateSmallHouse(TownCenter);
		// GenerateBlacksmith(TownCenter);
	}
}

void AMCWorld_Voxel::GenerateTrees()
{
	for (FIntVector TreeCenter : TreeCenters)
	{
		int32 B = TreeType[TreeCenter.X + (TreeCenter.Y * ChunkLineElementsExt) + (TreeCenter.Z * ChunkLineElementsP2Ext)];
		int32 Leaves = OAKLEAVES;
		int32 Trunk = OAKLOG;

		switch (B) {
		case 4:
			Leaves = OAKLEAVES;
			Trunk = OAKLOG;
			break;
		default:
			Leaves = OAKLEAVES;
			Trunk = OAKLOG;
			break;
		}

		int32 TreeHeight = (RandomStream.FRand() * 2) + 3;
		int32 RandomX = RandomStream.FRand() * 2;
		int32 RandomY = RandomStream.FRand() * 2;
		int32 RandomZ = RandomStream.FRand() * 2;

		// Tree Leaves
		for (int32 LeafX = -2; LeafX < 3; ++LeafX)
			for (int32 LeafY = -2; LeafY < 3; ++LeafY)
				for (int32 LeafZ = 0; LeafZ < 3; ++LeafZ)
				{
					if (InRange(LeafX + TreeCenter.X + 1, ChunkLineElements + 1) && InRange(LeafY + TreeCenter.Y + 1, ChunkLineElements + 1) && InRange(LeafZ + TreeCenter.Z + TreeHeight + 1, ChunkZElements))
					{
						float Radius = FVector(LeafX * RandomX, LeafY * RandomY, LeafZ * RandomZ).Size();
						if (Radius <= 2.8)
						{
							if (RandomStream.FRand() < 0.5 || Radius <= 0.5)
							{
								int32 cur = TreeCenter.X + LeafX + ((TreeCenter.Y + LeafY) * ChunkLineElementsExt) + ((TreeCenter.Z + LeafZ + TreeHeight) * ChunkLineElementsP2Ext);
								if (ChunkFields.IsValidIndex(cur))
									ChunkFields[cur] = ChunkFields[cur] == 0 ? Leaves : ChunkFields[cur]; // Max 9
							}
						}
					}
				}

		// Tree Trunk

		int32 Index = TreeCenter.X + (TreeCenter.Y * ChunkLineElementsExt);
		for (int h = 0; h < TreeHeight; ++h)
		{
			int32 i = Index + ((TreeCenter.Z + h) * ChunkLineElementsP2Ext);

			if (InRange(TreeCenter.Z + h + 1, ChunkZElements))
				ChunkFields[i] = Trunk; // + TreeType[TreeCenter.X + (TreeCenter.Y * ChunkLineElementsExt) + ((TreeCenter.Z) * ChunkLineElementsP2Ext)]; // Max 45

		}
	}
}

void AMCWorld_Voxel::GenerateOrePockets()
{
	int32 RandomX = RandomStream.RandRange(0, 1);
	int32 RandomY = RandomStream.RandRange(0, 1);
	int32 RandomZ = RandomStream.RandRange(0, 1);

	for (FIntVector IronOrePocketCenter : IronOrePocketCenters)
	{
		for (int x = -1; x < 1; ++x)
			for (int y = -1; y < 1; ++y)
				for (int z = -1; z < 1; ++z)
				{
					if (InRange(x + IronOrePocketCenter.X + 1, ChunkLineElements + 1) && InRange(y + IronOrePocketCenter.Y + 1, ChunkLineElements + 1) && InRange(z + IronOrePocketCenter.Z + 1, ChunkZElements))
					{
						float Radius = FVector(x * RandomX, y * RandomY, z * RandomZ).Size();
						if (Radius <= 2.8)
						{
							if (RandomStream.FRand() < 0.5 || Radius <= 1.4)
							{
								int32 Index = IronOrePocketCenter.X + x + ((IronOrePocketCenter.Y + y) * ChunkLineElementsExt) + ((IronOrePocketCenter.Z + z) * ChunkLineElementsP2);
								if (Index >= 0 && Index < ChunkFields.Num()) ChunkFields[Index] = IRONORE;
							}
						}
					}
				}
	}

	for (FIntVector DiamondOrePocketCenter : DiamondOrePocketCenters)
	{
		for (int x = -1; x < 1; ++x)
			for (int y = -1; y < 1; ++y)
				for (int z = -1; z < 1; ++z)
				{
					if (InRange(x + DiamondOrePocketCenter.X + 1, ChunkLineElements + 1) && InRange(y + DiamondOrePocketCenter.Y + 1, ChunkLineElements + 1) && InRange(z + DiamondOrePocketCenter.Z + 1, ChunkZElements))
					{
						float Radius = FVector(x * RandomX, y * RandomY, z * RandomZ).Size();
						if (Radius <= 2.8)
						{
							if (RandomStream.FRand() < 0.5 || Radius <= 1.4)
							{
								int32 Index = DiamondOrePocketCenter.X + x + ((DiamondOrePocketCenter.Y + y) * ChunkLineElementsExt) + ((DiamondOrePocketCenter.Z + z) * ChunkLineElementsP2);
								if (Index >= 0 && Index < ChunkFields.Num()) ChunkFields[Index] = DIAMONDORE;
							}
						}
					}
				}
	}
}

void AMCWorld_Voxel::GenerateGround(FIntVector Location, int32 Index, int32 inHeight, int32 Biome)
{
	if (Location.Z == (SurfaceHeight + inHeight + 1) && Location.Z > SeaLevelHeight && (RandomStream.FRand() * 100) < 80)
	{
		switch (Biome)
		{
		case 0:
			ChunkFields[Index] = (RandomStream.FRand() * 100) < 25 ? GRASSTALL : GRASSSHORT;
			break;
		}
	}
	else if (Location.Z > SurfaceHeight + inHeight && Location.Z < SeaLevelHeight)
	{
		ChunkFields[Index] = WATER;
	}
	else if (Location.Z == (SurfaceHeight + inHeight))
	{
		switch (Biome)
		{
		case 0:
			ChunkFields[Index] = GRASS;
			break;
		case 1:
			ChunkFields[Index] = SAND;
		default:
			ChunkFields[Index] = GRASS;
			break;
		}
	}
	else if ((Location.Z <= SurfaceHeight - 1 + inHeight) && (Location.Z > SurfaceHeight - 3 + inHeight))
	{
		switch (Biome)
		{
		case 0:
			ChunkFields[Index] = DIRT;
			break;
		case 1:
			ChunkFields[Index] = SAND;
		default:
			ChunkFields[Index] = DIRT;
			break;
		}
	}
	else if ((Location.Z <= SurfaceHeight - 3 + inHeight) && Location.Z > 0)
	{
		bool bOre = false;
		if (floor(RandomStream.FRandRange(1, 10000)) < 10)
			bOre = true;

		switch (Biome)
		{
		case 0:
			if (bOre) { IronOrePocketCenters.Add(Location); }
			else { ChunkFields[Index] = STONE; }
			break;
		default:
			ChunkFields[Index] = STONE;
			break;
		}
	}
	else if ((Location.Z <= SurfaceHeight - 10 + inHeight) && Location.Z > 0)
	{
		bool bOre = false;
		if ((RandomStream.FRand() * 10000) < 10)
			bOre = true;
		switch (Biome)
		{
		case 0:
			if (bOre) { DiamondOrePocketCenters.Add(Location); }
			else { ChunkFields[Index] = STONE; }
			break;
		default:
			ChunkFields[Index] = STONE;
			break;
		}
	}
	else if (Location.Z <= 0)
	{
		ChunkFields[Index] = BEDROCK;
	}
	else ChunkFields[Index] = AIR;
}

void AMCWorld_Voxel::BeforeChunkGenerated()
{
	RandomStream = FRandomStream(RandomSeed);
	USimplexNoiseBPLibrary::setNoiseSeed(RandomSeed);

	ChunkFields.SetNumZeroed(ChunkTotalElements);
	TreeType.SetNumZeroed(ChunkTotalElements);

	Height = CalcNoise();
	Temperature = CalcNoise();
	Moisture = CalcNoise();

	/*FString MyStringPrintf = FString(TEXT("Height Num: {0}, Temperature Num: {1}, Moisture Num: {2}"));
	FString FormattedText = FString::Format(*MyStringPrintf, { Height.Num(), Temperature.Num(), Moisture.Num() });
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FormattedText);*/
}

void AMCWorld_Voxel::ChunkGenerating(const FIntVector& CurrentLocation, int32 Index)
{
	int32 x = CurrentLocation.X;
	int32 y = CurrentLocation.Y;
	int32 z = CurrentLocation.Z;

	int32 H = Height.Num() > 0 ? Height[Index] : 0;
	int32 T = Temperature.Num() > 0 ? Temperature[x + (y * ChunkLineElementsExt)] : 0;
	int32 M = Moisture.Num() > 0 ? Moisture[x + (y * ChunkLineElementsExt)] : 0;

	/*FString MyStringPrintf = FString(TEXT("Current Index: {0}, Current Height {1}, Current Temperature {2}, Current Moisture {3}"));
	FString FormattedText = FString::Format(*MyStringPrintf, { Index, H, T, M });
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FormattedText);*/

	int32 B = 0;

	/** How Biome is determined:
	* 
	* Tempurature is 'COLD' 0, 1, 2
	* Tempurature is 'MODERATE' 3, 4, 5
	* Tempurature is 'HOT' 6, 7, 8
	* 
	* Moisture is 'DRY' 0, 3, 6
	* Moisture is 'PLEASENT' 1, 4, 7
	* Moisture is 'WET' 2, 5, 8
	* 
	* Biome is 'COLD' and 'DRY' 0
	* Biome is 'MODERATE' and 'DRY' 3
	* Biome is 'HOT' and 'DRY' 6
	*/
	if (T < 33)
	{
		if (M < 33)
		{
			B = 0;
		}
		else if (33 < M && M < 66)
		{
			B = 1;
		}
		else if (M > 66)
		{
			B = 2;
		}
	}
	else if (33 < T && T < 66)
	{
		if (M < 33)
		{
			B = 3;
		}
		else if (33 < M && M < 66)
		{
			B = 4;
		}
		else if (M > 66)
		{
			B = 5;
		}
	}
	else if (T > 66)
	{
		if (M < 33)
		{
			B = 6;
		}
		else if (33 < M && M < 66)
		{
			B = 7;
		}
		else if (M > 66)
		{
			B = 8;
		}
	}

	B = 0;

	GenerateGround(CurrentLocation, Index, H, B);

	// Tree Centers
	if ((x > 2 && x < ChunkLineElements - 2) && (y > 2 && y < ChunkLineElements - 2))
		if ((RandomStream.FRand() * 100) < 1 && z == SurfaceHeight + 1 + H && z > SeaLevelHeight)
		{
			TreeCenters.Add(CurrentLocation);
			TreeType[Index] = B;
		};

	// Town Centers
	if((x > 5 && x < ChunkLineElements - 5) && (y > 5 && y < ChunkLineElements - 5)) 
		if (RandomStream.FRand() < 0.0001 && z == SurfaceHeight + 1 + H && z > SeaLevelHeight)
		{
			TownCenters.Add(CurrentLocation);
		};

}

void AMCWorld_Voxel::AfterChunkGenerated()
{
	// Town
	GenerateTowns();

	// Tree
	GenerateTrees();

	// Ores
	GenerateOrePockets();
}

TArray<int32> AMCWorld_Voxel::CalcNoise_Implementation()
{
	TArray<int32> Value;

	Value.Reserve(ChunkTotalElements);

	float LocX = 0;
	float LocY = 0;
	float LocZ = 0;

	for (int z = 0; z < ChunkZElements; ++z)
	for (int y = 0; y < ChunkLineElementsExt; ++y)
	for (int x = 0; x < ChunkLineElementsExt; ++x)
	{
		LocX = (x + (ChunkIndex.X * ChunkLineElements)) * XMult;
		LocY = (y + (ChunkIndex.Y * ChunkLineElements)) * YMult;
		LocZ = (z + (ChunkIndex.Z * ChunkZElements)) * ZMult;

		float Noise0 = USimplexNoiseBPLibrary::SimplexNoise3D(LocX * 0.5, LocY * 0.5, LocZ * 0.5);
		float Noise1 = USimplexNoiseBPLibrary::SimplexNoise3D(LocX * 0.1, LocY * 0.1, LocZ * 0.1);
		float Noise2 = USimplexNoiseBPLibrary::SimplexNoise3D(LocX * 0.0025, LocY * 0.0025, LocZ * 0.025);
		Noise0 = (Noise0 + 1) / 2;
		Noise0 = Noise0 * 0.1;
		Noise1 = (Noise1 + 1) / 2;
		Noise1 = Noise1 * 2;
		Noise2 = (Noise2 + 1) / 2;
		Noise2 = Noise2 * 10;

		float Noise3 = FMath::Clamp<float>(USimplexNoiseBPLibrary::SimplexNoise3D(LocX / 10, LocY / 10, LocZ / 10) * 15.f, 0.f, 5.f);

		int32 FinalNoise = (int32)(((Noise0 + Noise1 + Noise2 + Noise3)) * Weight);
		// FinalNoise = FMath::Clamp<int32>(FinalNoise, 0, ChunkTotalElements);
		Value.Add(FinalNoise);
	}

	return Value;
}

void AMCWorld_Voxel::AddVoxelInstance_Implementation(FVector Location, int32 InstanceType)
{
	if (CustomMeshInstances.IsValidIndex(InstanceType) && CustomMeshInstances[InstanceType])
		CustomMeshInstances[InstanceType]->AddInstance(FTransform(Location));
}

bool AMCWorld_Voxel::InRange(int32 Value, int32 Range)
{
	return (Value >= 0 && Value < Range);
}
#pragma endregion

#pragma region CreateMeshes
//void AMCWorld_Voxel::CreateMeshFace(FProceduralMeshSection& MeshSection, int32 InIndex, const FIntVector& InLocation, int32& Triangle_Num)
//{
//	const auto x = InLocation.X;
//	const auto y = InLocation.Y;
//	const auto z = InLocation.Z;
//
//	MeshSection.Triangles.Add(VoxelTriangles[0] + Triangle_Num + MeshSection.ElementID);
//	MeshSection.Triangles.Add(VoxelTriangles[1] + Triangle_Num + MeshSection.ElementID);
//	MeshSection.Triangles.Add(VoxelTriangles[2] + Triangle_Num + MeshSection.ElementID);
//
//	MeshSection.Triangles.Add(VoxelTriangles[3] + Triangle_Num + MeshSection.ElementID);
//	MeshSection.Triangles.Add(VoxelTriangles[4] + Triangle_Num + MeshSection.ElementID);
//	MeshSection.Triangles.Add(VoxelTriangles[5] + Triangle_Num + MeshSection.ElementID);
//
//	Triangle_Num += 4;
//
//	// in Index represents each face of the new voxel we are generating
//	switch (InIndex)
//	{
//	case 0: {
//		// Top Face
//		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
//
//		MeshSection.Normals.Append(VoxelNormals0, UE_ARRAY_COUNT(VoxelNormals0));
//		break;
//	}
//	case 1: {
//		// Bottom Face
//		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
//
//		MeshSection.Normals.Append(VoxelNormals1, UE_ARRAY_COUNT(VoxelNormals1));
//		break;
//	}
//	case 2: {
//		// Right Face
//		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
//
//		MeshSection.Normals.Append(VoxelNormals2, UE_ARRAY_COUNT(VoxelNormals2));
//		break;
//	}
//	case 3: {
//		// Left Face
//		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
//
//		MeshSection.Normals.Append(VoxelNormals3, UE_ARRAY_COUNT(VoxelNormals3));
//		break;
//	}
//	case 4: {
//		// Front Face
//		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
//
//		MeshSection.Normals.Append(VoxelNormals4, UE_ARRAY_COUNT(VoxelNormals4));
//		break;
//	}
//	case 5: {
//		// Back Face
//		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
//		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
//
//		MeshSection.Normals.Append(VoxelNormals5, UE_ARRAY_COUNT(VoxelNormals5));
//		break;
//	}
//	}
//	MeshSection.UVs.Append(VoxelUVs, UE_ARRAY_COUNT(VoxelUVs));
//
//	FColor Color = FColor(255, 255, 255, InIndex);
//	MeshSection.VertexColors.Add(Color);
//	MeshSection.VertexColors.Add(Color);
//	MeshSection.VertexColors.Add(Color);
//	MeshSection.VertexColors.Add(Color);
//}
/*
void AMCWorld_Voxel::CreateStairMesh(FProceduralMeshSection& MeshSection, int32 InIndex, const FIntVector& InLocation, int32& Triangle_Num)
{
	const auto x = InLocation.X;
	const auto y = InLocation.Y;
	const auto z = InLocation.Z;

	MeshSection.Triangles.Add(VoxelTriangles[0] + Triangle_Num + MeshSection.ElementID);
	MeshSection.Triangles.Add(VoxelTriangles[1] + Triangle_Num + MeshSection.ElementID);
	MeshSection.Triangles.Add(VoxelTriangles[2] + Triangle_Num + MeshSection.ElementID);

	MeshSection.Triangles.Add(VoxelTriangles[3] + Triangle_Num + MeshSection.ElementID);
	MeshSection.Triangles.Add(VoxelTriangles[4] + Triangle_Num + MeshSection.ElementID);
	MeshSection.Triangles.Add(VoxelTriangles[5] + Triangle_Num + MeshSection.ElementID);

	Triangle_Num += 4;

	FColor Color = FColor(255, 255, 255, InIndex);

	// in Index represents each face of the new voxel we are generating
	switch (InIndex)
	{
	case 0: {
		// Top Face
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector((x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector((x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals0, UE_ARRAY_COUNT(VoxelNormals0));

		MeshSection.VertexColors.Add(Color);
		MeshSection.VertexColors.Add(Color);
		MeshSection.VertexColors.Add(Color);
		MeshSection.VertexColors.Add(Color);

		const FVector2D UVs1[] = { FVector2D(0.00000, 0.00000), FVector2D(0.00000, 0.50000), FVector2D(1.00000, 0.50000), FVector2D(1.00000, 0.00000) };
		MeshSection.UVs.Append(UVs1, UE_ARRAY_COUNT(UVs1));

		MeshSection.Triangles.Add(VoxelTriangles[0] + Triangle_Num + MeshSection.ElementID);
		MeshSection.Triangles.Add(VoxelTriangles[1] + Triangle_Num + MeshSection.ElementID);
		MeshSection.Triangles.Add(VoxelTriangles[2] + Triangle_Num + MeshSection.ElementID);

		MeshSection.Triangles.Add(VoxelTriangles[3] + Triangle_Num + MeshSection.ElementID);
		MeshSection.Triangles.Add(VoxelTriangles[4] + Triangle_Num + MeshSection.ElementID);
		MeshSection.Triangles.Add(VoxelTriangles[5] + Triangle_Num + MeshSection.ElementID);

		Triangle_Num += 4;

		MeshSection.Vertices.Add(FVector((x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector((x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals0, UE_ARRAY_COUNT(VoxelNormals0));

		const FVector2D UVs2[] = { FVector2D(0.00000, 0.50000), FVector2D(0.00000, 1.00000), FVector2D(1.00000, 1.00000), FVector2D(1.00000, 0.5000) };
		MeshSection.UVs.Append(UVs2, UE_ARRAY_COUNT(UVs2));
		break;
	}
	case 1: {
		// Bottom Face
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals1, UE_ARRAY_COUNT(VoxelNormals1));
		MeshSection.UVs.Append(VoxelUVs, UE_ARRAY_COUNT(VoxelUVs));
		break;

	}
	case 2: {
		// Right Face
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector((x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector((x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals0, UE_ARRAY_COUNT(VoxelNormals0));

		MeshSection.VertexColors.Add(Color);
		MeshSection.VertexColors.Add(Color);
		MeshSection.VertexColors.Add(Color);
		MeshSection.VertexColors.Add(Color);

		const FVector2D UVs1[] = { FVector2D(0.00000, 0.00000), FVector2D(0.00000, 0.50000), FVector2D(1.00000, 0.50000), FVector2D(1.00000, 0.00000) };
		MeshSection.UVs.Append(UVs1, UE_ARRAY_COUNT(UVs1));

		MeshSection.Triangles.Add(VoxelTriangles[0] + Triangle_Num + MeshSection.ElementID);
		MeshSection.Triangles.Add(VoxelTriangles[1] + Triangle_Num + MeshSection.ElementID);
		MeshSection.Triangles.Add(VoxelTriangles[2] + Triangle_Num + MeshSection.ElementID);

		MeshSection.Triangles.Add(VoxelTriangles[3] + Triangle_Num + MeshSection.ElementID);
		MeshSection.Triangles.Add(VoxelTriangles[4] + Triangle_Num + MeshSection.ElementID);
		MeshSection.Triangles.Add(VoxelTriangles[5] + Triangle_Num + MeshSection.ElementID);

		Triangle_Num += 4;

		MeshSection.Vertices.Add(FVector((x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector((x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals2, UE_ARRAY_COUNT(VoxelNormals2));

		const FVector2D UVs2[] = { FVector2D(0.50000, 0.00000), FVector2D(0.50000, 1.00000), FVector2D(1.00000, 1.00000), FVector2D(1.00000, 0.00000) };
		MeshSection.UVs.Append(UVs2, UE_ARRAY_COUNT(UVs2));
		break;
	}
	case 3: {
		// Left Face
		MeshSection.Vertices.Add(FVector((x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector((x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals0, UE_ARRAY_COUNT(VoxelNormals0));

		MeshSection.VertexColors.Add(Color);
		MeshSection.VertexColors.Add(Color);
		MeshSection.VertexColors.Add(Color);
		MeshSection.VertexColors.Add(Color);

		const FVector2D UVs1[] = { FVector2D(0.00000, 0.00000), FVector2D(0.00000, 0.50000), FVector2D(0.50000, 0.50000), FVector2D(0.50000, 0.00000) };
		MeshSection.UVs.Append(UVs1, UE_ARRAY_COUNT(UVs1));

		MeshSection.Triangles.Add(VoxelTriangles[0] + Triangle_Num + MeshSection.ElementID);
		MeshSection.Triangles.Add(VoxelTriangles[1] + Triangle_Num + MeshSection.ElementID);
		MeshSection.Triangles.Add(VoxelTriangles[2] + Triangle_Num + MeshSection.ElementID);

		MeshSection.Triangles.Add(VoxelTriangles[3] + Triangle_Num + MeshSection.ElementID);
		MeshSection.Triangles.Add(VoxelTriangles[4] + Triangle_Num + MeshSection.ElementID);
		MeshSection.Triangles.Add(VoxelTriangles[5] + Triangle_Num + MeshSection.ElementID);

		Triangle_Num += 4;

		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector((x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector((x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals3, UE_ARRAY_COUNT(VoxelNormals3));

		const FVector2D UVs2[] = { FVector2D(0.50000, 0.00000), FVector2D(0.50000, 1.00000), FVector2D(1.00000, 1.00000), FVector2D(1.00000, 0.00000) };
		MeshSection.UVs.Append(UVs2, UE_ARRAY_COUNT(UVs2));
		break;
	}
	case 4: {
		// Front Face
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals4, UE_ARRAY_COUNT(VoxelNormals4));

		MeshSection.VertexColors.Add(Color);
		MeshSection.VertexColors.Add(Color);
		MeshSection.VertexColors.Add(Color);
		MeshSection.VertexColors.Add(Color);

		const FVector2D UVs1[] = { FVector2D(0.00000, 0.00000), FVector2D(0.00000, 0.50000), FVector2D(1.00000, 0.50000), FVector2D(1.00000, 0.00000) };
		MeshSection.UVs.Append(UVs1, UE_ARRAY_COUNT(UVs1));

		MeshSection.Triangles.Add(VoxelTriangles[0] + Triangle_Num + MeshSection.ElementID);
		MeshSection.Triangles.Add(VoxelTriangles[1] + Triangle_Num + MeshSection.ElementID);
		MeshSection.Triangles.Add(VoxelTriangles[2] + Triangle_Num + MeshSection.ElementID);

		MeshSection.Triangles.Add(VoxelTriangles[3] + Triangle_Num + MeshSection.ElementID);
		MeshSection.Triangles.Add(VoxelTriangles[4] + Triangle_Num + MeshSection.ElementID);
		MeshSection.Triangles.Add(VoxelTriangles[5] + Triangle_Num + MeshSection.ElementID);

		Triangle_Num += 4;

		MeshSection.Vertices.Add(FVector((x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector((x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize),  (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector((x * VoxelSize), VoxelSizeHalf + (y * VoxelSize),  (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector((x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals4, UE_ARRAY_COUNT(VoxelNormals4));

		const FVector2D UVs2[] = { FVector2D(0.00000, 0.50000), FVector2D(0.00000, 1.00000), FVector2D(1.00000, 1.00000), FVector2D(1.00000, 0.50000) };
		MeshSection.UVs.Append(UVs2, UE_ARRAY_COUNT(UVs2));
		break;
	}
	case 5: {
		// Back Face
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals5, UE_ARRAY_COUNT(VoxelNormals5));
		MeshSection.UVs.Append(VoxelUVs, UE_ARRAY_COUNT(VoxelUVs));
		break;
	}
	}

	MeshSection.VertexColors.Add(Color);
	MeshSection.VertexColors.Add(Color);
	MeshSection.VertexColors.Add(Color);
	MeshSection.VertexColors.Add(Color);
}*/
/*
void AMCWorld_Voxel::CreateSlabMesh(FProceduralMeshSection& MeshSection, int32 InIndex, const FIntVector& InLocation, int32& Triangle_Num)
{
	const auto x = InLocation.X;
	const auto y = InLocation.Y;
	const auto z = InLocation.Z;

	MeshSection.Triangles.Add(VoxelTriangles[0] + Triangle_Num + MeshSection.ElementID);
	MeshSection.Triangles.Add(VoxelTriangles[1] + Triangle_Num + MeshSection.ElementID);
	MeshSection.Triangles.Add(VoxelTriangles[2] + Triangle_Num + MeshSection.ElementID);

	MeshSection.Triangles.Add(VoxelTriangles[3] + Triangle_Num + MeshSection.ElementID);
	MeshSection.Triangles.Add(VoxelTriangles[4] + Triangle_Num + MeshSection.ElementID);
	MeshSection.Triangles.Add(VoxelTriangles[5] + Triangle_Num + MeshSection.ElementID);

	Triangle_Num += 4;

	// in Index represents each face of the new voxel we are generating
	switch (InIndex)
	{
	case 0: {
		// Top Face
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals0, UE_ARRAY_COUNT(VoxelNormals0));
		break;
	}
	case 1: {
		// Bottom Face
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals1, UE_ARRAY_COUNT(VoxelNormals1));
		break;

	}
	case 2: {
		// Right Face
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals2, UE_ARRAY_COUNT(VoxelNormals2));
		break;
	}
	case 3: {
		// Left Face
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals3, UE_ARRAY_COUNT(VoxelNormals3));
		break;
	}
	case 4: {
		// Front Face
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals4, UE_ARRAY_COUNT(VoxelNormals4));
		break;
	}
	case 5: {
		// Back Face
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeHalf + (x * VoxelSize), -VoxelSizeHalf + (y * VoxelSize), (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals5, UE_ARRAY_COUNT(VoxelNormals5));
		break;
	}
	}
	MeshSection.UVs.Append(VoxelUVs, UE_ARRAY_COUNT(VoxelUVs));

	FColor Color = FColor(255, 255, 255, InIndex);
	MeshSection.VertexColors.Add(Color);
	MeshSection.VertexColors.Add(Color);
	MeshSection.VertexColors.Add(Color);
	MeshSection.VertexColors.Add(Color);
}

void AMCWorld_Voxel::CreatePostMesh(FProceduralMeshSection& MeshSection, int32 InIndex, const FIntVector& InLocation, int32& Triangle_Num)
{
	const auto x = InLocation.X;
	const auto y = InLocation.Y;
	const auto z = InLocation.Z;

	MeshSection.Triangles.Add(VoxelTriangles[0] + Triangle_Num + MeshSection.ElementID);
	MeshSection.Triangles.Add(VoxelTriangles[1] + Triangle_Num + MeshSection.ElementID);
	MeshSection.Triangles.Add(VoxelTriangles[2] + Triangle_Num + MeshSection.ElementID);

	MeshSection.Triangles.Add(VoxelTriangles[3] + Triangle_Num + MeshSection.ElementID);
	MeshSection.Triangles.Add(VoxelTriangles[4] + Triangle_Num + MeshSection.ElementID);
	MeshSection.Triangles.Add(VoxelTriangles[5] + Triangle_Num + MeshSection.ElementID);

	Triangle_Num += 4;

	// in Index represents each face of the new voxel we are generating
	switch (InIndex)
	{
	case 0: {
		// Top Face
		MeshSection.Vertices.Add(FVector(-VoxelSizeBit + (x * VoxelSize), VoxelSizeBit + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeBit + (x * VoxelSize), -VoxelSizeBit + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeBit + (x * VoxelSize), -VoxelSizeBit + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeBit + (x * VoxelSize), VoxelSizeBit + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals0, UE_ARRAY_COUNT(VoxelNormals0));
		break;
	}
	case 1: {
		// Bottom Face
		MeshSection.Vertices.Add(FVector(VoxelSizeBit + (x * VoxelSize), -VoxelSizeBit + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeBit + (x * VoxelSize), -VoxelSizeBit + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeBit + (x * VoxelSize), VoxelSizeBit + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeBit + (x * VoxelSize), VoxelSizeBit + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals1, UE_ARRAY_COUNT(VoxelNormals1));
		break;
	}
	case 2: {
		// Right Face
		MeshSection.Vertices.Add(FVector(VoxelSizeBit + (x * VoxelSize), VoxelSizeBit + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeBit + (x * VoxelSize), VoxelSizeBit + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeBit + (x * VoxelSize), VoxelSizeBit + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeBit + (x * VoxelSize), VoxelSizeBit + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals2, UE_ARRAY_COUNT(VoxelNormals2));
		break;
	}
	case 3: {
		// Left Face
		MeshSection.Vertices.Add(FVector(-VoxelSizeBit + (x * VoxelSize), -VoxelSizeBit + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeBit + (x * VoxelSize), -VoxelSizeBit + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeBit + (x * VoxelSize), -VoxelSizeBit + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeBit + (x * VoxelSize), -VoxelSizeBit + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals3, UE_ARRAY_COUNT(VoxelNormals3));
		break;
	}
	case 4: {
		// Front Face
		MeshSection.Vertices.Add(FVector(VoxelSizeBit + (x * VoxelSize), -VoxelSizeBit + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeBit + (x * VoxelSize), -VoxelSizeBit + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeBit + (x * VoxelSize), VoxelSizeBit + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(VoxelSizeBit + (x * VoxelSize), VoxelSizeBit + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals4, UE_ARRAY_COUNT(VoxelNormals4));
		break;
	}
	case 5: {
		// Back Face
		MeshSection.Vertices.Add(FVector(-VoxelSizeBit + (x * VoxelSize), VoxelSizeBit + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeBit + (x * VoxelSize), VoxelSizeBit + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeBit + (x * VoxelSize), -VoxelSizeBit + (y * VoxelSize), -VoxelSizeHalf + (z * VoxelSize)));
		MeshSection.Vertices.Add(FVector(-VoxelSizeBit + (x * VoxelSize), -VoxelSizeBit + (y * VoxelSize), VoxelSizeHalf + (z * VoxelSize)));

		MeshSection.Normals.Append(VoxelNormals5, UE_ARRAY_COUNT(VoxelNormals5));
		break;
	}
	}
	MeshSection.UVs.Append(VoxelUVs, UE_ARRAY_COUNT(VoxelUVs));

	FColor Color = FColor(255, 255, 255, InIndex);
	MeshSection.VertexColors.Add(Color);
	MeshSection.VertexColors.Add(Color);
	MeshSection.VertexColors.Add(Color);
	MeshSection.VertexColors.Add(Color);
}*/
#pragma endregion

#pragma region Updating
void AMCWorld_Voxel::UpdateChunkFromData()
{

}

void AMCWorld_Voxel::UpdateMesh()
{
	if (!BlockDataTable) return;

	int32 NumSections = 0;
	//TArray<UMaterialInterface*> Materials;
	TArray<FDTMCBlockInfo*> BlockData;

	BlockDataTable->GetAllRows<FDTMCBlockInfo>("", BlockData);
	/*Materials.Reserve(BlockData.Num());
	for (const FDTMCBlockInfo* Block : BlockData)
	{
		if (!Block) continue;
		if (Materials.IsEmpty()) Materials.Add(Block->Material);
		if (Materials.Contains(Block->Material)) continue;

		Materials.Add(Block->Material);
	}
	NumSections = Materials.Num();*/
	NumSections = BlockData.Num();
	TArray<FProceduralMeshSection> MeshSections;
	MeshSections.SetNum(NumSections);

	for (int32 z = 0; z < ChunkZElements; z += CurrentLOD)
	{
		int zIndex = (ChunkLineElementsP2Ext * (z));
		for (int32 y = 0; y < ChunkLineElements; y += CurrentLOD)
		{
			int yIndex = (ChunkLineElementsExt * (y + 1));
			for (int32 x = 0; x < ChunkLineElements; x += CurrentLOD)
			{
				int32 Index = (x + 1) + yIndex + zIndex;
				if (Index < 0 && Index >= ChunkFields.Num()) continue;
				int32 MeshIndex = ChunkFields[Index];

				if (MeshIndex > 0)
				{
					/*
						when zero, this never gets fired and results in an empty block.

						However:
							- we have a zero based array for materials so Mesh index must be reduced for proper material
							- selection.
							- IE: MeshIndex is 0 then it is an air block;
							-     MeshIndex is 1 then we find the material at index 0 so we must subtract 1.
					*/
					--MeshIndex;

					// If mesh index is out of bounds, restart loop.
					if (MeshIndex >= NumSections || MeshIndex < 0) continue;

					FDTMCBlockInfo* Data = BlockDataTable->FindRow<FDTMCBlockInfo>(BlockDataTable->GetRowNames()[MeshIndex], "");

					if (!Data || Data->BlockType < 0 || BlockTypes.IsEmpty() || !BlockTypes.IsValidIndex(Data->BlockType)) continue; // Restart loop; DO NOT EXECUTE FURTHER AFTER THIS IF TRUE

					bool MeshTransparent = Data->bIsTransparent;
					MeshSections[MeshIndex].Material = Data->Material;
					UGameplayTileData* MeshData = BlockTypes[Data->BlockType];

					TArray<FVector>& Vertices = MeshSections[MeshIndex].Vertices;
					TArray<int32>& Triangles = MeshSections[MeshIndex].Triangles;
					TArray<FVector>& Normals = MeshSections[MeshIndex].Normals;
					TArray<FVector2D>& UVs = MeshSections[MeshIndex].UVs;
					TArray<FColor>& VertexColors = MeshSections[MeshIndex].VertexColors;
					int32 ElementID = MeshSections[MeshIndex].ElementID;

					// Vertices, Faces, UVs, Normals generator
					int Triangle_Num = 0;
					for (int i = 0; i < 6; i++)
					{
						// Index of voxel neighboring the current face
						int NewIndex = Index + VoxelMask[i].X +
							(VoxelMask[i].Y * ChunkLineElementsExt) + /* Is equal to 0 based on Voxel mask */
							(VoxelMask[i].Z * ChunkLineElementsP2Ext); /* Is equal to 0 based on Voxel mask */

						bool Flag = false;
						
						// Is this side a neighbor with an existing side?
						if (NewIndex >= 0 && NewIndex < ChunkFields.Num())
						{
							int32 NewMeshIndex = ChunkFields[NewIndex]; 

							// Are our neighbors not Air?
							if (NewMeshIndex > 0)
							{
								// Treat this face like we are empty?
								if (MeshTransparent)
								{
									// Is our neighbor a different block than I am?
									if (NewMeshIndex != MeshIndex)
									{
										const TArray<FName>& Names = BlockDataTable->GetRowNames();
										if (Names.IsValidIndex(NewMeshIndex))
										{
											if (FDTMCBlockInfo* newData = BlockDataTable->FindRow<FDTMCBlockInfo>(Names[NewMeshIndex], ""))
											{
												// Are our neighbors acting like they are empty?
												if (newData->bIsTransparent) Flag = true;
											}
										}
									}
									else
									{
										Flag = true;
									}
								} 
								else
								{
									--NewMeshIndex;
									const TArray<FName>& Names = BlockDataTable->GetRowNames();
									if (Names.IsValidIndex(NewMeshIndex))
									{
										if (FDTMCBlockInfo* newData = BlockDataTable->FindRow<FDTMCBlockInfo>(Names[NewMeshIndex], ""))
										{
											// Are our neighbors acting like they are empty?
											if (newData->bIsTransparent) Flag = true;
										}
									}
								}
							}
							else
							{
								Flag = true;
							}
						}

						if (Flag)
						{
							// Water gets seperate mesh...
							if (Data->BlockType == WATER - 1)
							{
								// if (WaterMesh)
							}

							// CreateMeshFace(MeshSections[MeshIndex], i, FIntVector(x, y, z), Triangle_Num);
							// const FVoxelFaceData FaceData = FVoxelFaceData(MeshData, FIntVector(x, y, z), i, Triangle_Num, VoxelSize, 1);
							Triangle_Num = UGALibrary::CreateFace(MeshSections[MeshIndex], MeshData, FVector(x, y, z), i, Triangle_Num, VoxelSize, CurrentLOD);
						}
					}
					MeshSections[MeshIndex].ElementID += Triangle_Num;
					MeshSections[MeshIndex].bEnableCollision = MeshIndex == WATER - 1 ? false : bEnableCollision;
				}
				else if (MeshIndex < 0)
				{
					AddVoxelInstance(FVector(x * VoxelSizeHalf, y * VoxelSizeHalf, z * VoxelSize), abs(MeshIndex) - 1);
				}
			}
		}
	}

	if (!PMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Procedural Mesh is null!"));
		return;
	}

	PMesh->ClearAllMeshSections();

	// For each MeshSection, Create a new mesh section
	for (int i = 0; i < MeshSections.Num(); ++i)
	{
		// if (i == WATER - 1) WaterMesh->CreateWaterMesh(MeshSections[i]);
		if (MeshSections[i].Vertices.Num() <= 0) continue;
		PMesh->CreateMeshSection(i, MeshSections[i].Vertices, MeshSections[i].Triangles, MeshSections[i].Normals, MeshSections[i].UVs, MeshSections[i].VertexColors, MeshSections[i].Tangents, MeshSections[i].bEnableCollision);
		PMesh->SetMaterial(i, MeshSections[i].Material);
	}
}

void AMCWorld_Voxel::UpdateExtras()
{
}
#pragma endregion

#pragma region Interaction
void AMCWorld_Voxel::SetVoxel(FVector Location, int32 Value)
{
	// maybe location divide by ChunkLineElementsP2?
	int32 x = Location.X / VoxelSize;
	int32 y = Location.Y / VoxelSize;
	int32 z = Location.Z / VoxelSize;

	int32 Index = x + (y * ChunkLineElementsExt) + (z * ChunkLineElementsP2Ext);
	
	if (bSpawnItems)
	{
		int32 ChunkField = ChunkFields[Index];
		if (Value == 0 && ChunkField != 0)
		{
			if (ChunkField > 0)
			{
				FTransform Transform = FTransform(FRotator(), FVector(), FVector(1, 1, 1));

				AMCWorld_VoxelItem* Item = GetWorld()->SpawnActorDeferred<AMCWorld_VoxelItem>(AMCWorld_VoxelItem::StaticClass(), Transform);

				if (Item)
				{
					Transform = FTransform(FRotator(), FVector(x, y, z), FVector(1, 1, 1));

					FDTMCBlockInfo* Data = BlockDataTable->FindRow<FDTMCBlockInfo>(BlockDataTable->GetRowNames()[ChunkField], "");
					Item->Material = Data->Material;
					Item->bMini = true;

					Item->FinishSpawning(Transform);
					GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Purple, Transform.GetLocation().ToString());
					GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Purple, "The item should be spawned!");
				}

			}
			else // Value == 0 && ChunkFields[Index] < 0
			{
				FTransform Transform = FTransform(FRotator(), Location, FVector(1, 1, 1));

				AMCWorld_Instance* Item = GetWorld()->SpawnActorDeferred<AMCWorld_Instance>(AMCWorld_Instance::StaticClass(), Transform);

				// Item->;
				// Item->bMini = true;

				Item->FinishSpawning(Transform);
			}
		}
	}

	ChunkFields[Index] = Value;

	UpdateMesh();
}

void AMCWorld_Voxel::VoxelInteract(FVector Location, AActor* InteractingActor)
{
	int32 x = Location.X * VoxelSize;
	int32 y = Location.Y * VoxelSize;
	int32 z = Location.Z * VoxelSize;

	int32 Index = x + (y * ChunkLineElementsExt) + (z * ChunkLineElementsP2Ext);

	switch(ChunkFields[Index])
	{
		case CRAFTINGTABLE:
			break;
		default:
			break;
	}

}
#pragma endregion
