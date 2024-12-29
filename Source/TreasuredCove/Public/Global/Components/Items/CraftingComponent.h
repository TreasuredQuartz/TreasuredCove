// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Global/Structs/CraftingRecipe.h"
#include "CraftingComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UCraftingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCraftingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Crafting")
	TArray<FCraftingRecipe> AvailableRecipes;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Crafting")
	bool TryCrafting(TArray<FItemKey>& InRecipe) const;

	void GetCraftableItems(TArray<UObject*>& OutItems) const;

	void GetBaseItemClass(TSubclassOf<UObject>& OutClass) const;

	TArray<UObject*> CraftableItems;
};
