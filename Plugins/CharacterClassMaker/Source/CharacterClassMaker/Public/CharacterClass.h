// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CharacterClass.generated.h"

/**
 *
 */
UCLASS()
class CHARACTERCLASSMAKERRUNTIME_API UFeature : public UObject
{
	GENERATED_BODY()
private:
	FText DisplayName;

public:
	UFeature() {};

	FORCEINLINE FText GetDisplayName() const { return DisplayName; };
};

/**
 *
 */
USTRUCT(BlueprintType)
struct CHARACTERCLASSMAKERRUNTIME_API FFeatureList 
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Features")
	TArray<UFeature*> Features;

	FFeatureList() {};
};

/**
 *
 */
UCLASS()
class CHARACTERCLASSMAKERRUNTIME_API UProficiency : public UObject
{
	GENERATED_BODY()

public:
	UProficiency() {};
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class CHARACTERCLASSMAKERRUNTIME_API UCharacterClass : public UObject
{
	GENERATED_BODY()

public:
	UCharacterClass() {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	TArray<UProficiency*> SupportedProficiencies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	TMap<int32, FFeatureList> LevelUpFeatures;
};
