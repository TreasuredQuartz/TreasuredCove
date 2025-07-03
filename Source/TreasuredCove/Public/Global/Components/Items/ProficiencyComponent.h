// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProficiencyComponent.generated.h"

UCLASS(NotBlueprintable)
class UProficiencyType : public UDataAsset
{
	GENERATED_BODY()

public:
};

USTRUCT(BlueprintType)
struct FProficiencyDatabaseRow
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Proficiency")
	FText DisplayName;
};

/** A Container intended to be instanced and filled with data
*	partaining to common rarity indicators.
*/
UCLASS(Blueprintable)
class TREASUREDCOVE_API UProficiencyDatabase : public UDataAsset
{
	GENERATED_BODY()

#define INVALID_NAME FText::FromString("Invalid Name")

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<UProficiencyType*, FProficiencyDatabaseRow> Rows;

public:
	UFUNCTION(BlueprintCallable, Category = "Item|Proficiency")
	FText GetProficiencyName(UProficiencyType* InProficiency) { return !Rows.IsEmpty() && Rows.Contains(InProficiency) ? Rows[InProficiency].DisplayName : INVALID_NAME; };
};

/** A Component used to hold a value that represents the 
* "proficiency" of the actor regarding an item (usually a 
* weapon).
*
* Proficiency has an associated name for User Experience
* purposes.
*
* This attribute normally affects item specific aspects, 
* such as:
* - handling
* - accuracy
* - reload speed
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UProficiencyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProficiencyComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Proficiency")
	UProficiencyDatabase* Database;

	// The Proficiency that the owner of this component is associated with.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Proficiency")
	UProficiencyType* ProficiencyType;
};

/** A Component that stores the relations between proficiency
* types and the amount of proficiency that an actor has with
* that type (usually a character with an item).
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TREASUREDCOVE_API UProficientComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UProficientComponent();

	// The Proficiencies that the owner of this component is associated with.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Proficiency")
	TMap<UProficiencyType*, int32> Proficiencies;
};
