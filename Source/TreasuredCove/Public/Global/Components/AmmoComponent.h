// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeComponent.h"
#include "GameplayTagContainer.h"
#include "Global/AbilitySystem/AttributeSets/OnAttributeModifiedEvent.h"
#include "AmmoComponent.generated.h"

class UAbilitySystemComponent;
class UASAmmo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoZeroed, const AActor*, Victim, const AActor*, ResponsibleActor);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UAmmoComponent : public UAttributeComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAmmoComponent();

protected:
	//
	virtual void OnInitialized() override;
	//
	virtual void OnUninitialized() override;

public:
	// The health attibutes
	UPROPERTY(VisibleAnywhere, Category = "Character|Attributes")
	TObjectPtr<const UASAmmo> AmmoSet;

};
