// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectilePathComponent.generated.h"

USTRUCT(BlueprintType)
struct FProjectilePathParams
{
	GENERATED_BODY()

public:
	FVector LaunchVelocity;

};

class AProjectilePathActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UProjectilePathComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectilePath")
	TSubclassOf<AProjectilePathActor> PathActorClass;
private:
	AProjectilePathActor* PathActor;

public:	
	// Sets default values for this component's properties
	UProjectilePathComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "ProjectilePath")
	void BeginPrediction_Client(const FVector& StartLocation, const FVector& LaunchVelocity);
	void StopPrediction();
	void UpdatePrediction(const FVector& InStartLocation, const FVector& LaunchVelocity);
	void OnPathDestroyed(AActor* DestroyedActor);

};
