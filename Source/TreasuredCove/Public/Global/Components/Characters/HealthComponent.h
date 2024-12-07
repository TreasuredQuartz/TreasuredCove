// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Global/Components/AttributeComponent.h"
#include "HealthComponent.generated.h"

class UASHealth;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UHealthComponent : public UAttributeComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

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
	// The health attibutes
	UPROPERTY(VisibleAnywhere, Category = "Character|Attributes")
	TObjectPtr<const UASHealth> HealthSet;

};
