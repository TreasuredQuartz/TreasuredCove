// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PickupMagnetComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UPickupMagnetComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (EditCondition="true"))
	class USphereComponent* PickupMagnetOverlap;

public:	
	// Sets default values for this component's properties
	UPickupMagnetComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items | Repository")
	float PickupRadius;

	// Array of Static mesh awaiting destruction
	TArray<AActor*> CurrentAnimatingPickups;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Projectile")
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
