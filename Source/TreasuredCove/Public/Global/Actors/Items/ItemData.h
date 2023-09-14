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
UCLASS(Blueprintable, EditInlineNew)
class TREASUREDCOVE_API UItemTypeData : public UObject
{
	GENERATED_BODY()

public:
	UItemTypeData() {};

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UItemCreator* Creator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UGASkillTree* ProficiencyTree;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	uint8 UseType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	uint8 AimType;
};

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew)
class TREASUREDCOVE_API UItemPopupData : public UObject
{
	GENERATED_BODY()

public:
	UItemPopupData();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FText CooldownOrCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FText WeaponType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FText AimType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FText Summary;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	TMap<FString, FText> ItemAttributeData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Credits", meta = (AllowPrivateAccess = "true"))
	TMap<FString, FText> Credits;

public:
	UFUNCTION(BlueprintCallable, Category="Item")
	void GetAttributeMap(TMap<FString, FText>& Attributes);

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AGAWeapon> Blueprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UItemTypeData* TypeData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UItemPopupData* PopupData;
};
