// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "OnAttributeModifiedEvent.h"
#include "HealthComponent.generated.h"

class UAbilitySystemComponent;
class UASHealth;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthZeroed, const AActor*, Victim, const AActor*, ResponsibleActor);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	//
	virtual void OnUnregister() override;
	//
	void InitializeWithAbilitySystem(UAbilitySystemComponent* InASC);
	//
	void UninitializeFromAbilitySystem();

public:	
	// Called every frame /* Disabled. */
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Owner AbilitySystemComponent
	UAbilitySystemComponent* ASC;

	// Returns if the character has any health
	bool bIsHealthZeroed = false;
public:
	// The health attibutes
	UPROPERTY(VisibleAnywhere, Category = "Character|Attributes")
	TObjectPtr<const UASHealth> HealthSet;

	UPROPERTY(EditDefaultsOnly, Category = "Character|Attributes|Health|Tags")
	FGameplayTag FullHealthTag;

public:
	// Heal Modified Delegate
	UPROPERTY(BlueprintAssignable)
	FOnAttributeModifiedEvent_BP OnHealthModified;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeModifiedEvent_BP OnMaxHealthModified;

	UPROPERTY(BlueprintAssignable)
	FOnHealthZeroed OnHealthZeroed;

public:
	//
	UFUNCTION()
	void AddFullHealthTag() const;

	UFUNCTION()
	void RemoveFullHealthTag() const;

	UFUNCTION()
	void HandleHealthModified(const FOnAttributeModifiedPayload& Payload) const;

	UFUNCTION()
	void HandleMaxHealthModified(const FOnAttributeModifiedPayload& Payload) const;

	UFUNCTION()
	void HandleHealthZeroed(const FOnAttributeModifiedPayload& Payload) const;

	UFUNCTION()
	bool GetIsHealthZeroed() const { return bIsHealthZeroed; };
};
