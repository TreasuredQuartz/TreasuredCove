// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CommandMenuComponent.generated.h"

class UCommandMenu;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UCommandMenuComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	TSubclassOf<UCommandMenu> ActiveMenuClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	TArray<int> MaxActiveMenuSlots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	TArray<int> MaxSubMenuSlots;

public:	
	// Sets default values for this component's properties
	UCommandMenuComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
