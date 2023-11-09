// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "HealthComponent.generated.h"

USTRUCT(BlueprintType)
struct FOnHealthDamagedResult
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RemainingHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DamagedBoneName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Victim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* ResponsibleActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDamageType* DamageType;

public:
	FOnHealthDamagedResult() 
		: RemainingHealth(0.0f)
		, DamageAmount(0.0f)
		, DamagedBoneName(FName("None"))
		, Victim(nullptr)
		, ResponsibleActor(nullptr)
		, DamageType(nullptr)
	{};

	FOnHealthDamagedResult(float InRemainingHealth, float InDamageAmount, FName InBoneName, 
		AActor* InVictim, AActor* InResponsibleActor, UDamageType* InDamageType)
		: RemainingHealth(InRemainingHealth)
		, DamageAmount(InDamageAmount)
		, DamagedBoneName(InBoneName)
		, Victim(InVictim)
		, ResponsibleActor(InResponsibleActor)
		, DamageType(InDamageType)
	{};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthDamaged, const FOnHealthDamagedResult&, OnHealthDamagedResult);
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

public:	
	// Called every frame /* Disabled. */
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// Owner AbilitySystemComponent
	class UAbilitySystemComponent* ASC;

	// The health attibutes
	UPROPERTY(VisibleAnywhere, Category = "Character|Attributes")
	class UASHealth* HealthSet;

	UPROPERTY(EditDefaultsOnly, Category = "Character|Attributes|Health|Tags")
	FGameplayTag FullHealthTag;

	// Damaged Delegate
	UPROPERTY(BlueprintAssignable)
	FOnHealthDamaged OnHealthDamaged;

	// Death Delegate
	UPROPERTY(BlueprintAssignable)
	FOnHealthZeroed OnDeath;

public:
	//
	UFUNCTION()
	void AddFullHealthTag() const;

	UFUNCTION()
	void RemoveFullHealthTag() const;

	UFUNCTION()
	void DamageHealth(const FOnHealthDamagedResult& Result) const;

	UFUNCTION()
	void Die(const AActor* Victim, const AActor* ResponsibleActor) const;
};
