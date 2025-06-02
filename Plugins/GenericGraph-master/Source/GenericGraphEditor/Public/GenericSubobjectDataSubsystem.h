// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SubobjectDataSubsystem.h"
#include "GenericSubobjectDataSubsystem.generated.h"

struct FGenericSubobjectData;
struct FSubobjectDataHandle;
struct FSubobjectData;

/**
 * 
 */
UCLASS()
class GENERICGRAPHEDITOR_API UGenericSubobjectDataSubsystem : public USubobjectDataSubsystem
{
	GENERATED_BODY()

public:
	void GatherGenericSubobjectData(UObject* Context, TArray<FSubobjectDataHandle>& OutArray);
};
