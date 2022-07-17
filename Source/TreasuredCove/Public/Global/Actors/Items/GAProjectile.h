// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "GAProjectile.generated.h"

class USphereComponent;
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

	// Projectile collision Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComp;

	// Projectile Movement Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGAProjectileMovementComponent* ProjectileMovementComp;

	//////////////////////////////////////////////////////

	// Projectile hit event
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Projectile")
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Projectile")
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:
	// Gets the projectile component
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	UGAProjectileMovementComponent* GetProjectileMovementComponent();
	
};
