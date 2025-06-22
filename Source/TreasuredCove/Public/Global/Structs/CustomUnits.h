// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomUnits.generated.h"

/**
 * Numeric interface that specifies how to interact with a number in a specific unit.
 * Include NumericUnitTypeInterface.inl for symbol definitions.
 * /
template<typename NumericType>
struct FGrains : TNumericUnitTypeInterface<NumericType>
{
	/** The underlying units which the numeric type are specified in. * /
	const EUnit UnderlyingUnits;

	/** Optional units that this type interface will be fixed on * /
	TOptional<EUnit> FixedDisplayUnits;

	/** Constructor * /
	TNumericUnitTypeInterface(EUnit InUnits);

	/** Convert this type to a string * /
	virtual FString ToString(const NumericType& Value) const override;

	/** Attempt to parse a numeral with our units from the specified string. * /
	virtual TOptional<NumericType> FromString(const FString& ValueString, const NumericType& InExistingValue) override;

	/** Check whether the specified typed character is valid * /
	virtual bool IsCharacterValid(TCHAR InChar) const override;

	/** Set up this interface to use a fixed display unit based on the specified value * /
	void SetupFixedDisplay(const NumericType& InValue);
};

/**
 *
 */
USTRUCT(BlueprintType)
struct TREASUREDCOVE_API FCustomUnits
{
	GENERATED_BODY()

public:
	FCustomUnits() {}

	FCustomUnits(float NewValue)
		: Value(NewValue)
	{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Units")
	float Value;

	/*********************/
	/* Begin Mathematics */
	/*********************/

	// Addition
	FORCEINLINE float operator + (const FCustomUnits& OtherInfo) const
	{
		return Value + OtherInfo.Value;
	}
	// Subtraction
	FORCEINLINE float operator - (const FCustomUnits& OtherInfo) const
	{
		return Value - OtherInfo.Value;
	}
	// Multiplication
	FORCEINLINE float operator * (const FCustomUnits& OtherInfo) const
	{
		return Value * OtherInfo.Value;
	}
	// Division
	FORCEINLINE float operator / (const FCustomUnits& OtherInfo) const
	{
		return Value / OtherInfo.Value;
	}

	// floats

	// Addition
	FORCEINLINE float operator + (float other) const
	{
		return Value + other;
	}
	// Subtraction
	FORCEINLINE float operator - (float other) const
	{
		return Value - other;
	}
	// Multiplication
	FORCEINLINE float operator * (float other) const
	{
		return Value * other;
	}
	// Division
	FORCEINLINE float operator / (float other) const
	{
		return Value / other;
	}

	/*********************/
	/** End Mathematics **/
	/*********************/

	/********************/
	/* Begin Equalities */
	/********************/

	// Equals
	FORCEINLINE bool operator == (const FCustomUnits& OtherInfo) const
	{
		return Value == OtherInfo.Value;
	}
	// Does not equal
	FORCEINLINE bool operator != (const FCustomUnits& OtherInfo) const
	{
		return Value != OtherInfo.Value;
	}
	// Less than or equal
	FORCEINLINE bool operator <= (const FCustomUnits& OtherInfo) const
	{
		return Value <= OtherInfo.Value;
	}
	// More than or equal
	FORCEINLINE bool operator >= (const FCustomUnits& OtherInfo) const
	{
		return Value >= OtherInfo.Value;
	}
	// Less than
	FORCEINLINE bool operator < (const FCustomUnits & OtherInfo) const
	{
		return Value < OtherInfo.Value;
	}
	// More than
	FORCEINLINE bool operator > (const FCustomUnits& OtherInfo) const
	{
		return Value > OtherInfo.Value;
	}

	/********************/
	/** End Equalities **/
	/********************/
};

/**
 *
 */
USTRUCT()
struct TREASUREDCOVE_API FGrains : public FCustomUnits
{
	GENERATED_BODY()

public:

};

/**
 *
 */
USTRUCT()
struct TREASUREDCOVE_API FRPM : public FCustomUnits
{
	GENERATED_BODY()

public:

};
