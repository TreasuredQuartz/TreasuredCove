// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Organ.generated.h"

class UOrganSystem;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UOrgan : public UObject
{
	GENERATED_BODY()
	
private:


public:
	virtual void Initialize(UOrganSystem* InParentSystem = nullptr);
};
