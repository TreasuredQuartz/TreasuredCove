// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AimOffsetComponent.generated.h"

class UTimelineComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAimOffsetUpdated, UAimOffsetComponent*, AimOffsetComponent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UAimOffsetComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	float Alpha;
	UPROPERTY(EditDefaultsOnly)
	float FOV;
	UPROPERTY(EditDefaultsOnly)
	float Vignette;
	UPROPERTY(EditDefaultsOnly)
	float Speed;

	UPROPERTY(EditDefaultsOnly)
	FVector AimOffset;

public:
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetAlpha() const { return Alpha; };
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetFOV() const { return FOV; };
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetVignette() const { return Vignette; };

	UFUNCTION(BlueprintPure, BlueprintCallable)
	FVector GetAimOffset() const { return AimOffset; };

public:	
	// Sets default values for this component's properties
	UAimOffsetComponent();

	// AimOffset Curve
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Run")
	UCurveFloat* AimOffsetTimelineCurve;

	// Timeline Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UTimelineComponent* AimOffsetTimeline;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Called to start timeline to change AimOffset Alpha
	UFUNCTION(BlueprintCallable)
	void StartAimOffset();

private:
	// Called internally to update AimOffset Alpha.
	// Call "StartAimOffset()" to start.
	UFUNCTION()
	void UpdateAimOffsetTimeline(float progress);

public:
	// Called to reverse timeline to change AimOffset Alpha
	UFUNCTION(BlueprintCallable)
	void EndAimOffset();

public:
	UPROPERTY(BlueprintAssignable)
	FOnAimOffsetUpdated OnAimOffsetUpdated;
};
