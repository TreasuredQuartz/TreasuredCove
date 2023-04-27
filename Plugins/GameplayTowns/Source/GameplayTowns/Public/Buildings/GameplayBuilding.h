// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayBuilding.generated.h"

class UBoxComponent;
class APawn;

class UTownSystemComponent;
class AGameplayTown;
class AGameplayRoom;

UCLASS()
class GAMEPLAYTOWNS_API AGameplayBuilding : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	TArray<AGameplayRoom*> Rooms;
	
public:	
	// Sets default values for this actor's properties
	AGameplayBuilding();

	virtual void OnConstruction(const FTransform& Transform) override;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AGameplayRoom>> RoomClasses;

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
