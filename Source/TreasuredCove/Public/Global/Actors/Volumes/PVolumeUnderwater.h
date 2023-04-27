// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PhysicsVolume.h"
#include "PVolumeUnderwater.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API APVolumeUnderwater : public APhysicsVolume, public IInterface_PostProcessVolume
{
	GENERATED_BODY()
	
public:
	// Default constructor for setting default values and subobjects.
	APVolumeUnderwater(const FObjectInitializer& ObjectInitializer);
	
public:
	/** Post process settings to use for this volume. */
	UPROPERTY(interp, Category = PostProcessVolumeSettings, meta = (ShowPostProcessCategories))
	struct FPostProcessSettings Settings;

	/**
	 * Priority of this volume. In the case of overlapping volumes the one with the highest priority
	 * overrides the lower priority ones. The order is undefined if two or more overlapping volumes have the same priority.
	 */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PostProcessVolumeSettings)
	// float Priority; // Already defined in parent APhysicsActor

	/** World space radius around the volume that is used for blending (only if not unbound).			*/
	UPROPERTY(interp, Category = PostProcessVolumeSettings, meta = (ClampMin = "0.0", UIMin = "0.0", UIMax = "6000.0"))
	float BlendRadius;

	/** 0:no effect, 1:full effect */
	UPROPERTY(interp, Category = PostProcessVolumeSettings, BlueprintReadWrite, meta = (UIMin = "0.0", UIMax = "1.0"))
	float BlendWeight;

	/** Whether this volume is enabled or not.															*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PostProcessVolumeSettings)
	uint32 bEnabled : 1;

	/** Whether this volume covers the whole world, or just the area inside its bounds.								*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PostProcessVolumeSettings, meta = (DisplayName = "Infinite Extent (Unbound)"))
	uint32 bUnbound : 1;

	//~ Begin IInterface_PostProcessVolume Interface
	virtual bool EncompassesPoint(FVector Point, float SphereRadius/*=0.f*/, float* OutDistanceToPoint) override;
	virtual FPostProcessVolumeProperties GetProperties() const override
	{
		FPostProcessVolumeProperties Ret;
		Ret.bIsEnabled = IsPPVEnabled();
		Ret.bIsUnbound = bUnbound != 0;
		Ret.BlendRadius = BlendRadius;
		Ret.BlendWeight = BlendWeight;
		Ret.Priority = Priority;
		Ret.Settings = &Settings;
		return Ret;
	}
#if DEBUG_POST_PROCESS_VOLUME_ENABLE
	virtual FString GetDebugName() const override
	{
		return GetName();
	}
#endif
	//~ End IInterface_PostProcessVolume Interface

	/** Adds an Blendable (implements IBlendableInterface) to the array of Blendables (if it doesn't exist) and update the weight */
	UFUNCTION(BlueprintCallable, Category = "Rendering")
	void AddOrUpdateBlendable(TScriptInterface<IBlendableInterface> InBlendableObject, float InWeight = 1.0f) { Settings.AddBlendable(InBlendableObject, InWeight); }

	//~ Begin AActor Interface
	virtual void PostUnregisterAllComponents(void) override;
#if WITH_EDITOR
	virtual bool CanChangeIsSpatiallyLoadedFlag() const override { return !bUnbound; }
#endif
protected:
	virtual void PostRegisterAllComponents() override;
	//~ End AActor Interface

	bool IsPPVEnabled() const;
public:

	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif // WITH_EDITOR
	virtual void Serialize(FArchive& Ar);
	//~ End UObject Interface

	//~ Begin APhysicsVolume Interface
	// Called when actor enters a volume
	virtual void ActorEnteredVolume(class AActor* Other);

	// Called when actor leaves a volume, Other can be NULL
	virtual void ActorLeavingVolume(class AActor* Other);
	//~ End APhysicsVolume Interface

	//~ Begin PVolumeUnderwater Interface
public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnActorEnteredVolume(class AActor* Other);

	UFUNCTION(BlueprintImplementableEvent)
	void OnActorLeavingVolume(class AActor* Other);
	//~ End PVolumeUnderwater Interface
};
