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
class UAbilitySet;
class UGameplayAbilityBase;
class UProjectileMovementComponent;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAActor|Animation", meta = (AllowPrivateAccess = true))
	UBillboardComponent* DominantHandOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAActor|Animation", meta = (AllowPrivateAccess = true))
	UBillboardComponent* SupportingHandOffset;

public:	
	// Sets default values for this actor's properties
	AGAActor();

	// Override function from ability system interface
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// For animation blueprint dominant hand offset from default animation
	virtual FTransform GetHeldHandOffset() const;

	// For animation blueprint off hand offset from default animation
	virtual FTransform GetSupportingHandOffset() const;

public:
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
	// Array version of AquireAbility
	void AquireAbilities(const TArray<TSubclassOf<UGameplayAbility>>& InAbilities, TArray<FGameplayAbilitySpecHandle>& OutHandles);

	/** End AGActor Interface	*/


	/**	Begin IControlInput Interface */

	void UsePrimary_Implementation() override;
	void UseSecondary_Implementation() override;
	void UseThrow_Implementation() override;
	void UseMelee_Implementation() override;

	/** End IControlInput Interface	*/


	/** Begin Item Interface */

	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
	void OnPickedUp();
	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
	void OnDropped();

	/** End Item Interface */
protected:
	// The array of abilities that we currently can use
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	UAbilitySet* InitialActiveAbilities;

	// The Datatable to initialize our attribute sets from
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	UDataTable* InitialStatsTable;

public:
	// Root Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

	// Abilities Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UGASystemComponent* AbilitySystem;

	// Attribute Set Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UAttributeSet>> AttributeSetClasses;

	// Projectile Movement Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
};
