// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingBarActor.generated.h"

UCLASS()
class TREASUREDCOVE_API AFloatingBarActor : public AActor
{
	GENERATED_BODY()
	
private:
	// Initial Location when spawned
	FVector AnchorLocation;

public:	
	// Sets default values for this actor's properties
	AFloatingBarActor();

	inline const FVector& GetAnchorLocation() const { return AnchorLocation; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 
	UFUNCTION(BlueprintImplementableEvent, Category = "Floating Bar")
	void Initialize();

	//
	UFUNCTION(BlueprintImplementableEvent, Category = "Floating Bar")
	void UpdateProgress(float Current, float Max);
};
