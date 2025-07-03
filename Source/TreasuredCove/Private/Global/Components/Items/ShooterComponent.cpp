// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Components/Items/ShooterComponent.h"
#include "Global/Components/Shooter/ShooterAttachment.h"
#include "Global/Components/Shooter/SightAttachment.h"
#include "Global/Components/Items/EquipmentComponent.h"
#include "Global/Components/Items/InventoryComponent.h"
#include "Global/Components/Items/GAProjectileMovementComponent.h"

#include "Global/Actors/Items/BulletProjectile.h"
#include "Global/Actors/Items/MagazineHolderComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// class AContainer;

// Sets default values for this component's properties
UShooterComponent::UShooterComponent()
	: bTriggerPulled(false)
	, bAutomatic(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UShooterComponent::BeginPlay()
{

}

void UShooterComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	if (Frame)
	{
		SkeletalMeshComponent =
			Cast<USkeletalMeshComponent>(GetOwner()->AddComponentByClass(USkeletalMeshComponent::StaticClass(), false, FTransform::Identity, false));
		SkeletalMeshComponent->SetSkeletalMesh(Frame->FrameMesh);
		SkeletalMeshComponent->SetMaterial(0, Frame->FrameMaterial);
		SkeletalMeshComponent->SetAnimClass(MeshAnimationBlueprint);
		SkeletalMeshComponent->RegisterComponent();
	}

	for (UShooterAttachment* Attachment : Attachments)
	{
		if (!Attachment) continue;

		if (Attachment->IsA(UBarrelAttachment::StaticClass()))
		{
			Barrel = Cast<UBarrelAttachment>(Attachment);
		}
		else if (Attachment->IsA(UMagazineAttachment::StaticClass()))
		{
			Magazine = Cast<UMagazineAttachment>(Attachment);
		}

		FString AttachmentName;
		Attachment->GetName(AttachmentName);
		FName AttatchmentCompName = FName(TEXT("[" + AttachmentName + "] Skeletal Mesh Component"));
		USkeletalMeshComponent* AttachmentMeshComp =
			Cast<USkeletalMeshComponent>(GetOwner()->AddComponentByClass(USkeletalMeshComponent::StaticClass(), false, FTransform::Identity, false));
		AttachmentMeshComp->SetSkeletalMesh(Attachment->Mesh);
		AttachmentMeshComp->SetMaterial(0, Attachment->Material);
		AttachmentSkeletalMeshComponents.Add(AttachmentMeshComp);

		if (SkeletalMeshComponent)
		{
			FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);
			AttachmentMeshComp->AttachToComponent(SkeletalMeshComponent, AttachmentRules);
			AttachmentMeshComp->SetLeaderPoseComponent(SkeletalMeshComponent);
			AttachmentMeshComp->RegisterComponent();
		}
	}
}

void UShooterComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if (!AttachmentSkeletalMeshComponents.IsEmpty())
	{
		for (USkeletalMeshComponent* ChildMesh : AttachmentSkeletalMeshComponents)
		{
			if (ChildMesh)
			{
				ChildMesh->UnregisterComponent();
			}
		}
	}

	if (SkeletalMeshComponent)
	{
		AttachmentSkeletalMeshComponents.Empty();
		SkeletalMeshComponent->UnregisterComponent();
		SkeletalMeshComponent = nullptr;
	}

	Super::OnComponentDestroyed(bDestroyingHierarchy);
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

//////////////////////////////////////////////////////

