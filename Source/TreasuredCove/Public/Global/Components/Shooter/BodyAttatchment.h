// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Global/Components/Shooter/ShooterAttatchment.h"
#include "Global/Structs/CustomUnits.h"
#include "BodyAttatchment.generated.h"

typedef FRPM rpm;
enum class TREASUREDCOVE_API EActionCycleState : uint8
{
	None,
	Feeding,
	Chambering,
	Locking,
	Firing,
	Unlocking,
	Etracting,
	Ejecting,
	Cocking,
	MAX,
};

/**
 *
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API UBoltAttatchment : public UShooterAttatchment
{
	GENERATED_BODY()

public:
};

/** 
 *
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API USlideAttatchment : public UShooterAttatchment
{
	GENERATED_BODY()

public:
};

/**
 *
 */
UCLASS(Abstract, BlueprintType)
class TREASUREDCOVE_API UGunAction : public UShooterAttatchment
{
	GENERATED_BODY()

public:
	EActionCycleState CurrentState;

	// Directly equivellant to fire rate; represents the time required to perform
	// functions required to fire another bullet.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float CycleRate;

	UFUNCTION(BlueprintCallable)
	virtual float GetCycleRate() const { return CycleRate; };

	// A cartridge being stripped from the magazine
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	// float FeedingSpeed;
	// 
	// The cartridge being placed into the barrel chamber
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	//float ChamberingSpeed;
	// 
	//// The bolt/breech face/cylinder is locked into position with the bore/barrrel.
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	//float LockingSpeed;
	// 
	//// This point the gun would stay in a state awaiting for the trigger to fire.
	//// A trigger press rotates the sear, which sends the firing pin or striker against the 
	//// primer, causing ignition of the powder.
	//// This results in gas expansion within the chambered cartridge which propels the
	//// projectile into and through the barrel bore.
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	//float FiringSpeed;
	// 
	//// The backward movement of the bolt or slide/breech face unseating the cartridge case 
	//// from the chamber and engaging the disconnector.
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	//float UnlockingSpeed;
	// 
	//// Pull the cartridge case from the chamber.
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	//float ExtractionSpeed;
	// 
	//// Ejects the cartridge case through the ejection port.
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	//float EjectionSpeed;
	// 
	//// The hammer or striker is engaged by the sear and ready for the resumption 
	//// of operations.
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	//float CockingSpeed;
	// 
	// Ejects the cartridge case through the ejection port.
};

/**
 *
 */
UCLASS(BlueprintType)
class TREASUREDCOVE_API UGunSingleAction : public UGunAction
{
	GENERATED_BODY()

public:
};

/**
 *
 */
UCLASS(BlueprintType)
class TREASUREDCOVE_API UGunDoubleAction : public UGunAction
{
	GENERATED_BODY()

public:
};

/**
 *
 */
UCLASS(BlueprintType)
class TREASUREDCOVE_API UGunBreakAction : public UGunAction
{
	GENERATED_BODY()

public:
};

/**
 *
 */
UCLASS(BlueprintType)
class TREASUREDCOVE_API UGunBoltAction : public UGunAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	UBoltAttatchment* Bolt;
};

/**
 *
 */
UCLASS(BlueprintType)
class TREASUREDCOVE_API UGunLeverAction : public UGunAction
{
	GENERATED_BODY()

public:
};

/**
 *
 */
UCLASS(BlueprintType)
class TREASUREDCOVE_API UGunSemiAutomaticAction : public UGunAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	USlideAttatchment* Slide;
};

/**
 *
 */
UCLASS(BlueprintType)
class TREASUREDCOVE_API UGunFullyAutomaticAction : public UGunAction
{
	GENERATED_BODY()

public:
};

/**
 * 
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API UBodyAttatchment : public UShooterAttatchment
{
	GENERATED_BODY()
	
public:
	// Firing Pin
	// Action determines firing rate.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	UGunAction* Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	FRPM FireRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	float BurstFireSpeed;

	// 0 -> Automatic
	// 1 -> Semi-Automatic
	// 2 -> 2-Burst
	// 3 -> 3-Burst
	// Etc.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter")
	uint8 BurstFireAmount;

};
