// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ShooterAttatchment.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UShooterAttatchment : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	UStaticMesh* Mesh;
};

UCLASS()
class TREASUREDCOVE_API UBulletData : public UObject
{
	GENERATED_BODY()

public:
	// Mass in Grains
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter", meta = (ForceUnits = "Grains"))
	float Mass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float DragCoefficient;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter", meta = (Units = "Millimeters"))
	float Diameter;

	// Mass in Grains
	float GetMass() { return Mass; }
};

UCLASS()
class TREASUREDCOVE_API UBulletCartridgeData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float PowderForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter", meta = (Units = "Millimeters"))
	float CasingLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter", meta = (Units = "Millimeters"))
	float Diameter;
	// Mass in Grains
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter", meta = (ForceUnits = "Grains"))
	float Mass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	TArray<UBulletData*> Projectiles;

	// Mass in Grains
	float GetMass() const
	{
		float TotalMass = Mass;
		for (UBulletData* Projectile : Projectiles) TotalMass += Projectile->GetMass();
		return TotalMass;
	};
};

UCLASS()
class TREASUREDCOVE_API UMagazineAttatchment : public UShooterAttatchment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	int32 Size;
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Shooter")
	TArray<UBulletCartridgeData*> Bullets;

	// Mass in Kilograms
	float Mass;

	// Mass in Kilograms
	float GetMass() const
	{
		float TotalMass = 0.f;
		for (UBulletCartridgeData* Cartridge : Bullets) TotalMass += Cartridge->GetMass();
		return (TotalMass / 15430.f) + Mass;
	};
	void Push(UBulletCartridgeData* Cartridge) { Bullets.Push(Cartridge); };
	UBulletCartridgeData* Pop() { return Bullets.Pop(); };
	float GetCurrentAmount() const { return Bullets.Num(); };
};

UCLASS()
class TREASUREDCOVE_API UBarrelAttatchment : public UShooterAttatchment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter", meta = (Units = "Millimeters"))
	float Length;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter", meta = (Units = "Millimeters"))
	float Diameter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	int32 RiflingRatio;
};

UCLASS()
class TREASUREDCOVE_API UStockAttatchment : public UShooterAttatchment
{
	GENERATED_BODY()

public:
};

UCLASS()
class TREASUREDCOVE_API UMuzzleAttatchment : public UShooterAttatchment
{
	GENERATED_BODY()

public:
};

UCLASS()
class TREASUREDCOVE_API UGripAttatchment : public UShooterAttatchment
{
	GENERATED_BODY()

public:
};

UCLASS()
class TREASUREDCOVE_API UForegripAttatchment : public UShooterAttatchment
{
	GENERATED_BODY()

public:
};

