// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Components/Items/ShooterComponent.h"
#include "ShooterAttatchment.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "GAActor.h"

// class AContainer;

// Sets default values for this component's properties
UShooterComponent::UShooterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UShooterComponent::FireAtTarget(AActor* TargetActor, float Accuracy)
{
	UWorld* World = GetWorld();
	if (World == nullptr || TargetActor == nullptr) return;

	FHitResult Hit;
	{
		const FVector& Start = GetOwner()->GetActorLocation();
		const FVector& TargetLocation = TargetActor->GetActorLocation();
		const FVector& TargetDirection = TargetLocation - Start;
		const FVector& EndDelta = GetVectorWithVarience(TargetDirection, GetRandomVarience(Accuracy));
		const FVector& End = TargetLocation + EndDelta;
		ECollisionChannel Channel = ECC_Camera;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetOwner());
		World->LineTraceSingleByChannel(Hit, Start, End, Channel, Params);
	}

	if (Hit.bBlockingHit)
	{

	}
}

float UShooterComponent::GetCurrentVarience() const
{
	// GetDiscipline();
	return Control;
}

FVector2D UShooterComponent::GetRandomVarience(float Varience) const
{
	float x = FMath::RandRange(-Varience, Varience);
	float y = FMath::RandRange(-Varience, Varience);

	return FVector2D(x, y);
}

FVector UShooterComponent::GetVectorWithVarience(const FVector& Normal, const FVector2D& Varience) const
{
	FVector XAxis;
	FVector YAxis;
	Normal.FindBestAxisVectors(XAxis, YAxis);

	return (XAxis * Varience.X)
		 + (YAxis * Varience.Y);
}

FVector2D UShooterComponent::GetBulletVarience()
{
	FVector2D BulletVarience;
	SpreadPattern.Dequeue(BulletVarience);
	return BulletVarience;
}

FVector UShooterComponent::GetFinalLocation(const FVector& TargetLocation)
{
	const FVector& Start = GetOwner()->GetActorLocation();
	const FVector& TargetDirection = TargetLocation - Start;
	const FVector2D& Varience = GetRandomVarience(GetCurrentVarience());
	const FVector2D& BulletVarience = GetBulletVarience();
	const FVector& CurrentVarience = GetVectorWithVarience(TargetDirection, Varience) + GetVectorWithVarience(TargetDirection, BulletVarience);

	return TargetLocation +
		CurrentVarience;
}

bool UShooterComponent::CanFireBullet()
{
	// return Magazine ? Magazine->GetCurrentAmount > 0 : false;
	return true;
}

UObject* UShooterComponent::CommitBullet()
{
	if (CanFireBullet()) return Magazine->Pop();
	return nullptr;
}

AActor* UShooterComponent::SpawnProjectile()
{
	AActor* Projectile = UGameplayStatics::BeginDeferredActorSpawnFromClass(
		GetOwner(),											// World Context Object
		TSubclassOf<AActor>(),								// Actor subclass to spawn
		GetOwner()->GetTransform(),							// Spawn Transform
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn,	// Should always spawn regardless of collision
		GetOwner());										// The owner of this newly spawned actor


	return Projectile;
}

void UShooterComponent::FireProjectile()
{
	AActor* Projectile = SpawnProjectile();
	CalculateMuzzleVelocity();
}

//////////////////////////////////////////////////////

bool UShooterComponent::CanReloadMagazine()
{
	// Check if our owner has any equipped ammo stashes that we can reload from
	UEquipmentComponent* OwnerEquipment = GetOwner()->GetComponentByClass<UEquipmentComponent>();
	if (!OwnerEquipment) return false;

	uint8 i = 0;
	for (i; i < OwnerEquipment->GetInventorySize(); ++i)
	{
		AGAActor* Item = OwnerEquipment->GetItem(i);
		if (!Item) continue;

		UInventoryComponent* EquippedItemInventory = Item->GetComponentByClass<UInventoryComponent>();
		if (!EquippedItemInventory) continue;

		uint8 j = 0;
		for (j; j < EquippedItemInventory->GetInventorySize(); ++j) 
		{
			EquippedItemInventory->GetItem(j);
		}
	}

	return false;
}

void UShooterComponent::CommitReload()
{
	// Remove the magazine from the owner and add it to ourself.
	for (int32 i = 0; i < OwnerAmmoHolder.Num(); ++i)
	{
		if (!OwnerAmmoHolder.IsValidIndex(i)) continue;

		if (UMagazineAttatchment* OwnerMagazine = OwnerAmmoHolder[i])
		{
			Magazine = OwnerMagazine;
			OwnerAmmoHolder.RemoveAtSwap(i);
			OnReload.Broadcast();
			break;
		}
	}
}

void UShooterComponent::StartReload()
{
	if (CanReloadMagazine())
	{
		CommitReload();
	}
}

float UShooterComponent::GetStability()
{
	return 0.0f;
}

FVector2D UShooterComponent::RandPointInCircle(float InRadius)
{
	return FMath::RandPointInCircle(InRadius);
}

float UShooterComponent::CalculateMuzzleVelocity()
{
	float BarrelDiameter = 0.0f;
	float BarrelLength = 0.0f;
	float BulletMass = 0.0f;
	float ForceApplied = 0.0f;

	float MuzzleVelocity = 0.0f;

	// Convert MM to CM
	MuzzleVelocity = BarrelDiameter * 0.1f;

	// Calculate Bullet Pressure
	MuzzleVelocity *= 0.5f;
	MuzzleVelocity = FMath::Pow(MuzzleVelocity, 2.f);
	MuzzleVelocity *= PI;
	MuzzleVelocity *= ForceApplied;

	// IDK
	MuzzleVelocity /= BulletMass;
	MuzzleVelocity *= BarrelLength;
	MuzzleVelocity *= 2.f;

	return MuzzleVelocity;
}

float UShooterComponent::GetTwistRate()
{
	float BarrelLength = 0.0f;
	int32 RiflingRatio = 0;

	return RiflingRatio <= 0 ? 0.0f : BarrelLength / RiflingRatio;
}

//////////////////////////////////////////////////////

void UShooterComponent::BurstFire()
{
	// float DelayBetweenBursts;

	int32 BurstAmount = 1;
	for (int32 CurBullet = 0; CurBullet < BurstAmount; ++CurBullet)
	{
		FireBullet();
	}
}

void UShooterComponent::FireBullet()
{
	PreFireBullet();
	CommitBullet();

	{
		FireProjectile();
	}

	PostFireBullet();
}

void UShooterComponent::PostFireBullet()
{
}

void UShooterComponent::ApplyRecoil()
{
}

void UShooterComponent::PullTrigger()
{
	bTriggerPulled = true;

	if (!bAutomatic)
	{
		BurstFire();
	}
}

void UShooterComponent::PreFireBullet()
{
}

