// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PauseAreaComponent.generated.h"

class AGAPauseStudioPawn;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UPauseAreaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPauseAreaComponent();

	UPROPERTY(EditAnywhere, Category = "PauseArea")
	TObjectPtr<const UDataLayerAsset> PauseAreaDataLayer;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Class of pause area to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AGAPauseStudioPawn> PauseAreaClass;

	// The spawned pause area
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGAPauseStudioPawn* PauseArea;

public:
	// Called to spawn pause area if not already, and then activate it.
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "FloatingText")
	void EnterPauseArea_Client();

	// Called to return to player pawn and reset the pause area to a default state.
	UFUNCTION(BlueprintCallable, Category = "Pause Area")
	void ExitPauseArea();

	//
	UFUNCTION()
	void OnPauseAreaDestroyed(AActor* DestroyedActor);

};
