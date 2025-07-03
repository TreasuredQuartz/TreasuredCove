// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Global/Structs/BulletStruct.h"
#include "Global/Structs/CustomUnits.h"
#include "ShooterAttachment.generated.h"

typedef FGrains grains;

/**
 *
 */
UCLASS()
class TREASUREDCOVE_API UShooterAttachmentDatabase : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shooter")
	TArray<TSubclassOf<UShooterAttachment>> Database;
};

/**
 * 
 */
UCLASS(Abstract)
class TREASUREDCOVE_API UShooterAttachment : public UDataAsset
{
	GENERATED_BODY()

private:
	// Mass in Kilograms
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Shooter")
	float Mass;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	USkeletalMesh* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	UMaterialInterface* Material;

	virtual float GetMass() const { return Mass; };
};

/** Bullet drop is affected by the atmospheric pressure.
* - For every ~1,000ft higher in elevation than sea level
*   that the bullet is fired, ~1% of bullet drop is 
*   reduced.
* - Cold air creates more drag.
* - Dry air creates more drag.
*/
UCLASS(EditInlineNew)
class TREASUREDCOVE_API UBulletData : public UDataAsset
{
	GENERATED_BODY()

public:
	// Mass in Grains
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	FGrains Mass;
	// The ballestic coefficient: essentially the capability of the bullet to 
	// ignore air resistance or "drag."
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float DragCoefficient;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter", meta = (ForceUnits = "Millimeters"))
	float RiflingTwist;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter", meta = (ForceUnits = "Millimeters"))
	float Diameter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	UPhysicalMaterial* Material;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	EBulletJacket JacketType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	EBulletHead HeadType;

	// Mass in Grains
	float GetMass() { return Mass.Value; }
};

UCLASS(EditInlineNew)
class TREASUREDCOVE_API UBulletCartridgeData : public UDataAsset
{
	GENERATED_BODY()

private:
	/*
	* 0: Rimless
	* 1: Rimmed
	* 2: Belted
	* 3: Straight
	*/
	// uint8 CartridgeType;

public:
	// The proppellant detenation force that drives the bullet projectile.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter", meta = (Units = "Newtons"))
	float PowderForce;
	// The length of the casing discluding the bullet projectile.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter", meta = (ForceUnits = "Millimeters"))
	float CasingLength;
	// The diameter of the casing.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter", meta = (ForceUnits = "Millimeters"))
	float Diameter;

	// Mass in Grains
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	FGrains Mass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	TArray<UBulletData*> Projectiles;

	// Mass in Grains
	float GetMass() const
	{
		float TotalMass = Mass.Value;
		for (UBulletData* Projectile : Projectiles) TotalMass += Projectile->GetMass();
		return TotalMass;
	};
};

/** This Attatchment is simluatainously in control of: 
* - damage,
* - reload speed, 
* - handling, 
* - Ammunition type,
* - magazine size.
*/
UCLASS(Blueprintable)
class TREASUREDCOVE_API UMagazineAttachment : public UShooterAttachment
{
	GENERATED_BODY()

public:
	UMagazineAttachment() 
		: CartridgeType(nullptr)
		, MaxCartridges(10)
	{
		Bullets.Reserve(MaxCartridges);
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	UBulletCartridgeData* CartridgeType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	int32 MaxCartridges;

	/**
	* This should be a static size;
	* bullets simply become nullptrs when empty.
	*/
	// UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Shooter")
	TArray<UBulletCartridgeData*> Bullets;

	// Mass in Kilograms
	virtual float GetMass() const override
	{
		float TotalMass = 0.f;
		for (UBulletCartridgeData* Cartridge : Bullets) TotalMass += Cartridge->GetMass();
		return (TotalMass / 15432.f) + UShooterAttachment::GetMass();
	};
	void Push(UBulletCartridgeData* Cartridge) { if (Bullets.Num() < MaxCartridges) Bullets.Push(Cartridge); };
	UBulletCartridgeData* Pop() { return Bullets.Pop(); };
	float GetCurrentAmount() const { return Bullets.Num(); };
};

/** This Attatchment controls aspects such as:
* - Fall-off range
* - Accuracy
* - Damage
* - Attatchment capabilities
* - Handling
*/
UCLASS(Blueprintable)
class TREASUREDCOVE_API UBarrelAttachment : public UShooterAttachment
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

/** This Attatchment increases the stability and accuracy of 
* the weapon, in exchange for increasing the equip speed 
* and/or decreasing reload speed.
*/
UCLASS(Blueprintable)
class TREASUREDCOVE_API UStockAttachment : public UShooterAttachment
{
	GENERATED_BODY()

public:
	// float Stability;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float EquipSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float ADSMoveSpeed;

	// Stock Pad
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float ADSFocus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float ADSIdleSway;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float HorRecoilControl;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float VertRecoilControl;

	// Stock Comb
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float ADSSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float Versatility;
};

UCLASS(Blueprintable)
class TREASUREDCOVE_API UMuzzleAttachment : public UShooterAttachment
{
	GENERATED_BODY()

public:
};

/** This Attatchment controls aspects such as:
* - Accuracy
* - Handling
* - Equip speed
* - ADS speed
* - Recoil control
*/
UCLASS(Blueprintable)
class TREASUREDCOVE_API UGripAttachment : public UShooterAttachment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float ADSSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float SprintToFireSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float VertRecoilControl;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float HorRecoilControl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float FlinchResistance;
};

UCLASS(Blueprintable)
class TREASUREDCOVE_API UForegripAttachment : public UShooterAttachment
{
	GENERATED_BODY()

public:
};

