// Fill out your copyright notice in the Description page of Project Settings.

#include "GAWeapon.h"

#include "GameplayAbilityBase.h"

#include "Components/BoxComponent.h"


AGAWeapon::AGAWeapon()
{
	// Box Collision Component
	Box =
		CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapTrigger"));
	Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	Box->OnComponentBeginOverlap.AddDynamic(this, &AGAWeapon::OnOverlapBegin);
	Box->OnComponentEndOverlap.AddDynamic(this, &AGAWeapon::OnOverlapEnd);

	// Skeletal Mesh Component
	Mesh =
		CreateDefaultSubobject<UMeshComponent, USkeletalMeshComponent>(FName(TEXT("Mesh")));
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	RootComponent = Box;
	Mesh->SetupAttachment(Box);
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
		IInteractionInterface::Execute_NotifyCanPickup(OtherActor, this, true);
	}
}

//
void AGAWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		IInteractionInterface::Execute_NotifyCanPickup(OtherActor, this, false);
	}
}

//
void AGAWeapon::GetItemInfo(FGAItemInfo& Info)
{
	Info = FGAItemInfo(
		this,
		Name,
		Texture,
		ActiveMenuClass,
		EditMenuClass);
}
