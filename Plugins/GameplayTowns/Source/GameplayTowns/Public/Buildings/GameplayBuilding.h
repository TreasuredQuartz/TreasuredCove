// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayBuilding.generated.h"

class UBoxComponent;
class APawn;

class UTownSystemComponent;
class AGameplayTown;

UCLASS()
class GAMEPLAYTOWNS_API AGameplayBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameplayBuilding();

	UFUNCTION()
	void EnterBuilding(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void ExitBuilding(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameplayTowns")
	void OnEnteredBuilding(APawn* OtherPwn);
	UFUNCTION(BlueprintImplementableEvent, Category = "GameplayTowns")
	void OnExitedBuilding(APawn* OtherPawn);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BuildingName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGameplayTown* OwningTown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* EntryPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UTownSystemComponent*> Occupents;

	UFUNCTION()
	void AdmitSingleOccupent(UTownSystemComponent* NewOccupent);
	UFUNCTION()
	void ReleaseSingleOccupent(UTownSystemComponent* OldOccupent);
	UFUNCTION()
	// void CheckConstructionStatus();
	// void CheckOccupentList();
	void ReleaseAllOccupents();
	// void SetManufactureQueueLength();
	// void SetTrainingQueueLength();
	// void UpdateBuildingHealth();

};
