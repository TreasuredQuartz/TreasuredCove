// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WaterBodyComponent.h"
#include "WaterBodyProceduralComponent.generated.h"

class UProceduralMeshComponent;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UWaterBodyProceduralComponent : public UWaterBodyComponent
{
	GENERATED_BODY()

	UWaterBodyProceduralComponent(const FObjectInitializer& ObjectInitializer);
	friend class AWaterBodyProcedural;
public:
	/** AWaterBody Interface */
	virtual EWaterBodyType GetWaterBodyType() const override { return EWaterBodyType::Transition; }
	virtual TArray<UPrimitiveComponent*> GetCollisionComponents() const override;
	virtual TArray<UPrimitiveComponent*> GetStandardRenderableComponents() const override;

protected:
	/** AWaterBody Interface */
	virtual void Reset() override;
	virtual void BeginUpdateWaterBody() override;
	virtual void OnUpdateBody(bool bWithExclusionVolumes) override;

#if WITH_EDITOR
	virtual TArray<TSharedRef<FTokenizedMessage>> CheckWaterBodyStatus() const override;
#endif // WITH_EDITOR

protected:
	UPROPERTY(NonPIEDuplicateTransient)
	UProceduralMeshComponent* MeshComp;
};
