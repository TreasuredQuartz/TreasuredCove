// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Datatable.h"
#include "ResourceInfoRowStruct.generated.h"

class UPhysicalMaterial;

/**
 * 
 */
USTRUCT(BlueprintType)
struct TREASUREDCOVE_API FResourceInfoRow : 
	public FTableRowBase // FName RowName : Already included
{
	GENERATED_BODY()

public:
	FResourceInfoRow();

public:
	// Representation of a resource in UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	UTexture2D* Icon;

	/** Representation of a resource in the 3D world.
	* Multiple meshes used for randomness.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	TArray<UStaticMesh*> Meshes;

	// Interaction of a resource in the 3D world.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	UPhysicalMaterial* PhysMaterial;
};
