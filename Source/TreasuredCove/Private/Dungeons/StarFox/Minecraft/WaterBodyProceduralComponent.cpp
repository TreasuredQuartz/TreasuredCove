// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeons/StarFox/Minecraft/WaterBodyProceduralComponent.h"
#include "ProceduralMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "WaterSubsystem.h"
#include "Misc/UObjectToken.h"
#include "MeshExtensionLibrary.h"

#if WITH_EDITOR
#include "WaterIconHelper.h"
#endif

#define LOCTEXT_NAMESPACE "Water"

// ----------------------------------------------------------------------------------

UWaterBodyProceduralComponent::UWaterBodyProceduralComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAffectsLandscape = false;

	// @todo_water : Remove these checks (Once AWaterBody is no more Blueprintable, these methods should become PURE_VIRTUAL and this class should overload them)
	check(!IsFlatSurface());
	check(!IsWaterSplineClosedLoop());
	check(!IsHeightOffsetSupported());
}

TArray<UPrimitiveComponent*> UWaterBodyProceduralComponent::GetCollisionComponents(bool blah) const
{
	TArray<UPrimitiveComponent*> Result;
	if ((MeshComp != nullptr) && (MeshComp->GetCollisionEnabled() != ECollisionEnabled::NoCollision))
	{
		Result.Add(MeshComp);
	}
	return Result;
}

TArray<UPrimitiveComponent*> UWaterBodyProceduralComponent::GetStandardRenderableComponents() const
{
	TArray<UPrimitiveComponent*> Result;
	if (MeshComp != nullptr)
	{
		Result.Add(MeshComp);
	}
	return Result;
}

void UWaterBodyProceduralComponent::Reset()
{
	AActor* Owner = GetOwner();
	check(Owner);

	TArray<UStaticMeshComponent*> MeshComponents;
	Owner->GetComponents(MeshComponents);

	MeshComp = nullptr;
	for (UStaticMeshComponent* MeshComponent : MeshComponents)
	{
		MeshComponent->DestroyComponent();
	}
}

void UWaterBodyProceduralComponent::OnUpdateBody(bool bWithExclusionVolumes)
{
	AActor* OwnerActor = GetOwner();
	check(OwnerActor);

	if (!MeshComp)
	{
		MeshComp = NewObject<UProceduralMeshComponent>(OwnerActor, TEXT("ProceduralMeshComponent"), RF_Transactional);
		MeshComp->SetNetAddressable(); // it's deterministically named so it's addressable over network (needed for collision)
		MeshComp->SetupAttachment(this);
		MeshComp->SetCollisionProfileName(GetCollisionProfileName());
		// In the case of custom meshes, the static mesh component acts as both collision and visual component so we simply disable collision on it: 
		// MeshComp->SetGenerateOverlapEvents(bGenerateCollisions);
		/*if (!bGenerateCollisions)
		{
			MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}*/
		MeshComp->RegisterComponent();
	}

	TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents;
	OwnerActor->GetComponents(PrimitiveComponents);

	// Make no assumptions for custom meshes.  Add all components with collision to the list of collision components
	for (UPrimitiveComponent* Comp : PrimitiveComponents)
	{
		//if (bGenerateCollisions && (Comp->GetCollisionEnabled() != ECollisionEnabled::NoCollision))
		//{
		//	// Use value of bFillCollisionUnderWaterBodiesForNavmesh for all components with collisions.
		//	Comp->bFillCollisionUnderneathForNavmesh = bFillCollisionUnderWaterBodiesForNavmesh;
		//}

		Comp->SetMobility(Mobility);
	}

	CreateOrUpdateWaterMID();
	UMeshExtensionLibrary::CopyProceduralMeshFromStaticMesh(GetWaterMeshOverride(), 0, MeshComp, false);
	MeshComp->SetMaterial(0, WaterMID);
	MeshComp->SetCastShadow(false);
	MeshComp->MarkRenderStateDirty();
}

void UWaterBodyProceduralComponent::BeginUpdateWaterBody()
{
	Super::BeginUpdateWaterBody();

	UMaterialInstanceDynamic* WaterMaterialInstance = GetWaterMaterialInstance();
	if (WaterMaterialInstance && MeshComp)
	{
		// We need to get(or create) the water MID at runtime and apply it to the static mesh component 
		// The MID is transient so it will not make it through serialization, apply it here (at runtime)
		MeshComp->SetMaterial(0, WaterMaterialInstance);
	}
}

#if WITH_EDITOR
TArray<TSharedRef<FTokenizedMessage>> UWaterBodyProceduralComponent::CheckWaterBodyStatus()
{
	TArray<TSharedRef<FTokenizedMessage>> StatusMessages = Super::CheckWaterBodyStatus();

	if (!IsTemplate())
	{
		if (WaterMeshOverride == nullptr)
		{
			StatusMessages.Add(FTokenizedMessage::Create(EMessageSeverity::Error)
				->AddToken(FUObjectToken::Create(this))
				->AddToken(FTextToken::Create(FText::Format(
					LOCTEXT("MapCheck_Message_MissingCustomWaterMesh", "Custom water body {0} requires a static mesh to be rendered. Please set WaterMeshOverride to a valid static mesh. "),
					FText::FromString(GetWaterBodyActor()->GetActorLabel())))));
		}
	}

	return StatusMessages;
}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE

