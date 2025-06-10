// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GenericSubobjectDataHandle.generated.h"

struct FGenericSubobjectData;

/**
* A subobject handle is a globally unique identifier for subobjects
* Upon construction, the handle will be invalid. It is the responsibility
* of the owning FSubobjectData to set the DataPtr once the subobject
* data has validated that it has a good context.
*/
USTRUCT(BlueprintType)
struct GENERICGRAPHEDITOR_API FGenericSubobjectDataHandle
{
	friend struct FGenericSubobjectData;
	friend class UGenericSubobjectDataSubsystem;

	GENERATED_USTRUCT_BODY();

	explicit FGenericSubobjectDataHandle()
		: DataPtr(nullptr)
	{}

	~FGenericSubobjectDataHandle()
	{
		DataPtr = nullptr;
	}

	/**
	* True if the Handle is valid (i.e. not INDEX_NONE). This is true once GenerateNewHandle is called
	*
	* @return bool		True if this handle is valid
	*/
	inline bool IsValid() const { return DataPtr != nullptr; }

	bool operator==(const FGenericSubobjectDataHandle& Other) const
	{
		return DataPtr.Get() == Other.DataPtr.Get();
	}

	bool operator!=(const FGenericSubobjectDataHandle& Other) const
	{
		return DataPtr != Other.DataPtr;
	}

	/** Returns a pointer to the subobject data that this is a handle for */
	inline TSharedPtr<FGenericSubobjectData> GetSharedDataPtr() const { return DataPtr; }
	inline FGenericSubobjectData* GetData() const { return DataPtr.IsValid() ? DataPtr.Get() : nullptr; }
	// void SetData(TSharedPtr<FGenericSubobjectData> NewData) { DataPtr = NewData; }

	/**
	* A static representation of an invalid handle. Mostly used as a return
	* value for functions that need to return a handle but have the risk of being invalid.
	*/
	static const FGenericSubobjectDataHandle InvalidHandle;

	/** Get the hash code to use for the given FSubobjectDataHandle */
	friend uint32 GetTypeHash(const FGenericSubobjectDataHandle& Key)
	{
		return PointerHash(Key.DataPtr.Get());
	}

private:
	/** Pointer to the actual subobject data that this handle represents */
	TSharedPtr<FGenericSubobjectData> DataPtr;
};

template<>
struct TStructOpsTypeTraits<FGenericSubobjectDataHandle> : public TStructOpsTypeTraitsBase2<FGenericSubobjectDataHandle>
{
	enum
	{
		WithIdenticalViaEquality = true,
	};
};
