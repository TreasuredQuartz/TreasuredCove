// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WaterBodyActor.h"
#include "WaterBodyProceduralActor.generated.h"

class UDEPRECATED_ProceduralMeshGenerator;
class UProceduralMeshComponent;

UCLASS(MinimalAPI)
class UDEPRECATED_ProceduralMeshGenerator : public UDEPRECATED_WaterBodyGenerator
{
	GENERATED_BODY()

	UDEPRECATED_ProceduralMeshGenerator(const FObjectInitializer& ObjectInitializer);
public:
	UPROPERTY(NonPIEDuplicateTransient)
	UProceduralMeshComponent* MeshComp;
};

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API AWaterBodyProcedural : public AWaterBody
{
	GENERATED_BODY()

	AWaterBodyProcedural(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual bool IsIconVisible() const override;
#endif

#if WITH_EDITORONLY_DATA
	UPROPERTY(NonPIEDuplicateTransient)
	UDEPRECATED_ProceduralMeshGenerator* CustomGenerator_DEPRECATED;
#endif
};
