// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Global/Components/WeaponGeneratorComponent.h"
#include "RandomGunGenerator.generated.h"

class UShooterAttachmentDatabase;
class UShooterAttachment;
class UAccessoryAttachment;
class UMagazineAttachment;
class UBarrelAttachment;
// class UBodyAttachment;
class UGripAttachment;
class USightAttachment;
class UStockAttachment;

/**
 * 
 */
UCLASS(BlueprintType)
class TREASUREDCOVE_API URandomGunGenerator : public UWeaponGenerator
{
	GENERATED_BODY()
	
public:
	virtual void GenerateNewWeapon(AActor* NewWeapon) override;

	UFUNCTION(BlueprintCallable)
	void GenerateNewGun(class AActor* InGun);

	// UBodyAttachment* GenerateNewBody() const;

	UGripAttachment* GenerateNewGrip() const;

	UBarrelAttachment* GenerateNewBarrel() const;

	UMagazineAttachment* GenerateNewMagazine() const;

	UAccessoryAttachment* GenerateNewAccessory() const;

	USightAttachment* GenerateNewSight() const;

	UStockAttachment* GenerateNewStock() const;

	uint8 GenerateNewRarity() const;

	template<class T>
	T* GetAttachment() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Database")
	UShooterAttachmentDatabase* Database;
};

template<class T>
inline T* URandomGunGenerator::GetAttachment() const
{
	return nullptr;
}
