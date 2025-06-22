// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Global/Components/WeaponGeneratorComponent.h"
#include "RandomGunGenerator.generated.h"

class UShooterAttatchmentDatabase;
class UShooterAttatchment;
class UAccessoryAttatchment;
class UMagazineAttatchment;
class UBarrelAttatchment;
class UBodyAttatchment;
class UGripAttatchment;
class USightAttatchment;
class UStockAttatchment;

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

	UBodyAttatchment* GenerateNewBody() const;

	UGripAttatchment* GenerateNewGrip() const;

	UBarrelAttatchment* GenerateNewBarrel() const;

	UMagazineAttatchment* GenerateNewMagazine() const;

	UAccessoryAttatchment* GenerateNewAccessory() const;

	USightAttatchment* GenerateNewSight() const;

	UStockAttatchment* GenerateNewStock() const;

	uint8 GenerateNewRarity() const;

	template<class T>
	T* GetAttatchment() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Database")
	UShooterAttatchmentDatabase* Database;
};

template<class T>
inline T* URandomGunGenerator::GetAttatchment() const
{
	return nullptr;
}
