//

// Header files
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SkillInfoStruct.generated.h"

class UGameplayAbilityBase;

UENUM()
enum class EAbilityAquiredStatus : uint8
{
	Locked,
	Unlocked,
	Aquired,
};

USTRUCT(BlueprintType)
struct FSkillInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	FName AbilityName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	EAbilityAquiredStatus AquiredStatus;

	FSkillInfo() 
		: AbilityName(FName())
		, AquiredStatus(EAbilityAquiredStatus::Locked)
	{}
};

USTRUCT(BlueprintType)
struct FSkillInfoRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	FText Name;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	TSubclassOf<UGameplayAbilityBase> Class;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	UTexture2D* Texture;

public:
	FSkillInfoRow() 
		: Name(FText())
		, Class(nullptr)
		, Texture(nullptr)
	{}
};

USTRUCT(BlueprintType)
struct FGASkillTreeTier
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	EAbilityAquiredStatus AquiredStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	TArray<FSkillInfo> Skills;

public:
	FGASkillTreeTier() 
		: AquiredStatus(EAbilityAquiredStatus::Locked)
		, Skills()
	{}
};