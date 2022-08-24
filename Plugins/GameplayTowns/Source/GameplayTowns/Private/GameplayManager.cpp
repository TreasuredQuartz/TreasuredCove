// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayManager.h"
#include "GameplayVoxel.h"

#include "Components/BoxComponent.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

#include "TimerManager.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AGameplayManager::AGameplayManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PlayArea =
		CreateDefaultSubobject<UBoxComponent>(TEXT("PlayArea"));
	PlayArea->OnComponentBeginOverlap.AddDynamic(this, &AGameplayManager::EnterPlayArea);
	PlayArea->OnComponentEndOverlap.AddDynamic(this, &AGameplayManager::ExitPlayArea);

	USceneComponent* Root =
		CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	RootComponent = Root;
	PlayArea->SetupAttachment(RootComponent);

	RenderRange = 6;
	VoxelSize = 50;
	ChunkLineElements = 10;
	ChunkZElements = 10;
	VoxelClass = AGameplayVoxel::StaticClass();
}

void AGameplayManager::OnConstruction_DoOnce()
{
	if (!bConstructionDoOnce)
	{
		bConstructionDoOnce = true;
		// UE_LOG(LogTemp, Warning, TEXT("Construction script..."));

		ChunkSize = ChunkLineElements * VoxelSize;
		ChunkSizeHalf = ChunkSize * 0.5;
		int32 RRCubed = RenderRange * RenderRange * RenderRange;

		Chunks.Reserve(RRCubed);
		ChunkCoords.Reserve(RRCubed);

		if (bIsActive)
		{
			RemoveChunk();
			AddChunk();
		}
	}
}

// Called in editor and on begin play
void AGameplayManager::OnConstruction(const FTransform& Transform)
{
	ActiveChunkCoords = FIntVector(Transform.GetLocation() / ChunkSize);
	OnConstruction_DoOnce();
	

	Super::OnConstruction(Transform);
}

// Called in editor when a uproperty changes
void AGameplayManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : FName();

	if (
		(PropertyName == GET_MEMBER_NAME_CHECKED(AGameplayManager, RenderRange)) ||
		(PropertyName == GET_MEMBER_NAME_CHECKED(AGameplayManager, ChunkLineElements)) || 
		(PropertyName == GET_MEMBER_NAME_CHECKED(AGameplayManager, ChunkZElements)) || 
		(PropertyName == GET_MEMBER_NAME_CHECKED(AGameplayManager, VoxelSize)) || 
		(PropertyName == GET_MEMBER_NAME_CHECKED(AGameplayManager, RandomSeed))
		)
	{
		ChunkSize = ChunkLineElements * VoxelSize;
		if (ChunkCoords.Num() > 0 || Chunks.Num() > 0)
		{
			ClearChunks();
		}
	}

	if (bIsActive)
	{
		RemoveChunk();
		AddChunk();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

// Called when the game starts or when spawned
void AGameplayManager::BeginPlay()
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0); 
	
	FTimerDelegate PlayerCoordDel;
	PlayerCoordDel.BindUFunction(this, FName("UpdatePlayerCoordinate"));
	FTimerHandle PlayerCoordHandle;
	GetWorldTimerManager().SetTimer(PlayerCoordHandle, PlayerCoordDel, 1.0f, true);

	// AddChunk();

	Super::BeginPlay();
}

void AGameplayManager::AddVoxelActor(FVector VoxelSpawnLocation, FIntVector VoxelIndex, int32 CurrentLOD)
{
	if (GetWorld())
	{
		if (AGameplayVoxel* NewVoxel = GetWorld()->SpawnActorDeferred<AGameplayVoxel>(VoxelClass, FTransform(VoxelSpawnLocation)))
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
			NewVoxel->GetRootComponent()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
			// NewVoxel->GetRootComponent()->RegisterComponent();
			// NewVoxel->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
			// Register
			UGameplayStatics::FinishSpawningActor(NewVoxel, FTransform(VoxelSpawnLocation));
		}
	}
}

static int32 LODDistances[4] = {
	1,
	3,
	5,
	6,
};

