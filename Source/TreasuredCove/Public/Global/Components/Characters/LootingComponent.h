// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LootingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API ULootingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULootingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	//
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Looting")
	TMap<FName, float> LootTable;

public:
	UFUNCTION(BlueprintPure, BlueprintCallable)
	TMap<FName, float> GetLootTable() const { return LootTable; };

	UFUNCTION(BlueprintCallable)
	void AddLootToTable(FName LootName, float Chance) { LootTable.FindOrAdd(LootName) += Chance; };
};
