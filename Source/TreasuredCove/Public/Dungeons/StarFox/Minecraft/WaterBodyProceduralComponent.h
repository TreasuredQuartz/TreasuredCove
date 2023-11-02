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
	EWaterBodyType GetWaterBodyType() const override { return EWaterBodyType::Transition; };
	TArray<UPrimitiveComponent*> GetCollisionComponents(bool blah) const override;
	TArray<UPrimitiveComponent*> GetStandardRenderableComponents() const override;

protected:
	/** AWaterBody Interface */
	void Reset() override;
	void BeginUpdateWaterBody() override;
	void OnUpdateBody(bool bWithExclusionVolumes) override;

#if WITH_EDITOR
	virtual TArray<TSharedRef<FTokenizedMessage>> CheckWaterBodyStatus() override;
#endif // WITH_EDITOR

protected:
	UPROPERTY(NonPIEDuplicateTransient)
	UProceduralMeshComponent* MeshComp;
};
