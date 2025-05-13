// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Global/AbilitySystem/AttributeSets/Components/AttributeComponent.h"
#include "ManaComponent.generated.h"

class UASMana;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UManaComponent : public UAttributeComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UManaComponent();

protected:
	//
	virtual void OnInitialized() override;
	//
	virtual void OnUninitialized() override;
	//
	virtual void OnModified(const FOnAttributeModifiedPayload& Payload) const override;
	// Abstract function: Returns a UI friendly display name of this component. Not intended as the full list of attributes.
	virtual FText GetComponentDisplayName() const override;
	// Abstract function: Returns a UI friendly description of this component.
	virtual FText GetComponentDescription() const override;
	// Abstract function: Returns a UI friendly value that represents a summary of the attributes on this component.
	virtual float GetComponentSummaryValue() const override;

public:
	// The Mana attibutes
	UPROPERTY(VisibleAnywhere, Category = "Character|Attributes")
	TObjectPtr<const UASMana> ManaSet;
	
};
