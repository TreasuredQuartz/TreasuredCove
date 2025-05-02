// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/HexGrid/HexGridManager.h"
#include "Global/HexGrid/HexGrid.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AHexGridManager::AHexGridManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RenderRange = 6;
	VoxelSize = 50;
	ChunkLineElements = 10;
	VoxelClass = AHexGrid::StaticClass();

	bIsActive = false;
}

//
void AHexGridManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// AddChunk();
}

// Called when the game starts or when spawned
void AHexGridManager::BeginPlay()
{
	ClearChunks();
	AddChunk();

	AActor::BeginPlay();
}

// Called every frame
void AHexGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void Subdivide(const FVector3d& v1, const FVector3d& v2, const FVector3d& v3, TArray<FVector3d>& spherePoints, const unsigned int depth) {
	if (depth == 0) {
		spherePoints.Push(v1);
		spherePoints.Push(v2);
		spherePoints.Push(v3);
		return;
	}
	const FVector3d v12 = (v1 + v2).GetUnsafeNormal();
	const FVector3d v23 = (v2 + v3).GetUnsafeNormal();
	const FVector3d v31 = (v3 + v1).GetUnsafeNormal();
	Subdivide(v1, v12, v31, spherePoints, depth - 1);
	Subdivide(v2, v23, v12, spherePoints, depth - 1);
	Subdivide(v3, v31, v23, spherePoints, depth - 1);
	Subdivide(v12, v23, v31, spherePoints, depth - 1);
}

