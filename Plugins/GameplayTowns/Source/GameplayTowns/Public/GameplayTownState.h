// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTownState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTownSpeedModified, float, TownSpeed);

UCLASS()
class GAMEPLAYTOWNS_API AGameplayTownState : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameplayTownState();

private:
	class AGameplayTown* Town;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay Town")
	class UGameplayClockComponent* ClockComponent;
	TMap<FName, float> Resources;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetTown(AGameplayTown* Town);

	// Town we represent
	AGameplayTown* GetTown() const { return Town; };
};
