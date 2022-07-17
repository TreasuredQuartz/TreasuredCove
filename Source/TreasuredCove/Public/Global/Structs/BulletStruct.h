// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BulletStruct.generated.h"

UENUM(BlueprintType)
enum class EBulletSize : uint8
{
	// Handgun
	Point22LongRifle,
	Point22Magnum,
	Point25ACP, 
	Point32ACP,
	Point380ACP,
	Point38Special,
	mm9NATO,
	Point357Magnum,
	Point45ACP,
	Point44Magnum,

	// Rifle
	Point223Remington,
	mm39x7Point62,
	Point30_30Winchester,
	Point308Winchester,
	R54x7Point62,
	Point30_06Springfield,

	// Vehicle
	Point50BMG,
	mm102x20,

	// Shotgun
	Guage12,
	Guage20,
	Guage28
};

UENUM(BlueprintType)
enum class EBulletJacket : uint8
{
	UnJacketed,
	Frangible,
	Bonded,
	Partitioned,
	Solid,
	FullMetalJacket,
};

UENUM(BlueprintType)
enum class EBulletHead : uint8
{
	ShotShellCartridge,
	HardHead,
	SoftHead,
	HollowPoint
};

USTRUCT(BlueprintType)
struct FBullet
{
	GENERATED_BODY()

	FBullet()
	{
		Size = EBulletSize::Point22LongRifle;
		Jacket = EBulletJacket::UnJacketed;
		Head = EBulletHead::ShotShellCartridge;
		Ammo = 0;
	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	EBulletSize Size;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	EBulletJacket Jacket;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	EBulletHead Head;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	float Ammo;

	FORCEINLINE bool operator == (const FBullet& OtherInfo) const
	{
		return Size == OtherInfo.Size && Jacket == OtherInfo.Jacket && Head == OtherInfo.Head;
	}
};