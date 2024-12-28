// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Global/Enumerations/Emotion.h"
#include "Global/Enumerations/Language.h"
#include "Global/Enumerations/Race.h"
#include "Global/Enumerations/Species.h"
#include "Global/Enumerations/Relationship.h"

#include "DialogueComponent.generated.h"

class UDlgDialogue;

USTRUCT(BlueprintType, Blueprintable)
struct FDialogueData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FName, int32> Integers;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FName, int32> Floats;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FName, FName> Names;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TSet<FName> TrueBools;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UDialogueComponent 
	: public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDialogueComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	// This name will be what is referrenced in the Dialogue Editor; 
	// Use ParticipantDisplayName for what will be used in the UI.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Dialogue")
	FName ParticipantName = "Player";

	// Structure to help pass information between the dialogue and the character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Dialogue")
	FDialogueData DialogueData;

	// This is the in-game name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Dialogue")
	FText ParticipantDisplayName;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Dialogue")
	FText ParticipantState;

	//--- Please remember to update this to a Material Interface if necessary ---//
	// The UI representation of this character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Dialogue")
	UTexture2D* ParticipantIcon;

	// The sub-species designation of the creature
	// EX: Frost-Walker Goblin; Dark Elf; Asian Human
	// Note: In the case of a second species, race is ignored entirely
	// EX: Goblish Human; Dwarvish Elf; Cyborg Orc
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Social")
	ERace Race;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Social")
	ESpecies Species;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Social")
	TArray<ELanguage> KnownLanguages;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Social")
	TArray<EEmotion> EmotionalState;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Social")
	TMap<APawn*, ERelationship> Relationships;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Social")
	UDlgDialogue* Dialogue;
};
