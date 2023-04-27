// Fill out your copyright notice in the Description page of Project Settings.

#include "GAProjectile.h"
#include "GAProjectileMovementComponent.h"
#include "GAGameState.h"
#include "GALibrary.h"

#include "DestructibleMeshInterface.h"

#include "GACharacter.h"
#include "AbilitySystemComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/Engine.h"

// Sets default values
AGAProjectile::AGAProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	// Projectile Movement Component
	ProjectileMovementComp = 
		CreateDefaultSubobject<UGAProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->Velocity = FVector(1, 0, 0);
	ProjectileMovementComp->UpdatedComponent = RootComponent;
	ProjectileMovementComp->InitialSpeed = 3000.0f;
	ProjectileMovementComp->MaxSpeed = 3000.0f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->bShouldBounce = true;
	ProjectileMovementComp->OnProjectileBounce.AddDynamic(this, &AGAProjectile::OnBounce);
	ProjectileMovementComp->OnProjectileStop.AddDynamic(this, &AGAProjectile::OnStop);
	ProjectileMovementComp->OnProjectileHitPawn.AddDynamic(this, &AGAProjectile::OnHitPawn);
}

AGAProjectile::AGAProjectile(const FGameplayEffectSpecHandle& GESpecHandle)
	: EffectSpecHandleFromAbility(GESpecHandle)
{
	PrimaryActorTick.bCanEverTick = true;

	// Projectile Movement Component
	ProjectileMovementComp =
		CreateDefaultSubobject<UGAProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->Velocity = FVector(1, 0, 0);
	ProjectileMovementComp->UpdatedComponent = RootComponent;
	ProjectileMovementComp->InitialSpeed = 3000.0f;
	ProjectileMovementComp->MaxSpeed = 3000.0f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->bShouldBounce = true;
	ProjectileMovementComp->OnProjectileBounce.AddDynamic(this, &AGAProjectile::OnBounce);
	ProjectileMovementComp->OnProjectileStop.AddDynamic(this, &AGAProjectile::OnStop);
	ProjectileMovementComp->OnProjectileHitPawn.AddDynamic(this, &AGAProjectile::OnHitPawn);
}

//
UGAProjectileMovementComponent* AGAProjectile::GetProjectileMovementComponent()
{
	return ProjectileMovementComp;
}

void AGAProjectile::OnHitPawn_Implementation(const FHitResult& HitResult)
{
	if (HitResult.GetActor() && HitResult.GetActor() != GetOwner() && HitResult.GetActor() != GetInstigator() && EffectSpecHandleFromAbility.IsValid() && UGALibrary::GetAbilitySystemComponent(HitResult.GetActor()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile trace hit pawn!"));
		
		UGALibrary::ApplyGESpecHandleToTargetDataSpecsHandle(EffectSpecHandleFromAbility, UGALibrary::AbilityTargetDataFromActor(HitResult.GetActor()));
		if (AGACharacter* HitCharacter = Cast<AGACharacter>(HitResult.GetActor()))
		{
			// Force == Velocity * Mass
			HitCharacter->SimulateHit(HitResult, GetVelocity() * GetProjectileMovementComponent()->ProjectileGravityScale * 0.01);
		}
	}
}

void AGAProjectile::OnBounce_Implementation(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, "We hit something!");

	UInstancedStaticMeshComponent* InstancedMesh = Cast<UInstancedStaticMeshComponent>(ImpactResult.Component);

	if (InstancedMesh)
	{
		if (GetWorld())
		{
			if (AGAGameState* GameState = Cast<AGAGameState>(GetWorld()->GetGameState()))
			{
				FTransform InstanceTransform;
				InstancedMesh->GetInstanceTransform(ImpactResult.ElementIndex, InstanceTransform);

				if (GameState->CurrentStaticToDynamicMeshes.Contains(InstancedMesh->GetStaticMesh()))
				{
					IDestructibleMeshInterface* DestructibleActor =
						GetWorld()->
						SpawnActorDeferred<IDestructibleMeshInterface>(GameState->CurrentStaticToDynamicMeshes[InstancedMesh->GetStaticMesh()], InstanceTransform);

					DestructibleActor->InflictDamageToMesh(100, 100, ImpactResult.Location);
				}
			}
		}
	}
	else if (ImpactResult.GetActor() && ImpactResult.GetActor() != GetOwner() && UGALibrary::GetAbilitySystemComponent(ImpactResult.GetActor()) && EffectSpecHandleFromAbility.IsValid())
	{
		FGameplayEffectContextHandle Context;
		Context.AddSourceObject(this);
		Context.AddHitResult(ImpactResult);
		EffectSpecHandleFromAbility.Data->SetContext(Context);
		UGALibrary::ApplyGESpecHandleToTargetDataSpecsHandle(EffectSpecHandleFromAbility, UGALibrary::AbilityTargetDataFromActor(ImpactResult.GetActor()));
	}
}

void AGAProjectile::OnStop_Implementation(const FHitResult& ImpactResult)
{
	Destroy();
}
