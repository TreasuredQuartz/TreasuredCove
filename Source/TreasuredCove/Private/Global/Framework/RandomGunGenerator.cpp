// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Framework/RandomGunGenerator.h"
#include "Global/Components/Items/ShooterComponent.h"
#include "Global/Components/Shooter/ShooterAttachment.h"
// #include "Global/Components/Shooter/BodyAttachment.h"
#include "Global/Components/Shooter/AccessoryAttachment.h"
#include "Global/Components/Shooter/SightAttachment.h"

#include "Global/Components/Items/RarityComponent.h"
#include "Global/Components/Items/ProficiencyComponent.h"

void URandomGunGenerator::GenerateNewWeapon(AActor* NewWeapon)
{
	GenerateNewGun(NewWeapon);
}

void URandomGunGenerator::GenerateNewGun(AActor* InGun)
{
	if (!InGun) return;

	UShooterComponent* Shooter = Cast<UShooterComponent>(
		InGun->AddComponentByClass(UShooterComponent::StaticClass(), false, FTransform::Identity, false)
	);

	URarityComponent* Rarity = Cast<URarityComponent>(
		InGun->AddComponentByClass(URarityComponent::StaticClass(), false, FTransform::Identity, false)
	);

	UProficiencyComponent* Proficiency = Cast<UProficiencyComponent>(
		InGun->AddComponentByClass(UProficiencyComponent::StaticClass(), false, FTransform::Identity, false)
	);

	Rarity->Rarity		= (ERarityType)GenerateNewRarity();

	// UBodyAttachment* Body				= GenerateNewBody();
	UGripAttachment* Grip				= GenerateNewGrip();
	UBarrelAttachment* Barrel			= GenerateNewBarrel();
	UMagazineAttachment* Magazine		= GenerateNewMagazine();
	UAccessoryAttachment* Accessory	= GenerateNewAccessory();
	USightAttachment* Sight			= GenerateNewSight();
	UStockAttachment* Stock			= GenerateNewStock();

	Shooter->Barrel		= Barrel;
	Shooter->Magazine	= Magazine;

	Shooter->AddShooterAttachment(Barrel);
	Shooter->AddShooterAttachment(Magazine);
	// Shooter->AddShooterAttachment(Body);
	Shooter->AddShooterAttachment(Grip);
	Shooter->AddShooterAttachment(Accessory);
	Shooter->AddShooterAttachment(Sight);
	Shooter->AddShooterAttachment(Stock);
}

//UBodyAttachment* URandomGunGenerator::GenerateNewBody() const
//{
//	return NewObject<UBodyAttachment>();
//}

UGripAttachment* URandomGunGenerator::GenerateNewGrip() const
{
	return NewObject<UGripAttachment>();
}

UBarrelAttachment* URandomGunGenerator::GenerateNewBarrel() const
{
	return NewObject<UBarrelAttachment>();
}

UMagazineAttachment* URandomGunGenerator::GenerateNewMagazine() const
{
	return NewObject<UMagazineAttachment>();
}

UAccessoryAttachment* URandomGunGenerator::GenerateNewAccessory() const
{
	return NewObject<UAccessoryAttachment>();
}

USightAttachment* URandomGunGenerator::GenerateNewSight() const
{
	return NewObject<USightAttachment>();
}

UStockAttachment* URandomGunGenerator::GenerateNewStock() const
{
	UStockAttachment* Stock = GetAttachment<UStockAttachment>();



	return Stock;
}

uint8 URandomGunGenerator::GenerateNewRarity() const
{
	uint8 Rarity = FMath::RandRange(0, ((uint8)ERarityType::MAX) - 1);

	return Rarity;
}
