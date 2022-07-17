// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemKeyStruct.generated.h"

USTRUCT(BlueprintType)
struct TREASUREDCOVE_API FItemKey
{
	GENERATED_BODY()
	
	FItemKey() : Name(FName()), Quantity(1) {}

	FItemKey(FName inName, int32 inQuantity) : Name(inName), Quantity(inQuantity) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Quantity;

	// Compares only the name members
	FORCEINLINE bool operator == (const FItemKey& OtherInfo) const
	{
		return this->Name == OtherInfo.Name;
	}
	// Compares only the name members
	FORCEINLINE bool operator != (const FItemKey& OtherInfo) const
	{
		return this->Name != OtherInfo.Name;
	}
	// Returns a FItemKey that has the Quantity members Added to each other
	FORCEINLINE FItemKey operator + (const FItemKey& OtherInfo) const
	{
		ensure(this->Name == OtherInfo.Name);
		FItemKey Item = FItemKey(this->Name, Quantity + OtherInfo.Quantity);
		return Item;
	}
	// Returns a FItemKey that has the Quantity members subtracted from each other
	FORCEINLINE FItemKey operator - (const FItemKey& OtherInfo) const
	{
		ensure(this->Name == OtherInfo.Name);
		FItemKey Item = FItemKey(this->Name, Quantity - OtherInfo.Quantity);
		return Item;
	}
	// Performs the addition operator on the Quantity member
	FORCEINLINE FItemKey& operator += (const FItemKey& OtherInfo)
	{
		ensure(this->Name == OtherInfo.Name);
		this->Quantity = (this->Quantity + OtherInfo.Quantity);
		return *this;
	}
	// Performs the subtraction operator on the Quantity member
	FORCEINLINE FItemKey& operator -= (const FItemKey& OtherInfo)
	{
		ensure(this->Name == OtherInfo.Name);
		this->Quantity = (this->Quantity - OtherInfo.Quantity);
		return *this;
	}
};
