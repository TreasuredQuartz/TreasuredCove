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
class UAbilityInputConfigData;
class UGameplayAbilityBase;
class UItemMovementComponent;
class UPickupComponent;
class UEquippableComponent;
class UGAEnhancedInputComponent;
class AGAPlayerController;

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
	TArray< uint32 > CurrentActiveAbilityHandles;
	//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAActor|Animation", meta = (AllowPrivateAccess = true))
	UBillboardComponent* DominantHandOffset;
	//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAActor|Animation", meta = (AllowPrivateAccess = true))
	UBillboardComponent* SupportingHandOffset;

	// Whether the item is on the ground or a character has it on their person or in their inventory.
	uint8 bPickedUp:1;
	uint8 bEquipped:1;

public:	
	// Sets default values for this actor's properties
	AGAActor();

	// Override function from ability system interface
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// For animation blueprint dominant hand offset from default animation
	virtual FTransform GetHeldHandOffset() const;

	// For animation blueprint off hand offset from default animation
	virtual FTransform GetSupportingHandOffset() const;

	// For State Checking
	bool IsPickedUp() const { return bPickedUp; };

	// For State Checking
	bool IsEquipped() const { return bEquipped; };
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
	//
	void AbilityInputTagPressed(FGameplayTag Tag);
	//
	void AbilityInputTagReleased(FGameplayTag Tag);
	//
	void SetupPlayerAbilityInput(UGAEnhancedInputComponent* EIC, AGAPlayerController* PlayerController);
	//
	void RemovePlayerAbilityInput(UGAEnhancedInputComponent* EIC, AGAPlayerController* PlayerController);

	/** End AGActor Interface	*/


	/**	Begin IControlInput Interface */

	/*void UsePrimary_Implementation() override;
	void UseSecondary_Implementation() override;
	void UseThrow_Implementation() override;
	void UseMelee_Implementation() override;*/

	/** End IControlInput Interface	*/


	/** Begin Item Interface */

	UFUNCTION(meta = (DeprecatedFunction), BlueprintNativeEvent, Category = "Item")
	void OnPickedUp();
	virtual void OnPickedUp_Implementation();
	UFUNCTION(meta = (DeprecatedFunction), BlueprintNativeEvent, Category = "Item")
	void OnDropped();
	virtual void OnDropped_Implementation();
	UFUNCTION(meta = (DeprecatedFunction), BlueprintNativeEvent, Category = "Item")
	void OnEquipped();
	void OnEquipped_Implementation();
	UFUNCTION(meta=(DeprecatedFunction), BlueprintNativeEvent, Category = "Item")
	void OnUnEquipped();
	void OnUnEquipped_Implementation();

	/** End Item Interface */
public:
	// The array of abilities that we currently can use
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	class UAbilitySet* Abilities;

	// The Datatable to initialize our attribute sets from
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	UDataTable* InitialStatsTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	class UAbilityInputConfigData* InputAbilityActions;

public:
	// Root Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	// Abilities Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGASystemComponent* AbilitySystem;

	// Attribute Set Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TArray<TSubclassOf<UAttributeSet>> AttributeSetClasses;

	// Item Movement Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UItemMovementComponent* ItemMovement;

	// Pickup Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPickupComponent* Pickupable;

	// Item Movement Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UEquippableComponent* Equippable;

public:
	void LaunchItem(FVector LaunchVelocity, bool XYOveride, bool ZOveride);

	/** Let blueprint know that we were launched */
	UFUNCTION(BlueprintImplementableEvent)
	void OnLaunched(FVector LaunchVelocity, bool bXYOverride, bool bZOverride);
};
