// Copyright Epic Games, Inc. All Rights Reserved.

#include "WaterBodyProceduralActor.h"

#include "WaterBodyProceduralComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

#if WITH_EDITOR
#include "WaterIconHelper.h"
#endif

// ----------------------------------------------------------------------------------

AWaterBodyProcedural::AWaterBodyProcedural(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	WaterBodyType = EWaterBodyType::Transition;

#if WITH_EDITOR
	ActorIcon = FWaterIconHelper::EnsureSpriteComponentCreated(this, TEXT("/Water/Icons/WaterBodyCustomSprite"));
#endif

#if WITH_EDITORONLY_DATA
	bAffectsLandscape_DEPRECATED = false;
#endif
}

void AWaterBodyProcedural::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITORONLY_DATA
	if (GetLinkerCustomVersion(FFortniteMainBranchObjectVersion::GUID) < FFortniteMainBranchObjectVersion::WaterBodyComponentRefactor)
	{
		UWaterBodyProceduralComponent* CustomComponent = CastChecked<UWaterBodyProceduralComponent>(WaterBodyComponent);
		if (CustomGenerator_DEPRECATED)
		{
			CustomComponent->MeshComp = CustomGenerator_DEPRECATED->MeshComp;
		}
	}
#endif // WITH_EDITORONLY_DATA
}

#if WITH_EDITOR
bool AWaterBodyProcedural::IsIconVisible() const
{
	return (WaterBodyComponent->GetWaterMeshOverride() == nullptr);
}
#endif // WITH_EDITOR

// ----------------------------------------------------------------------------------

UDEPRECATED_ProceduralMeshGenerator::UDEPRECATED_ProceduralMeshGenerator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}