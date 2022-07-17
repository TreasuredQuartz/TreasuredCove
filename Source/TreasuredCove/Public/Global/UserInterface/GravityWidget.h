// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FloatingTextActor.h"
#include "GravityWidget.generated.h"

class UGAWidget;

UCLASS()
class TREASUREDCOVE_API AGravityWidget : public AFloatingTextActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGravityWidget();

	float Mass;
	float AirResistance;

	FVector Gravity;
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* Widget;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
#if WITH_EDITOR
	virtual void PostInitializeComponents() override;
#endif

	void AddForce(FVector Force);
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class URotatingMovementComponent* RotatingMovement;

	FVector StoredForce;
	FVector TargetLocation;
	FVector ConsumeMovement();
};
