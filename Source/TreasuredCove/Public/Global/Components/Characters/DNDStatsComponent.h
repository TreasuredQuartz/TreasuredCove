// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DNDStatsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UDNDStatsComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	uint8 Charisma = 9;
	uint8 Constitution = 9;
	uint8 Dexterity = 9;
	uint8 Intelligence = 9;
	uint8 Strength = 9;
	uint8 Wisdom = 9;

public:	
	// Sets default values for this component's properties
	UDNDStatsComponent();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Stats")
	int32 GetChaScore() { return Charisma; };
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Stats")
	int32 GetConScore() { return Constitution; };
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Stats")
	int32 GetDexScore() { return Dexterity; };
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Stats")
	int32 GetIntScore() { return Intelligence; };
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Stats")
	int32 GetStrScore() { return Strength; };
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Stats")
	int32 GetWisScore() { return Wisdom; };

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetChaScore(int32 NewScore) { Charisma = (uint8)NewScore; };
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetConScore(int32 NewScore) { Constitution = (uint8)NewScore; };
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetDexScore(int32 NewScore) { Dexterity = (uint8)NewScore; };
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetIntScore(int32 NewScore) { Intelligence = (uint8)NewScore; };
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetStrScore(int32 NewScore) { Strength = (uint8)NewScore; };
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetWisScore(int32 NewScore) { Wisdom = (uint8)NewScore; };

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Stats")
	int32 GetAbilityModifier(int32 AbilityScore) { return FMath::DivideAndRoundDown(AbilityScore - 10, 2); };

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
