// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingItemInfoActor.generated.h"

UCLASS()
class TREASUREDCOVE_API AFloatingItemInfoActor : public AActor
{
	GENERATED_BODY()
	
private:
	//
	bool bHasBegunPlay;
	//
	FVector AnchorLocation;

public:	
	// Sets default values for this actor's properties
	AFloatingItemInfoActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	//
	UFUNCTION(BlueprintImplementableEvent, Category = "Floating Bar")
	void Initialize(FName ItemName);

	//
	FORCEINLINE bool HasBegunPlay() const { return bHasBegunPlay; };
	//
	FORCEINLINE FVector GetAnchorLocation() const { return AnchorLocation; };
};
