// Fill out your copyright notice in the Description page of Project Settings.

#include "ProceduralPlanet.h"
#include "PlanetLandscape.h"
#include "Settings/PlanetShapeSettings.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Camera/CameraComponent.h"
#include "realtimeMeshComponent.h"

#include "Subsystems/UnrealEditorSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"
#include "LevelEditorViewport.h"


// Sets default values
AProceduralPlanet::AProceduralPlanet() :
	bIsValid(false),
	LODCheckPeriod(0.5f),
	TimeSinceLastLODCheck(999999.f),
	playerLocation(FVector::ZeroVector)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root =
		CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	LODCamera =
		CreateDefaultSubobject<UCameraComponent>(TEXT("LOD Test Camera"));

	SetRootComponent(Root);
	LODCamera->SetupAttachment(RootComponent);
	LODCamera->SetRelativeLocation(FVector(Settings.ShapeSettings.MeshSettings.radius, 0, 0));

	Settings = FPlanetSettings();
	ShapeGenerator = FPlanetShapeGenerator();
	MaterialGenerator = FPlanetMaterialGenerator();
}

#if WITH_EDITOR
void AProceduralPlanet::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	if (e.Property == NULL) return;
	FName MemberPropertyName = e.MemberProperty->GetFName();
	FName PropertyName = e.Property->GetFName();

	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Orange, PropertyName.ToString());
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Purple, MemberPropertyName.ToString());

	if (FPlanetNoiseSettings::isMemberName(PropertyName))
	{
		GenerateHeight();
	}
	else if (FPlanetMeshSettings::isMemberName(PropertyName))
	{
		GenerateMesh();
	}
	else if (FPlanetMaterialSettings::isMemberName(PropertyName))
	{
		Settings.MaterialSettings.Material = (UMaterialInstanceDynamic::Create(Settings.MaterialSettings.Material_Parent, this));
		GenerateMaterial();
	}

	Super::PostEditChangeProperty(e);
}

void AProceduralPlanet::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e)
{
	if (e.Property == NULL) return;
	FName MemberPropertyName = e.MemberProperty->GetFName();
	FName PropertyName = e.Property->GetFName();

	if (FPlanetShapeSettings::isMemberName(PropertyName))
	{
		GeneratePlanet();
	}

	Super::PostEditChangeChainProperty(e);
}

// This ultimately is what controls whether or not it can even tick at all in the editor view port. 
// But, it is EVERY view port so it still needs to be blocked from preview windows and junk.
bool AProceduralPlanet::ShouldTickIfViewportsOnly() const
{
	if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor /*&& UseEditorTick*/)
	{
		return true;
	}
	else
	{
		return false;
	}
}
#endif

// Called in editor and on begin play
void AProceduralPlanet::OnConstruction(const FTransform& Transform)
{
	OnValidate();
	
	Super::OnConstruction(Transform);
}

// Called when the game starts or when spawned
void AProceduralPlanet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProceduralPlanet::Tick(float DeltaTime)
{
#if WITH_EDITOR
	if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor)
	{
		EditorTick(DeltaTime);
	}
	else
#endif
	{
		checkCurrentView(DeltaTime);
		Super::Tick(DeltaTime);
	}
}

// Called every frame inside editor with a valid editor-type world. "Level Editor" for instance
void AProceduralPlanet::EditorTick(float DeltaTime)
{
	checkCurrentView(DeltaTime);
}

void AProceduralPlanet::OnValidate()
{
	if (!bIsValid)
	{
		GeneratePlanet();
		bIsValid = true;
	}
}

void AProceduralPlanet::GeneratePlanet()
{
	Initialize();
	GenerateMesh();
	GenerateHeight();
	GenerateMaterial();
}

static const TArray<FVector> Directions = {
		FVector::UpVector,
		FVector::DownVector,
		FVector::RightVector,
		FVector::LeftVector,
		FVector::ForwardVector,
		FVector::BackwardVector
};

void AProceduralPlanet::Initialize()
{
#if WITH_EDITOR
	if (GEditor)
	{
		if (const auto& Viewport = GEditor->GetActiveViewport())
		{
			if (auto Client = (FLevelEditorViewportClient*)(Viewport->GetClient()))
			{
				playerLocation = Client->GetViewLocation();
			}
		}
	}
#endif

	ShapeGenerator.UpdateSettings(Settings.ShapeSettings);
	MaterialGenerator.UpdateSettings(Settings.MaterialSettings);

	if (RuntimeMeshs.Num() <= 0) RuntimeMeshs.SetNum(6);
	Landscapes.SetNum(6);

	for (int i = 0; i < 6; ++i)
	{
		if (RuntimeMeshs[i] == nullptr)
		{
			URealtimeMeshComponent* meshComp =
				NewObject<URealtimeMeshComponent>(this, FName("Landscape_RMesh_%s" + FString::FromInt(i)));
			FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);

			meshComp->SetupAttachment(RootComponent);
			meshComp->RegisterComponent();
			RuntimeMeshs[i] = meshComp;
		}

		if (Landscapes[i] == nullptr) Landscapes[i] = NewObject<UPlanetLandscape>(this, FName("Landscape_Object_%s" + FString::FromInt(i)));

		Landscapes[i]->Initialize(ShapeGenerator, Settings.ShapeSettings.MeshSettings,
			*RuntimeMeshs[i], Directions[i]);
		Landscapes[i]->PlanetLocation = GetActorLocation();
	}
}

