// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UShooterComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	float Control;
	TQueue<FVector2D> SpreadPattern;
	FVector2D CurrentRecoil;

public:	
	// Sets default values for this component's properties
	UShooterComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//
	UFUNCTION(BlueprintCallable, Category = "Shooter")
	void FireAtTarget(AActor* TargetActor, float Accuracy);

private:
	float GetCurrentVarience() const;
	FVector2D GetRandomVarience(float Varience) const;
	FVector GetVectorWithVarience(const FVector& Normal, const FVector2D& Varience) const;
	// float GetDiscipline() const;
	FVector2D GetBulletVarience();

	FVector GetFinalLocation(const FVector& TargetLocation);
};
