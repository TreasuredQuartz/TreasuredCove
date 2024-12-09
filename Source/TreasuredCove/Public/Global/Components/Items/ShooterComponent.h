// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterComponent.generated.h"

class UShooterAttatchment;
class UMagazineAttatchment;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReload);

/**
 *
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UShooterComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	bool bTriggerPulled;
	bool bAutomatic;

	float Control;
	FVector2D CurrentRecoil;
	TQueue<FVector2D> SpreadPattern;
	TArray<UMagazineAttatchment*> OwnerAmmoHolder;

public:	
	// Sets default values for this component's properties
	UShooterComponent();

	//
	UFUNCTION(BlueprintCallable, Category = "Shooter")
	void FireAtTarget(AActor* TargetActor, float Accuracy);

	UFUNCTION(BlueprintCallable, Category = "Shooter")
	void PullTrigger();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	TArray<UShooterAttatchment*> Attatchments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	UMagazineAttatchment* Magazine;

private:
	float GetCurrentVarience() const;
	FVector2D GetRandomVarience(float Varience) const;
	FVector GetVectorWithVarience(const FVector& Normal, const FVector2D& Varience) const;
	FVector2D GetBulletVarience();
	FVector GetFinalLocation(const FVector& TargetLocation);

private:
	FTransform GetProjectileSpawnTransform() { return FTransform(); };
	bool CanFireBullet();
	UObject* CommitBullet();
	AActor* SpawnProjectile();
	void FireProjectile();

public:
	bool CanReloadMagazine();
	void CommitReload();
	void StartReload();

public:
	float GetStability();
	FVector2D RandPointInCircle(float InRadius);
	float CalculateMuzzleVelocity();
	float GetTwistRate();
	void BurstFire();

private:
	void PreFireBullet();
	void FireBullet();
	void PostFireBullet();
	void ApplyRecoil();

public:
	UPROPERTY(BlueprintAssignable)
	FOnReload OnReload;
};
