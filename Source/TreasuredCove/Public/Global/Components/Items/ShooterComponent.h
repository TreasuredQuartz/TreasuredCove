// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterComponent.generated.h"

class UShooterAttatchment;
class UMagazineAttatchment;
class UBarrelAttatchment;
class UBulletCartridgeData;
class UBulletData;
class ABulletProjectile;

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

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Shooter")
	TArray<UShooterAttatchment*> Attatchments;

	UPROPERTY(BlueprintReadWrite, Category = "Shooter")
	UMagazineAttatchment* Magazine;

	UPROPERTY(BlueprintReadWrite, Category = "Shooter")
	UBarrelAttatchment* Barrel;

private:
	FTransform GetProjectileSpawnTransform() { return FTransform(); };
	float GetCurrentVarience() const;
	FVector2D GetRandomVarience(float Varience) const;
	FVector GetVectorWithVarience(const FVector& Normal, const FVector2D& Varience) const;
	FVector2D GetBulletVarience();
	FVector GetFinalLocation(const FVector& TargetLocation);

public:
	bool CanReloadMagazine();
	void CommitReload();
	void StartReload();
	float GetStability();
	FVector2D RandPointInCircle(float InRadius);
	float CalculateMuzzleVelocity(float InMass, float InForceApplied);
	float GetTwistRate();

private:
	void PreFireBullet();
	bool CanFireBullet();
	UBulletCartridgeData* CommitBullet();
	ABulletProjectile* SpawnProjectile();
	void FireProjectile(UBulletData* InBullet, float ForceApplied);
	void PostFireBullet();
	void FireBullet();
	void BurstFire();
	void ApplyRecoil();

public:
	UPROPERTY(BlueprintAssignable)
	FOnReload OnReload;
};
