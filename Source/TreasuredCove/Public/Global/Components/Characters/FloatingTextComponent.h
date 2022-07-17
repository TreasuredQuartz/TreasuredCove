// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FloatingTextComponent.generated.h"

class AFloatingTextActor;

/** A restructing of an earlier actor (AGravityWidget),
* This is based on ReidsChannel tutorial on YouTube.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UFloatingTextComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloatingText", Meta = (AllowPrivateAccess="true"))
	TSubclassOf<AFloatingTextActor> FloatingTextActorClass = nullptr;

	// Vertical spacing in between spawned text actors
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloatingText", Meta = (AllowPrivateAccess = "true"))
	float TextVerticalSpacing = 40.0f;

	// Offset to apply when spawning text actors
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloatingText", Meta = (AllowPrivateAccess = "true"))
	float TextVerticalOffset = 100.0f;
public:	
	// Sets default values for this component's properties
	UFloatingTextComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Adds specified text at specified location
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "FloatingText")
	void AddFloatingText_Client(const FText& Text, const FVector& WorldLocation);

protected:
	// Callback function called when a floating text actior is destroyed
	UFUNCTION()
	void OnTextDestroyed(AActor* DestroyedActor);

	// An array of all active floating text actors
	UPROPERTY()
	TArray<AFloatingTextActor*> ActiveTextActors;
};