void AGameplayManager::AddChunk()
{
	// UE_LOG(LogTemp, Warning, TEXT("Adding Chunk!"));

	// Z
	int32 z = 0;
	int32 LocZ = z + ActiveChunkCoords.Z;
	int32 ZScaled = LocZ * ChunkSize;
	int32 ZCenter = ZScaled + ChunkSizeHalf;

	// Y
	for (int32 y = -RenderRange; y <= RenderRange; ++y)
	{
		int32 LocY = y + ActiveChunkCoords.Y;
		int32 YScaled = LocY * ChunkSize;
		int32 YCenter = YScaled + ChunkSizeHalf;

		// X
		for (int32 x = -RenderRange; x <= RenderRange; ++x)
		{
			int32 LocX = x + ActiveChunkCoords.X;
			int32 XScaled = LocX * ChunkSize;
			int32 XCenter = XScaled + ChunkSizeHalf;

			FIntVector LocVector = FIntVector(LocX, LocY, LocZ);

			// if chunk is outside play area, don't spawn
			//if (!InPlayArea(XCenter, YCenter)) {
			//	continue;
			//}

			// if Chunk is in player radius space...
			if (CHECK_RADIUS(XCenter, YCenter, ZCenter))
			{
				// And if Chunk does not exist in coords...
				if (!ChunkCoords.Contains(LocVector))
				{
					FVector Location = FVector(XCenter, YCenter, ZCenter);

					// UE_LOG(LogTemp, Warning, TEXT("Should Add Voxel Actor!"));
					int32 Distance = FMath::Abs(x) + FMath::Abs(y);
					int32 LOD = 0;
					if (Distance <= 1) LOD = 1;
					else if (Distance <= 3) LOD = 2;
					else LOD = 4;
					AddVoxelActor(Location, LocVector, 1);
				}
				else {
					GEngine->AddOnScreenDebugMessage(3, 10.f, FColor::Red, "Chunk updated!");
					Chunks[ChunkCoords.IndexOfByKey(LocVector)]->UpdateChunk(LocVector);
				}
			}
		}
	}
}

void AGameplayManager::RemoveChunk()
{
	for (int32 i = 0; i < ChunkCoords.Num(); ++i)
	{
		const FIntVector Coord = FIntVector( 
			((ChunkSize * ChunkCoords[i].X) + ChunkSizeHalf), 
			((ChunkSize * ChunkCoords[i].Y) + ChunkSizeHalf), 
			((ChunkSize * ChunkCoords[i].Z) + ChunkSizeHalf) 
		);
		bool bLocBool = CHECK_RADIUS(Coord.X, Coord.Y, Coord.Z);

		if (!bLocBool)
		{
			Chunks[i]->Destroy();
			ChunkCoords.RemoveAtSwap(i);
			Chunks.RemoveAtSwap(i);
		}
	}
}

void AGameplayManager::ClearChunks()
{
	for (int32 i = 0; i < ChunkCoords.Num(); ++i)
	{
		Chunks[i]->Destroy();
	}
	Chunks.Empty();
	ChunkCoords.Empty();
}

void AGameplayManager::AddUser(APawn* InPawn)
{
	APlayerState* PS = InPawn->GetPlayerState();

	if (PS)
	{
		UserArray.Add(PS);
	}
}

void AGameplayManager::AddUser(APlayerState* InPlayerState)
{
	UserArray.Add(InPlayerState);
}

void AGameplayManager::UpdatePlayerCoordinate()
{
	if (ChunkSize == 0) 
	{
		// UE_LOG(LogTemp, Warning, TEXT("ChunkSize is 0?"));
		return;
	}

	FVector Location = GetActorLocation();

	if (bUsePlayerPosition)
		if(APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0)) 
			Location = Pawn->GetActorLocation();

	PlayerCoords = FIntVector(Location);
	FIntVector PlayerChunkCoords = PlayerCoords / ChunkSize;

	if (ActiveChunkCoords != PlayerChunkCoords)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Updating Player Coordinate!"));

		if(GetActiveVoxel()) GetActiveVoxel()->ExitVoxel();
		ActiveChunkCoords = PlayerChunkCoords;
		if(GetActiveVoxel()) GetActiveVoxel()->EnterVoxel();

		AddChunk();
		RemoveChunk();
	}
}

AGameplayVoxel* AGameplayManager::GetActiveVoxel()
{
	AGameplayVoxel* Chunk = nullptr;
	int32 index = ChunkCoords.Find(ActiveChunkCoords);

	if(index >= 0 && 
		index < Chunks.Num() && 
		Chunks[index]) Chunk = Chunks[index];

	return Chunk;
}

bool AGameplayManager::InPlayArea(int32 X, int32 Y)
{
	return true;

	FVector BoxSize = PlayArea->GetComponentLocation() + PlayArea->GetScaledBoxExtent();
	return X < BoxSize.X && Y < BoxSize.Y;
}

void AGameplayManager::EnterPlayArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AGameplayManager::ExitPlayArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}


