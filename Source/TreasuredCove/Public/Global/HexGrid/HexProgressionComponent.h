// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Global/Components/ProgressionComponent.h"
#include "HexProgressionComponent.generated.h"

class AHexGridManager;
class UHexGridEditor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UHexProgressionComponent : public UProgressionComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHexProgressionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AHexGridManager* GridManager;
	UHexGridEditor* GridEditor;
	// UHexGridCivilization* GridCivilization;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	TSubclassOf<AHexGridManager> GridManagerClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	TSubclassOf<UHexGridEditor> GridEditorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HexGrid")
	FTransform GridSpawnTransform;

public:
	// Call Manually (probably from UI I guess)
	// After calling, call StopEditing when done.
	UFUNCTION(BlueprintCallable, Category="HexGrid")
	void StartEditing();

	// Call manually
	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	void StopEditing();

	//
	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	AHexGridManager* GetHexGridManager() { return GridManager; };
};
