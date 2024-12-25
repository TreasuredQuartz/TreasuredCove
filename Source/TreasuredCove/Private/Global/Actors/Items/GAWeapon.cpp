// Fill out your copyright notice in the Description page of Project Settings.

#include "GAWeapon.h"
#include "GASkillTreeComponent.h"
#include "RarityComponent.h"
#include "UIInfoComponent.h"
#include "EquippableComponent.h"
#include "GAProjectileMovementComponent.h"
#include "Global/Components/Characters/FloatingItemInfoComponent.h"
#include "Global/Actors/Items/ItemData.h"

#include "GameplayAbilityBase.h"

#include "Components/BoxComponent.h"


AGAWeapon::AGAWeapon()
{
	// Box Collision Component
	Box =
		CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapTrigger"));
	Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	Box->OnComponentSleep.AddDynamic(this, &AGAWeapon::OnPhysicsSleep);
	Box->OnComponentBeginOverlap.AddDynamic(this, &AGAWeapon::OnOverlapBegin);
	Box->OnComponentEndOverlap.AddDynamic(this, &AGAWeapon::OnOverlapEnd);


	ProficiencyTreeComponent =
		CreateDefaultSubobject<UGASkillTreeComponent>(TEXT("Proficiency Tree"));
	RarityComponent =
		CreateDefaultSubobject<URarityComponent>(TEXT("Rarity"));
	UIInfoComponent =
		CreateDefaultSubobject<UUIInfoComponent>(TEXT("User Interface Info"));

	// ItemData =
	//	CreateDefaultSubobject<UItemData>(TEXT("Item Data"));

	RootComponent = Box;
	bBeingPickedUp = false;
}

void AGAWeapon::LaunchItem(const FVector& LaunchVelocity) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *LaunchVelocity.ToString());
	Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Box->SetSimulatePhysics(true);
	Box->AddImpulse(LaunchVelocity);
}

UItemPopupData* AGAWeapon::GetItemData() const
{
	UItemPopupData* Data = NewObject<UItemPopupData>();

	Data->DisplayIcon = Texture;
	Data->DisplayName = FText::FromName(Name);

	return Data;
}

//
void AGAWeapon::InteractedWith_Implementation(AActor* OtherActor)
{
	if (OtherActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		IInteractionInterface::Execute_AddItemToInventory(OtherActor, this);
	}
}

//
void AGAWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		IInteractionInterface::Execute_NotifyCanInteract(OtherActor, Name, true);
	}
}

//
void AGAWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		IInteractionInterface::Execute_NotifyCanInteract(OtherActor, Name, false);
	}
}

//
void AGAWeapon::OnPhysicsSleep(UPrimitiveComponent* SleepingComponent, FName BoneName)
{
	Box->SetSimulatePhysics(false);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

//
FGAItemInfo AGAWeapon::GetItemInfo() const
{
	return FGAItemInfo(
		Name,
		Texture,
		EditMenuClass);
}