void UShooterComponent::AddShooterAttachment(UShooterAttachment* NewAttachment)
{
	Attachments.Add(NewAttachment);
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
		AActor* Item = OwnerEquipment->GetItem(i);
		if (!Item) continue;

		UInventoryComponent* EquippedItemInventory = Item->GetComponentByClass<UInventoryComponent>();
		if (!EquippedItemInventory) continue;

		uint8 j = 0;
		for (j; j < EquippedItemInventory->GetInventorySize(); ++j) 
		{
			AActor* ItemActor = EquippedItemInventory->GetItem(j);
			if (!ItemActor) continue;

			if (UMagazineHolderComponent* OwnerMagazine = ItemActor->GetComponentByClass<UMagazineHolderComponent>())
			{
				if (!OwnerMagazine->Magazines.IsEmpty())
				{
					OwnerAmmoHolder = OwnerMagazine->Magazines;
					return true;
				}
			}
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

		if (UMagazineAttachment* OwnerMagazine = OwnerAmmoHolder[i])
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

float UShooterComponent::CalculateMuzzleVelocity(float InMass, float InForceApplied)
{
	float BarrelDiameter = Barrel->Diameter;
	float BarrelLength = Barrel->Length;
	float BulletMass = InMass;
	float ForceApplied = InForceApplied;

	// Convert MM to CM
	float Diameter = BarrelDiameter * 0.1f;
	float Length = BarrelLength * 0.1f;

	// Convert Grains to Kilograms
	float Mass = BulletMass / 15430.f;

	// Convert Diameter to Radius
	float Radius = Diameter * 0.5f;

	/* acceleration = ( Pressure * (pi) * (r^2) ) / mass */

	// Pressure
	float Acceleration = ForceApplied;
	// PI
	Acceleration *= PI;
	// Radius^2
	Acceleration = FMath::Pow(Radius, 2.f);
	// Remove Mass
	Acceleration /= Mass;

	/* velocity = (initial_velocity = 0) + 2 * (acceleration) * (distance_traveled) */

	// Initial Velocity (0) + first coefficient
	float MuzzleVelocity = 2.0f;
	// Acceleration
	MuzzleVelocity *= Acceleration;
	// Barrel Length
	MuzzleVelocity *= Length;

	// Final Velocity
	return MuzzleVelocity;
}

float UShooterComponent::GetTwistRate()
{
	float BarrelLength = Barrel->Length;
	int32 RiflingRatio = Barrel->RiflingRatio;

	return RiflingRatio <= 0 ? 0.0f : BarrelLength / RiflingRatio;
}

//////////////////////////////////////////////////////

void UShooterComponent::PreFireBullet()
{
}

bool UShooterComponent::CanFireBullet()
{
	return Magazine ? Magazine->GetCurrentAmount() > 0 : false;
}

UBulletCartridgeData* UShooterComponent::CommitBullet()
{
	if (CanFireBullet()) return Magazine->Pop();
	return nullptr;
}

ABulletProjectile* UShooterComponent::SpawnProjectile()
{
	ABulletProjectile* Projectile = Cast<ABulletProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
		GetOwner(),											// World Context Object
		ABulletProjectile::StaticClass(),					// Actor subclass to spawn
		GetOwner()->GetTransform(),							// Spawn Transform
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn,	// Should always spawn regardless of collision
		GetOwner()));										// The owner of this newly spawned actor


	return Projectile;
}

void UShooterComponent::FireProjectile(UBulletData* InBullet, float ForceApplied)
{
	ABulletProjectile* Projectile = SpawnProjectile();

	UGAProjectileMovementComponent* MoveComp = Projectile->GetProjectileMovementComponent();
	MoveComp->InitialSpeed = CalculateMuzzleVelocity(InBullet->GetMass(), ForceApplied);

	UGameplayStatics::FinishSpawningActor(Projectile, GetOwner()->GetTransform());
}

void UShooterComponent::PostFireBullet()
{
}

void UShooterComponent::FireBullet()
{
	PreFireBullet();

	if (UBulletCartridgeData* Cartridge = CommitBullet())
	{
		for (UBulletData* Bullet : Cartridge->Projectiles)
		{
			FireProjectile(Bullet, Cartridge->PowderForce);
		}
	}

	PostFireBullet();
}

void UShooterComponent::BurstFire()
{
	// float DelayBetweenBursts;

	int32 BurstAmount = 1;
	for (int32 CurBullet = 0; CurBullet < BurstAmount; ++CurBullet)
	{
		FireBullet();
	}
}

void UShooterComponent::PullTrigger()
{
	bTriggerPulled = true;

	if (!bAutomatic)
	{
		BurstFire();
	}
}

void UShooterComponent::ApplyRecoil()
{
}




