// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "GAProjectile.generated.h"

class UGAProjectileMovementComponent;

UCLASS()
class TREASUREDCOVE_API AGAProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGAProjectile();

	AGAProjectile(const FGameplayEffectSpecHandle& GESpecHandle);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FGameplayEffectSpecHandle EffectSpecHandleFromAbility;
protected:

	//////////////////////////////////////////////////////

	// Projectile Movement Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGAProjectileMovementComponent* ProjectileMovementComp;

	//////////////////////////////////////////////////////

	// Projectile hit event
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Projectile")
	void OnHitPawn(const FHitResult& ImpactResult);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Projectile")
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Projectile")
	void OnStop(const FHitResult& ImpactResult);
public:
	// Gets the projectile component
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	UGAProjectileMovementComponent* GetProjectileMovementComponent();
	
};