void AProceduralPlanet::GenerateMesh()
{
	for (UPlanetLandscape* Landscape : Landscapes)
	{
		if (Landscape != nullptr)
			Landscape->ConstructMesh(playerLocation);
	}

	// MaterialGenerator.UpdateElevation(ShapeGenerator.elevationMinMax);
	/*
	for (UPlanetWaterBody WaterBody : WaterBodies)
	{
		
	}
	*/
}

void AProceduralPlanet::GenerateHeight()
{
	for (UPlanetLandscape* Landscape : Landscapes)
	{
		if (Landscape != nullptr)
			Landscape->CalculateNoise();
	}
}

void AProceduralPlanet::GenerateMaterial()
{
	for (URealtimeMeshComponent* Mesh : RuntimeMeshs)
	{
		if (Mesh && Settings.MaterialSettings.Material_Parent)
		{
			Settings.MaterialSettings.UpdateMaterial(this);
			Mesh->SetMaterial(0, Settings.MaterialSettings.Material);
		}
	}
}

void AProceduralPlanet::CheckLOD()
{
	int32 i = 0;
	for (UPlanetLandscape* Landscape : Landscapes)
	{
		if (Landscape != nullptr)
		{
			Landscape->OnCameraLocationUpdated(playerLocation);
		}
		++i;
	}

	GenerateHeight();
	GenerateMaterial();
}

void AProceduralPlanet::checkCurrentView(float DeltaTime)
{
	TimeSinceLastLODCheck += DeltaTime;

	// Don't check every frame
	if (TimeSinceLastLODCheck < LODCheckPeriod)
		return;

	TimeSinceLastLODCheck = 0.f;

#if WITH_EDITOR
	if (GEditor)
	{
		if (true)
		{
			if (UUnrealEditorSubsystem* ESystem = GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>())
			{
				FVector Loc = FVector::ZeroVector;
				FRotator Rot = FRotator::ZeroRotator;

				if (!ESystem->GetLevelViewportCameraInfo(Loc, Rot))
				{
					// UE_LOG(LogTemp, Warning, TEXT("UPlanet: \n -GetLevelViewportCameraInfo failed! \n\n"));
				}
				else
				{
					// Int vector uses integer division, so remainder is lost.
					const FIntVector oldPlayerLocation = FIntVector(playerLocation) / 500; // Player cordinates in relative chunks
					const FIntVector newPlayerLocation = FIntVector(Loc) / 500;
					if (oldPlayerLocation != newPlayerLocation) // Only change corrdinates if player is 500m away from their previous location
					{
						playerLocation = Loc;
						// UE_LOG(LogTemp, Warning, TEXT("UPlanet: \n -Camera location Changed! \n --%s \n\n"), *playerLocation.ToString());
						CheckLOD();
					}
				}
			}

			return;
		}

		if (const auto& Viewport = GEditor->GetActiveViewport())
		{
			if (auto Client = (FLevelEditorViewportClient*)(Viewport->GetClient()))
			{
				const FVector Loc = Client->GetViewLocation();
				const FRotator Rot = Client->GetViewRotation();

				// Int vector uses integer division, so remainder is lost.
				// const FIntVector PlayerChunkCoords = FIntVector(Loc) / 500; // Player cordinates in relative chunks
				if (playerLocation != Loc) // Only change corrdinates if player is 500m away from their previous location
				{
					// UE_LOG(LogTemp, Warning, TEXT("UPlanet: \n -Camera location Changed! \n --%s \n\n"), *playerLocation.ToString());
					playerLocation = Loc;
					CheckLOD();
				}
			}
		}
	}
#else
#endif

	// Never moves
	/*auto world = GetWorld();
	if (world == nullptr)
		return;

	auto viewLocations = world->ViewLocationsRenderedLastFrame;
	if (viewLocations.Num() == 0)
		return;

	FVector playerLocation = viewLocations[0];
	CheckLOD();*/

	// LODCamera has been depreciated
	/*if (LODCamera)
	{
		const FVector Temp = LODCamera->GetComponentTransform().GetLocation();

		if (playerLocation != Temp)
		{
			UE_LOG(LogTemp, Warning, TEXT("Procedural Planet Tick: Camera Location Changed!"));
			playerLocation = Temp;
			GeneratePlanet();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Procedural Planet Tick: Camera Location Unchanged..."));
		}
	}*/
}
