// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerAssetEditor/FPreviewScene_MapMaker.h"
#include "Components/DirectionalLightComponent.h"

FPreviewScene_MapMaker::FPreviewScene_MapMaker(ConstructionValues CVS)
{
	// world setting
	GetWorld()->GetWorldSettings()->NotifyBeginPlay();
	GetWorld()->GetWorldSettings()->NotifyMatchStarted();
	GetWorld()->GetWorldSettings()->SetActorHiddenInGame(false);
	GetWorld()->bBegunPlay = true;

	// set light options 
	DirectionalLight->SetRelativeLocation(FVector(-1024.f, 1024.f, 2048.f));
	DirectionalLight->SetRelativeScale3D(FVector(15.f));
	SetLightBrightness(4.f);
	DirectionalLight->InvalidateLightingCache();
	DirectionalLight->RecreateRenderState_Concurrent();

	// creae a sky sphere
	UStaticMeshComponent* SkyComp = NewObject<UStaticMeshComponent>();
	UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(NULL, TEXT("/Engine/MapTemplates/Sky/SM_SkySphere.SM_SkySphere"), NULL, LOAD_None, NULL);
	SkyComp->SetStaticMesh(StaticMesh);
	UMaterial* SkyMaterial = LoadObject<UMaterial>(NULL, TEXT("/Engine/EditorMaterials/PersonaSky.PersonaSky"), NULL, LOAD_None, NULL);
	SkyComp->SetMaterial(0, SkyMaterial);
	const float SkySphereScale = 1000.f;
	const FTransform SkyTransform(FRotator(0, 0, 0), FVector(0, 0, 0), FVector(SkySphereScale));
	AddComponent(SkyComp, SkyTransform);

	// now add floor
	UStaticMesh* FloorMesh = LoadObject<UStaticMesh>(NULL, TEXT("/Engine/EditorMeshes/EditorCube.EditorCube"), NULL, LOAD_None, NULL);
	UStaticMeshComponent* FloorComp = NewObject<UStaticMeshComponent>();
	FloorComp->SetStaticMesh(FloorMesh);
	AddComponent(FloorComp, FTransform::Identity);
	FloorComp->SetRelativeScale3D(FVector(3.f, 3.f, 1.f));
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Engine/EditorMaterials/PersonaFloorMat.PersonaFloorMat"), NULL, LOAD_None, NULL);
	FloorComp->SetMaterial(0, Material);
	FloorComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	FloorComp->SetCollisionObjectType(ECC_WorldStatic);
}
