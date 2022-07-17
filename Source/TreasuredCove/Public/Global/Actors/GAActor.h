// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Abilities/GameplayAbility.h"

#include "AbilitySystemInterface.h"
#include "InteractionInterface.h"
#include "ControlInputInterface.h"

#include "GAActor.generated.h"

class UGASystemComponent;
class UGameplayAbilityBase;

/** This Class serves a dual purpose:
* - Reduce Complexity of AGACharacter.
* - Allow Items (Weapons in mind) to have an ability system.
*		AGAActor
*		/	   \
*	Weapon	 Character
*/
UCLASS()
class TREASUREDCOVE_API AGAActor : 
	public AActor,
	public IAbilitySystemInterface,
	public IControlInputInterface,
	public IInteractionInterface
{
	GENERATED_BODY()
	
private:
	// These are the handles to the actual abilities 
	// that will be given to us by the ability system
	TArray< FGameplayAbilitySpecHandle > CurrentActiveAbilityHandles;
public:	
	// Sets default values for this actor's properties
	AGAActor();

	// Override function from ability system interface
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
protected:
	/**	Begin UObject Interface	*/

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called after components have been initialized (InEditor and BeginPlay, after OnConstruction)
	virtual void PostInitializeComponents() override;

	/** End UObject Interface	*/



	/**	Begin AGActor Interface */

	// Called to Initialize the ability system
	void IntializeAbilitySystem();
	// Called to iniailize an attribute set's stats 
	void InitializeAttributeSet(UAttributeSet* Set);
	// Called to grant an ability to our ability system
	void AquireAbility(TSubclassOf<UGameplayAbility> InAbility, FGameplayAbilitySpecHandle& OutHandle);

	/** End AGActor Interface	*/
protected:
	// The array of abilities that we currently can use
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
	TArray<TSubclassOf<UGameplayAbilityBase>> CurrentActiveAbilities;

	// The Datatable to initialize our attribute sets from
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
	UDataTable* InitialStatsTable;

public:
	// Abilities Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UGASystemComponent* AbilitySystem;

	// Attribute Set Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UAttributeSet>> AttributeSetClasses;
};
