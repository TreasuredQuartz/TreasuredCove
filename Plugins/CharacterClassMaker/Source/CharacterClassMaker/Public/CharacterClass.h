// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CharacterClass.generated.h"

USTRUCT(BlueprintType)
struct CHARACTERCLASSMAKERRUNTIME_API FFeatureList {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Features")
	TArray<FName> Features;

	FFeatureList() {};
};

/**
 * 
 */
UCLASS()
class CHARACTERCLASSMAKERRUNTIME_API UCharacterClass : public UObject
{
	GENERATED_BODY()

public:
	UCharacterClass();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillTrees")
	TArray<FName> SupportedSkillTrees;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillTrees")
	TMap<int32, FFeatureList> LevelUpFeatures;
};
