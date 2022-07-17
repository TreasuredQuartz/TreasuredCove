// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "GABarrier.generated.h"

class UBoxComponent;

struct FGameplayEffectSpecHandle;

UCLASS()
class TREASUREDCOVE_API AGABarrier : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGABarrier();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FGameplayEffectSpecHandle EffectSpecHandleFromAbility;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CollisionComp;

	UFUNCTION()
	void OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
