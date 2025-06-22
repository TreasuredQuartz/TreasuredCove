// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Framework/RandomGunGenerator.h"
#include "Global/Components/Items/ShooterComponent.h"
#include "Global/Components/Shooter/ShooterAttatchment.h"
#include "Global/Components/Shooter/BodyAttatchment.h"
#include "Global/Components/Shooter/AccessoryAttatchment.h"
#include "Global/Components/Shooter/SightAttatchment.h"

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

	UBodyAttatchment* Body				= GenerateNewBody();
	UGripAttatchment* Grip				= GenerateNewGrip();
	UBarrelAttatchment* Barrel			= GenerateNewBarrel();
	UMagazineAttatchment* Magazine		= GenerateNewMagazine();
	UAccessoryAttatchment* Accessory	= GenerateNewAccessory();
	USightAttatchment* Sight			= GenerateNewSight();
	UStockAttatchment* Stock			= GenerateNewStock();

	Shooter->Barrel		= Barrel;
	Shooter->Magazine	= Magazine;

	Shooter->AddShooterAttatchment(Barrel);
	Shooter->AddShooterAttatchment(Magazine);
	Shooter->AddShooterAttatchment(Body);
	Shooter->AddShooterAttatchment(Grip);
	Shooter->AddShooterAttatchment(Accessory);
	Shooter->AddShooterAttatchment(Sight);
	Shooter->AddShooterAttatchment(Stock);
}

UBodyAttatchment* URandomGunGenerator::GenerateNewBody() const
{
	return NewObject<UBodyAttatchment>();
}

UGripAttatchment* URandomGunGenerator::GenerateNewGrip() const
{
	return NewObject<UGripAttatchment>();
}

UBarrelAttatchment* URandomGunGenerator::GenerateNewBarrel() const
{
	return NewObject<UBarrelAttatchment>();
}

UMagazineAttatchment* URandomGunGenerator::GenerateNewMagazine() const
{
	return NewObject<UMagazineAttatchment>();
}

UAccessoryAttatchment* URandomGunGenerator::GenerateNewAccessory() const
{
	return NewObject<UAccessoryAttatchment>();
}

USightAttatchment* URandomGunGenerator::GenerateNewSight() const
{
	return NewObject<USightAttatchment>();
}

UStockAttatchment* URandomGunGenerator::GenerateNewStock() const
{
	UStockAttatchment* Stock = GetAttatchment<UStockAttatchment>();



	return Stock;
}

uint8 URandomGunGenerator::GenerateNewRarity() const
{
	uint8 Rarity = FMath::RandRange(0, ((uint8)ERarityType::MAX) - 1);

	return Rarity;
}
