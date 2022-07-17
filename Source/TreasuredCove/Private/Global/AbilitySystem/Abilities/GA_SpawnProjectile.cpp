// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_SpawnProjectile.h"
#include "GACharacter.h"
#include "GAProjectile.h"
#include "GALibrary.h"
#include "AbilityTask_SpawnProjectile.h"

#include "ASWeaponStats.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

void UGA_SpawnProjectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle,ActorInfo, ActivationInfo, TriggerEventData);

	// Anything before we Commit goes here

	if (CommitAbility(Handle, ActorInfo, ActivationInfo)) // Commit...
	{
		// ...Everything else we do goes here! Make sure to call EndAbility()!!!

		float Range = 200.f;
		FHitResult Hit;
		FVector Start = AbilitySpawnLocation;
		FVector End = GetOwningCharacter()->GetMesh()->GetSocketLocation(FName("head")) * Range;
		FCollisionQueryParams CollisionParems;

		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParems))
		{
			if (Hit.bBlockingHit)
			{
				FVector Direction = Hit.ImpactPoint - AbilitySpawnLocation;
				FRotator Rotation = FRotationMatrix::MakeFromX(Direction).Rotator();
				FTransform ProjectileTransform;
				ProjectileTransform.SetLocation(AbilitySpawnLocation);
				ProjectileTransform.SetRotation(Rotation.Quaternion());

				if (GetOwningActorFromActorInfo()->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()))
				{
					//FGameplayAttribute();
					//UProperty* GET_MEMBER_NAME_CHECKED();
					//GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute();
					float ProjectileVelocity = 0.f;
					//UAbilityTask_SpawnProjectile::SpawnProjectile(this, ProjectileClass, ProjectileTransform, ProjectileVelocity, );
				}

				EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			}
			else
			{
				EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			}

		}
		else
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}