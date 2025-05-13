// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Global/Framework/GAProgressionManager.h"
#include "ProgressionManagerComponent.generated.h"

class FProgressionComponentDetails;
class AGAProgressionManager;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UProgressionManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProgressionManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Progression")
	TSubclassOf<AGAProgressionManager> ProgressionManagerClass;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	AGAProgressionManager* ProgressionManager;
};
