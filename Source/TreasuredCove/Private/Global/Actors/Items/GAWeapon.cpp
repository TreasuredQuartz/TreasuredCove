// Fill out your copyright notice in the Description page of Project Settings.

#include "GAWeapon.h"
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

	//ItemData =
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

UObject* AGAWeapon::GetItemData()
{
	return ItemData;
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
void AGAWeapon::GetItemInfo(FGAItemInfo& Info)
{
	Info = FGAItemInfo(
		Name,
		Texture,
		ActiveMenuClass,
		EditMenuClass);
}
