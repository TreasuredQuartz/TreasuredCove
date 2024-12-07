// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "OnAttributeModifiedEvent.h"
#include "AttributeComponent.generated.h"

class UAttributeSet;
class UAttributeSetInfo;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeZeroed, const AActor*, Victim, const AActor*, ResponsibleActor);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called when owner is deleted or this component is removed.
	virtual void OnUnregister() override;
	// Assigns internal Ability System to the assed in component. Children can then add their own Attribute Sets and assign delegates.
	void InitializeWithAbilitySystem(UAbilitySystemComponent* InASC);
	// Sets the ability system to NULL and allows children to clear delegates or clean up pointers etc.
	void UninitializeFromAbilitySystem();

protected:
	// Owner AbilitySystemComponent
	UAbilitySystemComponent* ASC;

	// Returns if the character has any health
	bool bIsZeroed = false;

public:
	// The health attibutes
	UPROPERTY(VisibleAnywhere, Category = "Character|Attributes")
	TSubclassOf<UAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, Category = "Character|Attributes|Tags")
	FGameplayTag FullAttributeTag;

	UPROPERTY(EditDefaultsOnly, Category = "Character|Attributes|Tags")
	TObjectPtr<UAttributeSetInfo> AttributeInfo;
public:
	// Heal Modified Delegate
	UPROPERTY(BlueprintAssignable)
	FOnAttributeModifiedEvent_BP OnAttributeModified;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeModifiedEvent_BP OnMaxAttributeModified;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeZeroed OnAttributeZeroed;

public:
	//
	UFUNCTION()
	void AddFullAttributeTag() const;

	UFUNCTION()
	void RemoveFullAttributeTag() const;

	UFUNCTION()
	void HandleAttributeModified(const FOnAttributeModifiedPayload& Payload) const;

	UFUNCTION()
	void HandleMaxAttributeModified(const FOnAttributeModifiedPayload& Payload) const;

	UFUNCTION()
	void HandleAttributeZeroed(const FOnAttributeModifiedPayload& Payload);

	UFUNCTION()
	bool GetIsAttributeZeroed() const { return bIsZeroed; };

public:
	// Abstract function: Perform initialization after assigned new Ability System. Such as adding an Attribute Set and assigning delegates.
	virtual void OnInitialized() {};
	// Abstract function: Remove delegates, clean up pointers, and clean up any other features at this point.
	virtual void OnUninitialized() {};
	// Abstract function: Called from HandleAttributeModified(), allows children to call AddFullAttributeTag() and RemoveFullAttributeTag() when appropriate.
	virtual void OnModified(const FOnAttributeModifiedPayload& Payload) const {};
	// Abstract function: Returns a UI friendly display name of this component. Not intended as the full list of attributes.
	UFUNCTION(BlueprintCallable)
	virtual FText GetComponentDisplayName() const { return FText(); };
	// Abstract function: Returns a UI friendly description of this component.
	UFUNCTION(BlueprintCallable)
	virtual FText GetComponentDescription() const { return FText(); };
	// Abstract function: Returns a UI friendly value that represents a summary of the attributes on this component.
	UFUNCTION(BlueprintCallable)
	virtual float GetComponentSummaryValue() const { return 0.0f; };
	// Abstract function: Returns a UI friendly value that represents a summary of the attributes on this component.
	UFUNCTION(BlueprintCallable)
	virtual UAttributeSetInfo* GetComponentAttributeValues() const { return AttributeInfo; };
};
