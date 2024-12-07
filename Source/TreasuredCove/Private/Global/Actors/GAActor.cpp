// Fill out your copyright notice in the Description page of Project Settings.

#include "GAActor.h"
#include "GACharacter.h"
#include "GASystemComponent.h"
#include "GAPlayerController.h"
#include "GAEnhancedInputComponent.h"
#include "GameplayAbilityBase.h"
#include "GAInputConfigData.h"
#include "AbilitySet.h"
#include "ItemMovementComponent.h"

#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ProceduralMeshComponent.h"

#include "Engine/Engine.h"

// Sets default values
AGAActor::AGAActor()
{
	// Ability System Component
	AbilitySystem =
		CreateDefaultSubobject<UGASystemComponent>(TEXT("AbilitySystem"));

	// Item Movement Component
	ItemMovement =
		CreateDefaultSubobject<UItemMovementComponent>(TEXT("Item Movement"));

	// Root
	Root =
		CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Animation Offset
	DominantHandOffset =
		CreateDefaultSubobject<UBillboardComponent>(TEXT("DominantHandOffset"));
	DominantHandOffset->SetupAttachment(Root);

	// Animation Offset
	SupportingHandOffset =
		CreateDefaultSubobject<UBillboardComponent>(TEXT("SupportingHandOffset"));
	SupportingHandOffset->SetupAttachment(Root);
}

UAbilitySystemComponent* AGAActor::GetAbilitySystemComponent() const { return AbilitySystem; }

FTransform AGAActor::GetHeldHandOffset() const
{
	return DominantHandOffset ? DominantHandOffset->GetComponentTransform() : FTransform();
}

FTransform AGAActor::GetSupportingHandOffset() const
{
	return DominantHandOffset ? SupportingHandOffset->GetComponentTransform() : FTransform();
}

// Called when the game starts or when spawned
void AGAActor::BeginPlay()
{
	Super::BeginPlay();

	IntializeAbilitySystem();
}

void AGAActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AGAActor::IntializeAbilitySystem()
{
	if (AbilitySystem)
	{
		AbilitySystem->InitAbilityActorInfo(this, this);

		if (!AttributeSetClasses.IsEmpty())
		{
			for (TSubclassOf<UAttributeSet> SetClass : AttributeSetClasses)
			{
				UAttributeSet* Set = const_cast<UAttributeSet*>(AbilitySystem->InitStats(SetClass, InitialStatsTable));
				InitializeAttributeSet(Set);
			}
		}

		if (Abilities)
		{
			FAbilitySet_GrantedHandles* OutHandles = nullptr;
			Abilities->GiveToAbilitySystem(GetAbilitySystemComponent(), OutHandles, this);
		}
	}
}

void AGAActor::InitializeAttributeSet(UAttributeSet* Set)
{

}

void AGAActor::AquireAbilities(const TArray<TSubclassOf<UGameplayAbility>>& InAbilities, TArray<FGameplayAbilitySpecHandle>& OutHandles)
{
	OutHandles.Reserve(InAbilities.Num());
	for (const auto& Ability : InAbilities)
	{
		FGameplayAbilitySpecHandle Handle;
		AquireAbility(Ability, Handle);
		OutHandles.Add(Handle);
		UE_LOG(LogTemp, Warning, TEXT("Item ability obtained."));
	}
}

void AGAActor::AquireAbility(TSubclassOf<UGameplayAbility> InAbility, FGameplayAbilitySpecHandle& OutHandle)
{
	if (AbilitySystem)
	{
		if (HasAuthority() && InAbility)
		{
			FGameplayAbilitySpecDef SpecDef = FGameplayAbilitySpecDef();
			SpecDef.Ability = InAbility;
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(SpecDef, 1);
			OutHandle = AbilitySystem->GiveAbility(AbilitySpec);
		}
	}
}

