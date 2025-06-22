// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RarityComponent.generated.h"

UENUM(BlueprintType)
enum class ERarityType : uint8
{
	Common,
	Uncommon,
	Unusual,
	Scarce,
	Rare,
	Epic,
	Mythic,
	Legendary,
	Relic,
	Artifact,
	Unique,
	MAX
};

USTRUCT(BlueprintType)
struct FRarityDatabaseRow
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Rarity")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Rarity")
	FLinearColor Color;
};

/** A Container intended to be instanced and filled with data
*	partaining to common rarity indicators.
*/
UCLASS(Blueprintable)
class TREASUREDCOVE_API URarityDatabase : public UDataAsset
{
	GENERATED_BODY()

#define INVALID_NAME FText::FromString("Invalid Name")
#define INVALID_COLOR FLinearColor(1, 0.25, 0.75, 1)

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FRarityDatabaseRow> Rows;

public:
	UFUNCTION(BlueprintCallable, Category = "Item|Rarity")
	FText GetRarityName(uint8 InRarity) { return !Rows.IsEmpty() && Rows.IsValidIndex(InRarity) ? Rows[InRarity].DisplayName : INVALID_NAME; };
	UFUNCTION(BlueprintCallable, Category = "Item|Rarity")
	FLinearColor GetRarityColor(uint8 InRarity) { return !Rows.IsEmpty() && Rows.IsValidIndex(InRarity) ? Rows[InRarity].Color : INVALID_COLOR; };
};

/** A Component used to hold a value that represents the "rarity"
* of the actor (usually an item). 
* 
* Rarity has an associated name and color for User Experience 
* purposes.
* 
* This attribute normally affects other aspects, such as:
* - spawn rate
* - crafting difficulty
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API URarityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URarityComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Rarity", Meta = (AllowPrivateAccess = "true"))
	ERarityType Rarity = ERarityType::Common;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Rarity", Meta = (AllowPrivateAccess = "true"))
	URarityDatabase* Database;

protected:
	// Called when the game starts
	// virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
