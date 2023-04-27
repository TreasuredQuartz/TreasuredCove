// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FloatingBarComponent.generated.h"

class AFloatingBarActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UFloatingBarComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	float BarVerticalOffset;
	float BarVerticalSpacing;
	TArray<AFloatingBarActor*> ActiveBarActors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloatingBar", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AFloatingBarActor> FloatingBarActorClass;

public:	
	// Sets default values for this component's properties
	UFloatingBarComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Adds specified text at specified location
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "FloatingBar")
	void AddFloatingBar_Client(int Layers, int Bars, const FVector& WorldLocation);

	// Bound to floating bar actors on destoryed delegate
	UFUNCTION()
	void OnBarDestroyed(AActor* DestroyedActor);

	// Updates the progress of the bar at passed in index
	UFUNCTION()
	void UpdateProgress(int32 Index, float Current, float Max);

	// checks that index for bar exists
	UFUNCTION()
	bool HasIndex(int32 Index);

	// Clears active bar actor array
	UFUNCTION()
	void DestroyAllBars();
};
