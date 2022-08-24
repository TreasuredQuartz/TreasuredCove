// Fill out your copyright notice in the Description page of Project Settings.

#include "ProceduralPlanet.h"
#include "PlanetLandscape.h"
#include "PlanetShapeSettings.h"
#include "PlanetShapeGenerator.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"
#include "LevelEditorViewport.h"


// Sets default values
AProceduralPlanet::AProceduralPlanet() :
	bIsValid(false),
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

// Allows Tick in Editor
bool AProceduralPlanet::ShouldTickIfViewportsOnly() const
{
	return true;
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
	Super::Tick(DeltaTime);

	checkCurrentView();
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
				playerLocation = UKismetMathLibrary::ComposeTransforms(GetActorTransform(), FTransform(FRotator::ZeroRotator, Client->GetViewLocation(), FVector::OneVector)).GetLocation();
			}
		}
	}
#endif

	ShapeGenerator.UpdateSettings(Settings.ShapeSettings);
	MaterialGenerator.UpdateSettings(Settings.MaterialSettings);

	if (Meshs.Num() <= 0) Meshs.SetNum(6);
	Landscapes.SetNum(6);

	for (int i = 0; i < 6; ++i)
	{
		if (Meshs[i] == nullptr)
		{
			UProceduralMeshComponent* meshComp = 
				NewObject<UProceduralMeshComponent>(this, FName("Landscape_PMesh_%s" + FString::FromInt(i)));
			FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);

			meshComp->SetupAttachment(RootComponent);
			meshComp->RegisterComponent();
			Meshs[i] = meshComp;
		}

		if (Landscapes[i] == nullptr)
			Landscapes[i] = NewObject<UPlanetLandscape>(this, FName("Landscape_Object_%s" + FString::FromInt(i)));

		Landscapes[i]->Initialize(ShapeGenerator, Settings.ShapeSettings.MeshSettings, 
			*Meshs[i], Directions[i]);
	}
}

void AProceduralPlanet::GenerateMesh()
{
	for (UPlanetLandscape* Landscape : Landscapes)
	{
		if (Landscape != nullptr)
			Landscape->ConstructMesh(playerLocation);
	}

	GenerateHeight();

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
	for (UProceduralMeshComponent* Mesh : Meshs)
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
	for (auto* Landscape : Landscapes)
	{
		if (Landscape != nullptr) Landscape->OnCameraLocationUpdated(playerLocation);
	}
}

void AProceduralPlanet::checkCurrentView()
{
#if WITH_EDITOR
	if (GEditor)
	{
		if (const auto& Viewport = GEditor->GetActiveViewport())
		{
			if (auto Client = (FLevelEditorViewportClient*)(Viewport->GetClient()))
			{
				const FVector Loc =
					UKismetMathLibrary::ConvertTransformToRelative(FTransform(FRotator::ZeroRotator, Client->GetViewLocation(), FVector::OneVector), GetActorTransform()).GetLocation();
				const FRotator Rot = Client->GetViewRotation();

				if (playerLocation != Loc)
				{
					// UE_LOG(LogTemp, Warning, TEXT("UPlanet: \n -Camera location Changed! \n --%s"), *playerLocation.ToString());
					playerLocation = Loc;
					CheckLOD();
				}
			}
		}
	}
#else
	LODCamera->SetWorldLocation(UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation());
	LODCamera->SetWorldRotation(UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraRotation());
#endif
	
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
