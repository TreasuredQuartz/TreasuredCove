// Fill out your copyright notice in the Description page of Project Settings.

#include "GAProjectile.h"
#include "GAProjectileMovementComponent.h"
#include "GACharacter.h"
#include "GAGameState.h"
#include "GALibrary.h"

#include "DestructibleMeshInterface.h"

#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/Engine.h"

// Sets default values
AGAProjectile::AGAProjectile()
	: EffectSpecHandleFromAbility(nullptr)
{
	// Collision Component
	CollisionComp = 
		CreateDefaultSubobject<USphereComponent>(TEXT("Collision Comp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("OverlapOnlyPawn");
	CollisionComp->OnComponentHit.AddDynamic(this, &AGAProjectile::OnHit);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AGAProjectile::OnBeginOverlap);

	// Cannot Walk on Projectile
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root
	RootComponent = CollisionComp;
	
	// Projectile Movement Component
	ProjectileMovementComp = 
		CreateDefaultSubobject<UGAProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->Velocity = FVector(1, 0, 0);
	ProjectileMovementComp->UpdatedComponent = CollisionComp;
	ProjectileMovementComp->InitialSpeed = 3000.0f;
	ProjectileMovementComp->MaxSpeed = 3000.0f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->bShouldBounce = true;
}

AGAProjectile::AGAProjectile(const FGameplayEffectSpecHandle& GESpecHandle)
	: EffectSpecHandleFromAbility(GESpecHandle)
{
}

//
UGAProjectileMovementComponent* AGAProjectile::GetProjectileMovementComponent()
{
	return ProjectileMovementComp;
}

void AGAProjectile::OnHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetInstigator())
		return;

	//this->Destroy();

	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, "We hit something!");
}

void AGAProjectile::OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetInstigator())
		return;

	AGACharacter* OtherCharacter = Cast<AGACharacter>(OtherActor);

	UInstancedStaticMeshComponent* InstancedMesh = Cast<UInstancedStaticMeshComponent>(OtherComp);

	if (OtherCharacter)
	{
		AGACharacter* CharaInstigator = Cast<AGACharacter>(GetInstigator());

		if (CharaInstigator)
		{
			UGALibrary::ApplyGESpecHandleToTargetDataSpecsHandle(EffectSpecHandleFromAbility, UGALibrary::AbilityTargetDataFromActor(OtherCharacter));
			GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, "We Overlapped a character!");
		}
	}
	else if (InstancedMesh)
	{
		AGAGameState* GameState = Cast<AGAGameState>(GetWorld()->GetGameState());
		if (GameState)
		{
			FTransform InstanceTransform;
			InstancedMesh->GetInstanceTransform(OtherBodyIndex, InstanceTransform);

			if (GameState->CurrentStaticToDynamicMeshes.Contains(InstancedMesh->GetStaticMesh()))
			{
				IDestructibleMeshInterface* DestructibleActor =
					GetWorld()->
					SpawnActorDeferred<IDestructibleMeshInterface>(GameState->CurrentStaticToDynamicMeshes[InstancedMesh->GetStaticMesh()], InstanceTransform);

				DestructibleActor->InflictDamageToMesh(100, 100, SweepResult.Location);
			}
		}
	}
}
