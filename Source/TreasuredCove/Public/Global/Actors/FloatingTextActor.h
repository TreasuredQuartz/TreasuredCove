// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingTextActor.generated.h"

UCLASS(Abstract)
class TREASUREDCOVE_API AFloatingTextActor : public AActor
{
	GENERATED_BODY()
private:
	// The location to which the text is anchored
	FVector AnchorLocation;
	/*UPROPERTY(VisibkleAnywhere, BlueprintReadOnly, Category = "Floating Text", Meta = (ShouldAllowPrivateAccess = "true"))
	UWidgetComponent* WidgetComponent;*/

protected:
	// Whether this actor will handle the setting of its location or not
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Floating Text", Meta = (ShouldAllowPrivateAccess = "true"))
	bool bSelfUpdateLocation;
public:	
	// Sets default values for this actor's properties
	AFloatingTextActor();

	UFUNCTION(BlueprintImplementableEvent, Category = "FloatingText")
	void Initialize(const FText& Text);

	inline const FVector& GetAnchorLocation() const { return AnchorLocation; }

	inline bool DoesSelfUpdateLocation() const { return bSelfUpdateLocation; }

	// const class UWidget* GetUserWidget() const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

};
