// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemData.generated.h"

class UItemAttributeData;
class UGASkillTree;
class UItemCreator;

enum class EItemType : uint8
{
	None,
};

/**
*
*/
UCLASS()
class TREASUREDCOVE_API UItemData : public UObject
{
	GENERATED_BODY()

public:
	UItemData() {};

public:
	virtual TMap<FString, FText> GetPopupProperties() const { return TMap<FString, FText>(); };
};

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew)
class TREASUREDCOVE_API UItemTypeData : public UItemData
{
	GENERATED_BODY()

public:
	UItemTypeData() {};

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UGASkillTree* ProficiencyTree;

	TMap<FString, FText> GetPopupProperties() const override;
};

/** The 3D representation of the item. 
* While a mesh would be the expected, items can
* Be constructed by players. A necessary range 
* Is provided when required.
*/
UCLASS()
class TREASUREDCOVE_API UItemPhysicalData : public UItemData
{
	GENERATED_BODY()

public:
	UItemPhysicalData() {};

public:
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* GetMesh();

	float Mass;
};

/** How the item is handled in combat. Stats,
* Combos, Skills, Proficiency and anything else
* Related would be included here.
*/
UCLASS()
class TREASUREDCOVE_API UItemCombatData : public UItemData
{
	GENERATED_BODY()
public:


};

/** Information regarding the creation of the item;
* Such as a specific player, company, or any other 
* Type of attribution would go here.
*/
UCLASS()
class TREASUREDCOVE_API UItemCreditData : public UItemData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CreatorName;

	TMap<FString, FText> GetPopupProperties() const override;
};