void InitializeIcoSphere(TArray<FVector3d>& spherePoints, const unsigned int depth) {
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
		Subdivide(vdata[tindices[i][0]], vdata[tindices[i][1]], vdata[tindices[i][2]], spherePoints, depth);

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

void InitializeGoldenSpiralSphere(TArray<FVector3d>& spherePoints) {
	int32 n = 100; // number of points
	double c = 1; // scale factor

	for (int32 i = 0; i < n; ++i) {
		double theta = i * FMath::DegreesToRadians(137.5);
		double r = c * FMath::Sqrt(i + 0.0);
		double x = FMath::Cos(theta) * r;
		double y = FMath::Sin(theta) * r;
		double z = 0.0;
		FVector p = FVector(x, y, z);
		spherePoints.Add(p);
	}
}

void AHexGridManager::AddChunk()
{
	UE_LOG(LogTemp, Warning, TEXT("Adding Chunk!"));

	if (!bSphere)
	{
		if (RenderRange <= 0)
		{
			AddVoxelActor(FVector(ActiveChunkCoords), ActiveChunkCoords, 1);
			return;
		}

		int32 a, b, c;
		const FVector VoxelNormal = FVector::UpVector;
		FVector axisA;
		FVector axisB;
		FVector axisC;

		// Built-In utility function for returning a perpendicular plane to the given Vector
		VoxelNormal.FindBestAxisVectors(axisA, axisB);

		// axisA and axisB are 90 degrees apart when each vector should be 30 degrees apart.
		axisA = axisA.RotateAngleAxis(90.0f, VoxelNormal);
		axisC = axisA.RotateAngleAxis(60.0f, VoxelNormal);
		axisB = axisB.ProjectOnToNormal(VoxelNormal);

		axisA.Normalize();
		axisB.Normalize();
		axisC.Normalize();

		FVector Position = (GetActorLocation() / (ChunkSize * 2));
		Position.Z = -Position.X - Position.Y;

		int32 N = RenderRange;
		for (a = -N; a <= N; a++) {
			int32 b1 = FMath::Max(-N, -a - N);
			int32 b2 = FMath::Min(N, -a + N);
			for (b = b1; b <= b2; b++) {
				c = -a - b;

				int32 LocA = a + Position.X;
				int32 LocB = b + Position.Y;
				int32 LocC = c + Position.Z;

				int32 ChunkOffset = ChunkSize * 2;
				int32 AScaled = LocA * ChunkOffset;
				int32 BScaled = LocB * ChunkOffset;
				int32 CScaled = LocC * ChunkOffset;

				int32 XCenter = AScaled + ChunkSizeHalf;
				int32 YCenter = BScaled + ChunkSizeHalf;
				int32 ZCenter = CScaled + ChunkSizeHalf;

				FIntVector LocVector = FIntVector(LocA, LocB, LocC);

				// if chunk is outside play area, don't spawn
				//if (!InPlayArea(XCenter, YCenter)) {
				//	continue;
				//}

				// if Chunk is in player radius space...
				// if (CHECK_RADIUS(XCenter, YCenter, ZCenter))
				{
					// And if Chunk does not exist in coords...
					if (!ChunkCoords.Contains(LocVector))
					{
						FVector Location = 
							(XCenter * axisA) + 
							(YCenter * axisB) + 
							(ZCenter * axisC);

						Location.Z = GetActorLocation().Z;

						// UE_LOG(LogTemp, Warning, TEXT("Should Add Voxel Actor!"));
						// int32 Distance = FMath::Abs(x) + FMath::Abs(y);
						// int32 LOD = 0;
						// if (Distance <= 1) LOD = 1;
						// else if (Distance <= 3) LOD = 2;
						// else LOD = 4;
						AddVoxelActor(Location, LocVector, 1);
					}
					else {
						GEngine->AddOnScreenDebugMessage(3, 10.f, FColor::Red, "Chunk updated!");
						int32 index = ChunkCoords.IndexOfByKey(LocVector);
						if (Chunks.IsValidIndex(index) && Chunks[index]) Chunks[index]->UpdateChunk(LocVector, 1);
					}
				}
			}
		}
	} 
	else 
	{
		FVector Position = FVector::UpVector;
		int32 HexGridSize = ChunkLineElements * VoxelSize * VoxelSize;
		TArray<FVector3d> SpherePoints;
		InitializeGoldenSpiralSphere(SpherePoints);
		for (const FVector3d& Point : SpherePoints) {
			FIntVector VoxelIndex = FIntVector(FMath::Floor(Point.X * ChunkSize), FMath::Floor(Point.Y * ChunkSize), FMath::Floor(Point.Z * ChunkSize));
			if (!ChunkCoords.Contains(VoxelIndex))
			{
				FVector VoxelLocation = GetActorLocation() + Point * HexGridSize;
				AddVoxelActor(VoxelLocation, VoxelIndex, 0);
			}
		}

		/*
		TArray<FVector> Directions = {
			FVector::UpVector,
			FVector::DownVector,
			FVector::RightVector,
			FVector::LeftVector,
			FVector::ForwardVector,
			FVector::BackwardVector
		};
		TArray<FVector> Sides = {
			Directions[0] +
			Directions[2] +
			Directions[4],

			Directions[0] +
			Directions[3] +
			Directions[4],

			Directions[0] +
			Directions[2] +
			Directions[5],

			Directions[0] +
			Directions[3] +
			Directions[5],

			Directions[1] +
			Directions[2] +
			Directions[4],

			Directions[1] +
			Directions[3] +
			Directions[4],

			Directions[1] +
			Directions[2] +
			Directions[5],

			Directions[1] +
			Directions[3] +
			Directions[5],
		};
		// 
		// InitializeSphere(Sides, 0);
		Chunks.Reserve(Directions.Num());

		FIntVector VoxelIndex = ActiveChunkCoords;
		for (const FVector& Side : Directions)
		{
			if (GetWorld())
			{
				if (AHexGrid* NewVoxel = GetWorld()->SpawnActorDeferred<AHexGrid>(VoxelClass, FTransform(GetActorLocation())))
				{
					// UE_LOG(LogTemp, Warning, TEXT("Adding Voxel Actor: %s"), *VoxelSpawnLocation.ToString());
					Chunks.Add(NewVoxel);
					ChunkCoords.Add(VoxelIndex);
					NewVoxel->SetRandomSeed(RandomSeed);
					NewVoxel->SetVoxelSize(VoxelSize);
					NewVoxel->SetChunkZElements(ChunkZElements);
					NewVoxel->SetChunkLineElements(ChunkLineElements);
					NewVoxel->SetChunkIndex(VoxelIndex);
					NewVoxel->SetCurrentLOD(0);
					NewVoxel->NormalVector = Side.GetUnsafeNormal();
					NewVoxel->SetSphere(bSphere);
					// NewVoxel->GetRootComponent()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
					// NewVoxel->GetRootComponent()->RegisterComponent();
					// NewVoxel->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
					// Register
					UGameplayStatics::FinishSpawningActor(NewVoxel, FTransform(GetActorLocation()));
					NewVoxel->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));

					++VoxelIndex.X;
				}
			}
		} // */

		/* 
		FVector axisA;
		FVector axisB;
		FVector axisC;

		// Built-In utility function for returning a perpendicular plane to the given Vector
		Position.FindBestAxisVectors(axisA, axisB);

		// axisA and axisB are 90 degrees apart when each vector should be 30 degrees apart.
		axisA = axisA.RotateAngleAxis(90.0f, Position);
		axisC = axisA.RotateAngleAxis(60.0f, Position);
		axisB = axisB.ProjectOnToNormal(Position);

		axisA.Normalize();
		axisB.Normalize();
		axisC.Normalize(); */
		// double a, b, c;


		/* int32 N = RenderRange;
		int32 Depth = SphereDepth;
		int32 HexGridSize = ChunkLineElements * ChunkLineElements * VoxelSize * Depth;
		TArray<FVector> SpherePoints;
		InitializeIcoSphere(SpherePoints, Depth);
		if (GetWorld() && RenderRange > 0) {
		for (const FVector& Point : SpherePoints) {
				FVector VoxelNormal = Point.GetClampedToSize(1.0, 1.0);
				if (VoxelNormal.Dot(RenderAngle) > (1 / RenderRange)) continue;

				FTransform VoxelTransform = FTransform(GetActorLocation() + VoxelNormal * HexGridSize);
				if (AHexGrid* NewVoxel = GetWorld()->SpawnActorDeferred<AHexGrid>(VoxelClass, VoxelTransform)) {
					// UE_LOG(LogTemp, Warning, TEXT("Adding Voxel Actor: %s"), *VoxelSpawnLocation.ToString());
					Chunks.Add(NewVoxel);
					ChunkCoords.Add(VoxelIndex);
					NewVoxel->SetRandomSeed(RandomSeed);
					NewVoxel->SetVoxelSize(VoxelSize);
					NewVoxel->SetChunkZElements(ChunkZElements);
					NewVoxel->SetChunkLineElements(ChunkLineElements);
					NewVoxel->SetChunkIndex(VoxelIndex);
					NewVoxel->SetCurrentLOD(0);
					NewVoxel->NormalVector = VoxelNormal;
					// NewVoxel->GetRootComponent()->SetVisibility(false, true);
					// NewVoxel->SetActorHiddenInGame(true);
					UGameplayStatics::FinishSpawningActor(NewVoxel, VoxelTransform);
					NewVoxel->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
				}
			}
		} // */

		/* int32 SphereXYRings = 4;
		for (int32 i = -SphereXYRings; i <= SphereXYRings; i++) {
			if (FMath::Abs(i) == SphereXYRings) {
				if (GetWorld()) {
					FVector VoxelNormal = (i > 0 ? FVector::UpVector : FVector::DownVector);
					FTransform VoxelTransform = FTransform(GetActorLocation() + VoxelNormal * HexGridSize);
					if (AHexGrid* NewVoxel = GetWorld()->SpawnActorDeferred<AHexGrid>(VoxelClass, VoxelTransform)) {
						// UE_LOG(LogTemp, Warning, TEXT("Adding Voxel Actor: %s"), *VoxelSpawnLocation.ToString());
						Chunks.Add(NewVoxel);
						ChunkCoords.Add(VoxelIndex);
						NewVoxel->SetRandomSeed(RandomSeed);
						NewVoxel->SetVoxelSize(VoxelSize);
						NewVoxel->SetChunkZElements(ChunkZElements);
						NewVoxel->SetChunkLineElements(ChunkLineElements);
						NewVoxel->SetChunkIndex(VoxelIndex);
						NewVoxel->SetCurrentLOD(0);
						NewVoxel->NormalVector = VoxelNormal;
						// NewVoxel->SetSphere(bSphere);
						// NewVoxel->GetRootComponent()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
						// NewVoxel->GetRootComponent()->RegisterComponent();
						// NewVoxel->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
						// Register
						UGameplayStatics::FinishSpawningActor(NewVoxel, VoxelTransform);
						NewVoxel->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
					}
				}
			}
			else {
				int32 SphereRingMax = 6 * (SphereXYRings - FMath::Abs(i));
				for (int32 SphereZRing = 0; SphereZRing <= SphereRingMax; SphereZRing++) {

					bool bMiddleRung = FMath::Abs(i) < SphereXYRings - 1;
					bool bEvenRung = i % 2 == 0;
					bool bEvenHex = SphereZRing % 2 != 0;
					bool bNegativeRung = i < 0;
					bool bOffsetHex = !bEvenHex;
					double OffsetAmount = bNegativeRung ? 0.5 : -0.5;

					double phiAlpha = ((i - (!bMiddleRung ? 0.0 : bOffsetHex ? 0.0 : OffsetAmount)) / SphereXYRings + 1.0) / 2.0;
					double phi = FMath::Lerp(0.0, PI, phiAlpha);
					double thetaAlpha = ((SphereZRing + 0.0) / SphereRingMax);
					double theta = FMath::Lerp(0.0, PI * 2.0, thetaAlpha);

					double x = FMath::Cos(theta) * FMath::Sin(phi);
					double y = FMath::Sin(theta) * FMath::Sin(phi);
					double z = FMath::Cos(phi);

					if (GetWorld()) {
						FVector VoxelNormal = FVector(x, y, z).GetClampedToSize(1.0, 1.0);
						FTransform VoxelTransform = FTransform(GetActorLocation() + VoxelNormal * HexGridSize);
						if (AHexGrid* NewVoxel = GetWorld()->SpawnActorDeferred<AHexGrid>(VoxelClass, VoxelTransform)) {
							// UE_LOG(LogTemp, Warning, TEXT("Adding Voxel Actor: %s"), *VoxelSpawnLocation.ToString());
							Chunks.Add(NewVoxel);
							ChunkCoords.Add(VoxelIndex);
							NewVoxel->SetRandomSeed(RandomSeed);
							NewVoxel->SetVoxelSize(VoxelSize);
							NewVoxel->SetChunkZElements(ChunkZElements);
							NewVoxel->SetChunkLineElements(ChunkLineElements);
							NewVoxel->SetChunkIndex(VoxelIndex);
							NewVoxel->SetCurrentLOD(0);
							NewVoxel->NormalVector = VoxelNormal;
							// NewVoxel->bFlatTop = i % 2 != 0; // Switch the orientation every other one
							// NewVoxel->SetSphere(bSphere);
							// NewVoxel->GetRootComponent()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
							// NewVoxel->GetRootComponent()->RegisterComponent();
							// NewVoxel->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
							// Register
							UGameplayStatics::FinishSpawningActor(NewVoxel, VoxelTransform);
							NewVoxel->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
						}
					}
				}
			}
		} // */

		/* for (a = -N; a <= N; a++) {
			double sAlpha = (a / N + 1) / 2.0;
			double s = FMath::Lerp(0.0, PI * 2, sAlpha);
			int32 b1 = FMath::Max(-N, -a - N);
			int32 b2 = FMath::Min(N, -a + N);
			for (b = b1; b <= b2; b++) {
				double tAlpha = (b / N + 1) / 2.0;
				double t = FMath::Lerp(0.0, PI, tAlpha);

				c = -a - b;

				double x = cos(s) * sin(t) * ChunkLineElements * VoxelSize * ChunkLineElements;
				double y = sin(s) * sin(t) * ChunkLineElements * VoxelSize * ChunkLineElements;
				double z = cos(t) * ChunkLineElements * VoxelSize * ChunkLineElements;

				FVector FinalCoord = FVector(x, y, z);
				VoxelIndex = FIntVector(a, b, c);
				if (GetWorld())
				{
					if (AHexGrid* NewVoxel = GetWorld()->SpawnActorDeferred<AHexGrid>(VoxelClass, FTransform(GetActorLocation() + FinalCoord)))
					{
						// UE_LOG(LogTemp, Warning, TEXT("Adding Voxel Actor: %s"), *VoxelSpawnLocation.ToString());
						Chunks.Add(NewVoxel);
						ChunkCoords.Add(VoxelIndex);
						NewVoxel->SetRandomSeed(RandomSeed);
						NewVoxel->SetVoxelSize(VoxelSize);
						NewVoxel->SetChunkZElements(ChunkZElements);
						NewVoxel->SetChunkLineElements(ChunkLineElements);
						NewVoxel->SetChunkIndex(VoxelIndex);
						NewVoxel->SetCurrentLOD(0);
						NewVoxel->NormalVector = FinalCoord.GetUnsafeNormal();
						// NewVoxel->SetSphere(bSphere);
						// NewVoxel->GetRootComponent()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
						// NewVoxel->GetRootComponent()->RegisterComponent();
						// NewVoxel->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
						// Register
						UGameplayStatics::FinishSpawningActor(NewVoxel, FTransform(GetActorLocation() + FinalCoord));
						NewVoxel->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
					}
				}
			}
		} // */
	}
}

AGameplayVoxel* AHexGridManager::AddVoxelActor(FVector VoxelSpawnLocation, FIntVector VoxelIndex, int32 CurrentLOD)
{
	if (GetWorld())
	{
		FTransform VoxelTransform = FTransform(VoxelSpawnLocation);
		if (AHexGrid* NewVoxel = GetWorld()->SpawnActorDeferred<AHexGrid>(VoxelClass, VoxelTransform))
		{
			// UE_LOG(LogTemp, Warning, TEXT("Adding Voxel Actor: %s"), *VoxelSpawnLocation.ToString());
			Chunks.Add(NewVoxel);
			ChunkCoords.Add(VoxelIndex);
			NewVoxel->SetRandomSeed(RandomSeed);
			NewVoxel->SetVoxelSize(VoxelSize);
			NewVoxel->SetChunkZElements(ChunkZElements);
			NewVoxel->SetChunkLineElements(ChunkLineElements);
			NewVoxel->SetChunkIndex(VoxelIndex);
			NewVoxel->SetCurrentLOD(CurrentLOD);
			NewVoxel->NormalVector = FVector::UpVector;
			UGameplayStatics::FinishSpawningActor(NewVoxel, VoxelTransform);
			NewVoxel->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));

			return NewVoxel;
		}
	}

	return nullptr;
}

AHexGrid* AHexGridManager::GetGridAtIndex(int32 InIndex)
{
	int32 GridIndex = 0;
	int32 Division = 0;
	int32 Remainder = InIndex;

	float ChunkLineElementsP3 = FMath::Pow(ChunkLineElements,3.f);
	if (Remainder > ChunkLineElementsP3)
		Division = Remainder / ChunkLineElementsP3;

	if (!Chunks.IsValidIndex(Division)) return nullptr;
	return Cast<AHexGrid>(Chunks[Division]);
}
