// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponGeneratorComponent.generated.h"

class URandomGunGenerator;
// class URandomSwordGenerator;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UWeaponGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponGeneratorComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator")
	URandomGunGenerator* GunGenerator;

	// UPROPERTY(BlueprintReadWrite, Category = "Generator")
	// URandomSwordGenerator* SwordGenerator;

protected:
	// Called when the game starts
	// virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};

UCLASS(Blueprintable)
class TREASUREDCOVE_API UWeaponGenerator : public UObject
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWeaponGenerator();

	UFUNCTION(BlueprintCallable)
	class AGAWeapon* SpawnNewWeaponActor(const FTransform& SpawnTransform);

	UFUNCTION(BlueprintCallable)
	virtual void GenerateNewWeapon(AActor* NewWeapon);
};