void AGAActor::AbilityInputTagPressed(FGameplayTag Tag)
{
	if (AbilitySystem) AbilitySystem->AbilityInputTagPressed(Tag);
}
void AGAActor::AbilityInputTagReleased(FGameplayTag Tag)
{
	if (AbilitySystem) AbilitySystem->AbilityInputTagReleased(Tag);
}

void AGAActor::SetupPlayerAbilityInput(UGAEnhancedInputComponent* EIC, AGAPlayerController* PlayerController)
{
	if (EIC)
	{
		EIC->BindAbilityActions(InputAbilityActions, this, &AGAActor::AbilityInputTagPressed, &AGAActor::AbilityInputTagReleased, /*out*/ CurrentActiveAbilityHandles);
	}
	if (PlayerController)
	{
		PlayerController->OnPostProcessInput.AddUniqueDynamic(AbilitySystem, &UGASystemComponent::ProcessAbilityInput);
	}
}

void AGAActor::RemovePlayerAbilityInput(UGAEnhancedInputComponent* EIC, AGAPlayerController* PlayerController)
{
	if (EIC)
	{
		TArray<uint32> BindHandles;
		for (uint32 Handle : CurrentActiveAbilityHandles)
		{
			EIC->RemoveBindingByHandle(Handle);
		}
	}
	if (PlayerController)
	{
		PlayerController->OnPostProcessInput.RemoveAll(AbilitySystem);
	}
}

//void AGAActor::UsePrimary_Implementation()
//{
//	if (CurrentActiveAbilityHandles.IsValidIndex(0) && CurrentActiveAbilityHandles[0].IsValid())
//	{
//		AbilitySystem->TryActivateAbility(CurrentActiveAbilityHandles[0]);
//	}
//	else UE_LOG(LogTemp, Warning, TEXT("Item Primary not called."));
//}
//
//void AGAActor::UseSecondary_Implementation()
//{
//	if (CurrentActiveAbilityHandles.IsValidIndex(1) && CurrentActiveAbilityHandles[1].IsValid()) AbilitySystem->TryActivateAbility(CurrentActiveAbilityHandles[1]);
//	else UE_LOG(LogTemp, Warning, TEXT("Item Secondary not called."));
//}
//
//void AGAActor::UseThrow_Implementation()
//{
//	if (CurrentActiveAbilityHandles.IsValidIndex(2) && CurrentActiveAbilityHandles[2].IsValid()) AbilitySystem->TryActivateAbility(CurrentActiveAbilityHandles[2]);
//}
//
//void AGAActor::UseMelee_Implementation()
//{
//	if (CurrentActiveAbilityHandles.IsValidIndex(3) && CurrentActiveAbilityHandles[3].IsValid()) AbilitySystem->TryActivateAbility(CurrentActiveAbilityHandles[3]);
//}

void AGAActor::OnPickedUp_Implementation()
{
	bPickedUp = true;
	SetActorEnableCollision(false);
	DisableComponentsSimulatePhysics();
	ItemMovement->Deactivate();
}

void AGAActor::OnDropped_Implementation()
{
	bPickedUp = false;
	SetOwner(nullptr);
	DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	ItemMovement->Activate();
}

void AGAActor::OnEquipped_Implementation()
{
	bEquipped = true;
}

void AGAActor::OnUnEquipped_Implementation()
{
	bEquipped = false;
}

void AGAActor::LaunchItem(FVector LaunchVelocity, bool bXYOverride, bool bZOverride)
{
	if (ItemMovement)
	{
		FVector FinalVel = LaunchVelocity;
		const FVector Velocity = GetVelocity();

		if (!bXYOverride)
		{
			FinalVel.X += Velocity.X;
			FinalVel.Y += Velocity.Y;
		}
		if (!bZOverride)
		{
			FinalVel.Z += Velocity.Z;
		}

		ItemMovement->Launch(FinalVel);

		OnLaunched(LaunchVelocity, bXYOverride, bZOverride);
	}
}


