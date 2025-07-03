// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Global/Structs/CustomUnits.h"
#include "ShooterComponent.generated.h"

class UShooterAttachment;
class UAccessoryAttachment;
class UMagazineAttachment;
class UBarrelAttachment;
class UBodyAttachment;
class UGripAttachment;
class USightAttachment;
class UStockAttachment;

class UBulletCartridgeData;
class UBulletData;
class ABulletProjectile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReload);

typedef FRPM rpm;

UENUM(BlueprintType)
enum class EActionCycleState : uint8
{
	None,
	Feeding,
	Chambering,
	Locking,
	Firing,
	Unlocking,
	Etracting,
	Ejecting,
	Cocking,
	MAX,
};

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
	TArray<UMagazineAttachment*> OwnerAmmoHolder;

public:	
	// Sets default values for this component's properties
	UShooterComponent();

	// Called when created during play.
	virtual void BeginPlay() override;

	// Called when component is created, in editor or otherwise.
	virtual void OnComponentCreated() override;

	// Called when component is destroyed, in editor or otherwise.
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	//
	UFUNCTION(BlueprintCallable, Category = "Shooter")
	void FireAtTarget(AActor* TargetActor, float Accuracy);

	UFUNCTION(BlueprintCallable, Category = "Shooter")
	void PullTrigger();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	TArray<UShooterAttachment*> Attachments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	UShooterFrameData* Frame;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Shooter")
	UBarrelAttachment* Barrel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	UMagazineAttachment* Magazine;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	UAnimBlueprintGeneratedClass* MeshAnimationBlueprint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter")
	TArray<USkeletalMeshComponent*> AttachmentSkeletalMeshComponents;

private:
	FTransform GetProjectileSpawnTransform() { return FTransform(); };
	float GetCurrentVarience() const;
	FVector2D GetRandomVarience(float Varience) const;
	FVector GetVectorWithVarience(const FVector& Normal, const FVector2D& Varience) const;
	FVector2D GetBulletVarience();
	FVector GetFinalLocation(const FVector& TargetLocation);

public:
	void AddShooterAttachment(UShooterAttachment* NewAttachment);

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

/**
 *
 */
UCLASS(BlueprintType)
class TREASUREDCOVE_API UShooterFrameData : public UDataAsset
{
	GENERATED_BODY()

public:
	// Weight in Kilograms
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float Mass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	USkeletalMesh* FrameMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	UMaterialInterface* FrameMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	FRPM FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float BurstFireSpeed;

	// 0 -> Automatic
	// 1 -> Semi-Automatic
	// 2 -> 2-Burst
	// 3 -> 3-Burst
	// Etc.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	uint8 BurstFireAmount;

public:
	// The action of a gun is part of the frame.

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	// USkeletalMesh* ActionMesh;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	// UMaterialInterface* ActionMaterial;

	UPROPERTY(BlueprintReadWrite, Category = "Shooter")
	EActionCycleState CurrentState;

	// Directly equivellant to fire rate; represents the time required to perform
	// functions required to fire another bullet.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float CycleRate;

	UFUNCTION(BlueprintCallable)
	virtual float GetCycleRate() const { return CycleRate; };

	// A cartridge being stripped from the magazine
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	// float FeedingSpeed;
	// 
	// The cartridge being placed into the barrel chamber
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	// float ChamberingSpeed;
	// 
	// // The bolt/breech face/cylinder is locked into position with the bore/barrrel.
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	// float LockingSpeed;
	// 
	// // This point the gun would stay in a state awaiting for the trigger to fire.
	// // A trigger press rotates the sear, which sends the firing pin or striker against the 
	// // primer, causing ignition of the powder.
	// // This results in gas expansion within the chambered cartridge which propels the
	// // projectile into and through the barrel bore.
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	// float FiringSpeed;
	// 
	// // The backward movement of the bolt or slide/breech face unseating the cartridge case 
	// // from the chamber and engaging the disconnector.
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	// float UnlockingSpeed;
	// 
	// // Pull the cartridge case from the chamber.
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	// float ExtractionSpeed;
	// 
	// // Ejects the cartridge case through the ejection port.
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	// float EjectionSpeed;
	// 
	// // The hammer or striker is engaged by the sear and ready for the resumption 
	// // of operations.
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	// float CockingSpeed;
	// 
	// Ejects the cartridge case through the ejection port.
};
