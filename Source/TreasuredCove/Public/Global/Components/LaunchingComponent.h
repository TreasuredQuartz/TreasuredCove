// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LaunchingComponent.generated.h"

enum class EProjectilePathIndicatorType : uint8
{
	NumBounces,
	DistanceTraveled
};

UENUM()
enum class ELaunchType : uint8
{
	Instant,
	BuildUp
};

class UProjectilePathComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API ULaunchingComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region Members
private:
	ELaunchType LaunchType;

	bool bLaunching;
	bool bShowProjectilePath;
	bool bShowProjectileAreaOfEffect;

	float Speed;
	float Acceleration;
	float MaxVelocity;
	float ProjectilePathIndicatorDistance;

	FVector Location;
	FVector Offset;
	FVector Direction;

	AActor* AttachedActor;

#pragma endregion

#pragma region Subcomponents
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UProjectilePathComponent* ProjectilePath;

#pragma endregion

#pragma region ActorComponentInterface
public:	
	// Sets default values for this component's properties
	ULaunchingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion
		
#pragma region DefaultFunctions
public:
	// Called to begin ticking for launch arc and landing location
	UFUNCTION(BlueprintCallable, Category = "Launching Component")
	void BeginLaunch(const FVector& InLocation, const FVector& InDirection, float InAcceleration, ELaunchType InLaunchType);

	// Called to begin ticking for launch arc and landing location
	// Overload for attaching to an actor
	UFUNCTION(BlueprintCallable, Category = "Launching Component")
	void BeginLaunch_Attached(AActor* InAttachActor, const FVector& OffsetLocation, float InAcceleration, ELaunchType InLaunchType);

	// Called to end launch. Returns current velocity
	UFUNCTION(BlueprintCallable, Category = "Launching Component")
	void EndLaunch(FVector& LaunchVelocity);

	// Called to reset all launch variables. Used after EndLaunch
	void ResetLaunch();

	// Called to change launch from single LaunchVelocity to building up LaunchVelocity.
	void SetLaunchType(ELaunchType InLaunchType);
#pragma endregion
};
