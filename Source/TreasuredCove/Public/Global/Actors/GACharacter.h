// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"

#include "Global/Interfaces/InteractionInterface.h"
#include "Global/Interfaces/RepositoryInterface.h"
#include "Global/Interfaces/InventoryInterface.h"

#include "DlgSystem/DlgDialogueParticipant.h"
#include "TownSystem/TownSystemInterface.h"
#include "InputActionValue.h"

#include "Global/Components/Characters/DialogueComponent.h"

#include "SenseSysHelpers.h"

#include "Global/Enumerations/Element.h"
#include "Global/Enumerations/TownJob.h"
#include "Global/Enumerations/WarPartyRole.h"
#include "Global/Enumerations/WeaponType.h"
#include "Global/Enumerations/AbilityType.h"
#include "Global/Enumerations/AttributeType.h"
#include "Global/Enumerations/NavType.h"

#include "Global/Structs/SavedAttribute.h"
#include "Global/Structs/GAActiveLoadoutStruct.h"
#include "Global/Structs/AICharacterInfoStruct.h"
#include "Global/Structs/ItemKeyStruct.h"

#include "GACharacter.generated.h"

#pragma region ForwardDeclarations
class AGAActor;
class AGAWeapon;
class AGAPlayerController;
class AGAAIController;
class UGAInputConfigData;
class UAbilityInputConfigData;
class AGravityWidget;
class UAbilitySet;
class UGAWidget;
class UGASystemComponent;
class UGASkillTreeComponent;
class UTownSystemComponent;

class AGameplayBuilding;

class APhysicsConstraintActor;
class UGameplayAbilityBase;
class UInputMappingContext;

class UComboComponent;
class UCraftingComponent;
class UFloatingBarComponent;
class UFloatingTextComponent;
class UFloatingItemInfoComponent;
class UFootprintComponent;
class UTimelineComponent;
class UGACharacterMovementComponent;
class UHealthComponent;
class UInventoryComponent;
class UEquipmentComponent;
class ULaunchingComponent;
class UMovementTrailComponent;
class UPickupMagnetComponent;
class URepositoryComponent;
class USenseReceiverComponent;
class USenseStimulusComponent;
class USensorBase;

class UParticleSystemComponent;
class UProceduralMeshComponent;
class UPhysicalAnimationComponent;
class UWidgetComponent;

UENUM(BlueprintType)
enum class EAdvantageType : uint8
{
	Advantage,
	Neutral,
	Disadvantage
};

UENUM(BlueprintType)
enum class EHeldInputType : uint8
{
	Primary,
	Secondary,
	Switch,
	Throw,
	Melee,
	Interact,
	Ultimate
};
#pragma endregion

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGAEnded, const UGameplayAbility*, AbilityEndedData);

UCLASS(Blueprintable)
class TREASUREDCOVE_API AGACharacter
	: public ACharacter,
	public IAbilitySystemInterface,
	public IInteractionInterface,
	public IDlgDialogueParticipant,
	public ITownSystemInterface,
	public IRepositoryInterface,
	public IInventoryInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGACharacter(const FObjectInitializer& ObjectInitializer);

	// Override function from ability system interface
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// Override function from town system interface
	UTownSystemComponent* GetTownSystemComponent() const override;
	// Override function from repository interface
	URepositoryComponent* GetRepositoryComponent_Implementation() const override;
	// Override function from inventory interface
	UInventoryComponent* GetInventoryComponent_Implementation() const override;
	// Helper function for Character Movment Component
	UGACharacterMovementComponent* GetGACharacterMovement() const;
	// Helper function for Skill Tree Component
	UGASkillTreeComponent* GetSkillTreeComponent() const;

#pragma region PrivateMembers
private:
	uint8 bInitialized:1;
	uint8 bIsPaused:1;
	UPROPERTY()
	uint8 bShouldUpdateConstructionScript:1;
	uint8 bShouldUpdatePhysicalAnimation:1;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	uint8 bIsDead:1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	uint8 bCanJump:1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	uint8 bCanSlide:1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	uint8 bCanWallRun:1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	uint8 bCanClimb:1;

	uint8 bShadowed:1;

	// int MaxJumps; JumpMaxCount		Character Movement Component Variables
	// int CurJumps; JumpCurrentCount	Character Movement Component Variables

	float Scale;
	float HoldTime;
	float ReactionTime;
	float CameraTiltTime;
	float ForwardAxisValue;
	float RightAxisValue;
	float HitReactionTimeRemaining;

	FRotator ViewRotation;

	FTimerHandle WallRunTimerHandle;
	FTimerHandle CameraTiltTimerHandle;
	FTimerHandle PhysicalAnimationHandle;
	FTimerHandle MenuUpTimerHandle;
	FTimerHandle MenuDownTimerHandle;
	FTimerHandle MenuRightTimerHandle;
	FTimerHandle MenuLeftTimerHandle;
	FTimerHandle InputTimerHandle;

	AActor* Sun;

	AGAPlayerController* PC;
	AGAAIController* AC;
#pragma endregion

#pragma region ProtectedMembers
protected:
	uint8 TeamID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Run")
	UCurveFloat* WallRunTimelineCurve;

	//////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
	UAbilitySet* InitialActiveAbilities;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
	UAbilitySet* InitialPassiveAbilities;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
	FGAActivatedInfo SwitchAbility;

	TSubclassOf<UGameplayAbility> DecreaseSpreadAbility;
	TSubclassOf<UGameplayAbility> IncreaseSpreadAbility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
	UDataTable* InitialStatsTable;

	UPROPERTY(replicated)
	TArray< FGameplayAbilitySpecHandle > ActiveAbilityHandles;
	TArray< FGameplayAbilitySpecHandle > PassiveHandles;

	FGameplayAbilitySpecHandle SwitchAbilityHandle;
	FGameplayAbilitySpecHandle DecreaseSpreadAbilityHandle;
	FGameplayAbilitySpecHandle IncreaseSpreadAbilityHandle;

	//////////////////////////////////////////////////////

protected:
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* SlideMontage;
#pragma endregion

#pragma region InputHandling
protected:
	// Enhanced Player Input Mapping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputMappingContext* InputMapping;

	// Enhanced Unarmed Input Mapping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputMappingContext* Unarmed_InputMapping;

	// Input Configuration Data
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UGAInputConfigData* InputActions;

	// Unarmed Input Configuration Data
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UAbilityInputConfigData* InputAbilityActions;

	// In single player terms,
	// All AI Controlled characters are seperated from
	// Player controlled ones.
	void AutoDetermineTeamIDByControllerType();
	// 
	void BeginQuickSelect();
	// 
	void EndQuickSelect();
	//
	bool ShouldPrioritizeWeapon() const;
	// 
	void BeginPrimary();
	// 
	void EndPrimary();
	// 
	void BeginSecondary();
	// 
	void EndSecondary();
	// 
	void Switch(float Val);
	// 
	void BeginSwitch();
	// 
	void EndSwitch();
	// 
	void BeginThrow();
	// 
	void EndThrow();
	// 
	void BeginMelee();
	// 
	void EndMelee();
	// 
	void BeginInteract();
	// 
	void EndInteract();
	// 
	void BeginUltimate();
	// 
	void EndUltimate();

	//////////////////////////////////////////////////////
public:
	// 
	void MenuUp(float Val);
	// 
	void MenuRight(float Val);
	// 
	void ReturnToMainMenu();
	// 
	void BeginPause();
	// 
	void EndPause();
	// 
	UFUNCTION()
	void StartHeldInputTimer(EHeldInputType InputType);
private:
	//////////////////////////////////////////////////////

	// Called when player-controlled from WASD or controller right analog
	void Move(const FInputActionValue& Value);
	// Called when player-controlled from the mouse or controller left analog
	void Look(const FInputActionValue& Value);
	// Called when player-controlled from the space bar or controller bottom face button
	void Jump_Input(const FInputActionValue& Value);
	// Called when player-controlled from the ctrl key or controller right face button
	void Crouch_Input(const FInputActionValue& Value);
	// Called when player-controlled from the shift key or controller analog press
	void Sprint_Input(const FInputActionValue& Value);
	// Called when player-controlled from the combination of crouching during sprint
	void Slide_Input(const FInputActionValue& Value);
	// Called when player-controlled from the F key or controller top face button
	void Switch_Input(const FInputActionValue& Value);

	UFUNCTION()
	void AbilityInputTagPressed(FGameplayTag Tag);
	UFUNCTION()
	void AbilityInputTagReleased(FGameplayTag Tag);

	// Called when "MoveForawrd" passed a value
	void MoveForward(float Val);
	// Called when "MoveRight" passed a value
	void MoveRight(float Val);
	// Called when "LookUp" is passed a value
	void LookUp(float Val);
	// Called when "LookRight" is passed a value
	void LookRight(float Val);

	// Called when "Jump" is pressed
	void BeginJump();
	// Called when "Jump" is released
	void EndJump();
	// Called when "Run" is pressed
	void BeginSprint();
	// Called when "Run" is released
	void EndSprint();
	// Called when "Crouch" is pressed
	void BeginCrouch();
	// Called when "Crouch" is released
	void EndCrouch();

	// Called to update AI Sight component
	void UpdateSightRotation();
#pragma endregion

#pragma region VisualFX
public:
	// 
	void BeginGrip();
	// 
	void SwapGrip();
	// 
	void EndGrip();
	// Called when "Crouch" is pressed whilst Sprinting from MovementComponent
	void OnBeginSlide();
	// Called when "Crouch" is released, or velocity is low enough, from MovementComponent
	void OnEndSlide();
	// Called from MovementComponent when conditions are meet when hitting a wall 
	void OnBeginWallRun();
	// Called from MovementComponent when no longer wall running
	void OnEndWallRun();
	// Called from Tick to clamp velocity
	void ClampHorizontalVelocity();
	// Visual effect only for wall running rn
	void BeginCameraTilt();
	// Visual effect only for wall running rn
	UFUNCTION()
	void UpdateCameraTilt(float progress);
	// Visual effect only for wall running rn
	void EndCameraTilt();

	UFUNCTION()
	FTransform GetPawnSenseTransform();

#pragma endregion

#pragma region UserPreferrenceSettings
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	bool bToggleCrouch = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	bool bToggleSprint = false;

	bool bIsUsingController = false;
	uint8 LookUpperLimit = 80;
	int8 InvertUpDown = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	float VerticalLookRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	float HorizontalLookRate;
	float DeltaTime;
#pragma endregion

#pragma region LegacyMembers
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	FName HeadSocket;
	FRotator SightInitialRotation;
	// Dominant Hand Constraint
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|Locomotion")
	APhysicsConstraintActor* GripConstraint_d;
	// Submissive Hand Constraint
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|Locomotion")
	APhysicsConstraintActor* GripConstraint_s;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation|Locomotion", meta = (AllowPrivateAccess = "true"))
	EWeaponType HeldWeaponType;
#pragma endregion

	// Attributes to save
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
	TArray<FSavedAttribute> AttributesToSave;

#pragma region Components
public:
	//
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UProceduralMeshComponent* RealMesh;*/

	// Sight Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USenseReceiverComponent* AISenses;

	// Stimulus Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USenseStimulusComponent* AIStimulus;

	// Abilities Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UGASystemComponent* RenamedAbilitySystem;

	// Skill trees component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|Abilities", meta = (AllowPrivateAccess = "true"))
	UGASkillTreeComponent* SkillTrees;
	
	// Attribute Set Classes to create components from
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|Abilities", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UAttributeSet>> AttributeSetClasses;
	// Attribute Set Components
	TArray<UAttributeSet*> AttributeSets;

	// Combo System Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UComboComponent* ComboSystem;

	// Launching Component (For Throwing, Pushing, etc)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	ULaunchingComponent* LaunchingComponent;

	// Inventory Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* Inventory;

	// Repository Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	URepositoryComponent* Repository;

	// Equipment Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UEquipmentComponent* Equipment;

	// Item Magnet Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPickupMagnetComponent* Magnet;

	// Crafting Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCraftingComponent* Crafting;

	// Dialogue Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDialogueComponent* DialogueParticipantInfo;

	// Town System Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UTownSystemComponent* TownInfo;

	// Physical Animation Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPhysicalAnimationComponent* PhysicalAnimation;

	// Camera Animation Timeline Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UTimelineComponent* CameraTiltTimeline;

	// Movement Trail Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UMovementTrailComponent* MovementTrail;

	// On Damaged Particle System Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* PSPuncture;

	// On Damaged Numbers (maybe extend to add Dialogue?)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UFloatingTextComponent* FloatingTextComponent;

	// On Damaged Health bar (maybe extend for more attributes?)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UFloatingBarComponent* FloatingBarComponent;

	// While looking at a valid pickup, present info about it
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UFloatingItemInfoComponent* FloatingItemInfoComponent;

	// Spawn Hints for tracking players and visual indicators for hearing footsteps
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UFootprintComponent* FootprintComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UHealthComponent* HealthComponent;

	//////////////////////////////////////////////////////
#pragma endregion

#pragma region AnimationChecks
public:
	/** Animation Settings */

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation|Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsWalking = false;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation|Locomotion", meta = (AllowPrivateAccess = "true"))
	// bool bIsCrouching = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation|Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsSprinting = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation|Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsSliding = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation|Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsWallRunning = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation|Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsClimbing = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation|Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsDominantGripping = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation|Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsSubmisiveGripping = false;

#pragma endregion

	// Data Table for attribute set
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities") 
	// UDataTable* AttrDataTable;

#pragma region ObjectDefaultFunctions
public:
	/** Begin Object Interface		*/
#if WITH_EDITOR
	// Called in editor and on begin play
	virtual void OnConstruction(const FTransform& Transform) override;
	// Called by editor for changes made to UPROPERTYs in TArrays
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e) override;
#endif
	/**	End Object Interface		*/
#pragma endregion

#pragma region ActorDefaultFunctions
public:
	/** Begin Actor Interface		*/

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	void Tick(float InDeltaTime) override;

	/**	End Actor Interface			*/
#pragma endregion

#pragma region PawnDefaultFunctions
public:
	/** Begin Pawn Interface		*/

	// Called after all components have been initialized
	virtual void PostInitializeComponents() override;
	// Called when possessed by a new controller
	virtual void PossessedBy(AController* NewController) override;
	// Called when control leaves this pawn (Typically to a vehicle or another character)
	virtual void UnPossessed() override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Called on clients when controller is replicated
	virtual void OnRep_Controller() override;
	//
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	/**	End Pawn Interface			*/
#pragma endregion

#pragma region CharacterDefaultFunctions
public:
	/* Begin Character Interface	*/

	// Called when attempting to jump. Overridden to allow jump while crouched
	virtual bool CanJumpInternal_Implementation() const override;
	// Called after successfully validating a jump then performing one
	virtual void OnJumped_Implementation() override;

	// Called from Movement Component to correct Mesh location to match capsule.
	virtual void OnStartCrouch(float, float) override;
	// Called from Movement Component to correct Mesh location to match capsule.
	virtual void OnEndCrouch(float, float) override;

	/* End Character Interface		*/
#pragma endregion

#pragma region DefaultFunctions
public:
	// Returns whether this character has been unpossessed by virtue of losing all Health Points.
	UFUNCTION(BlueprintPure, Category="Character")
	bool GetIsDead() const { return bIsDead; };
	//
	bool CanSprint() const;
	//
	void Sprint();
	//
	void UnSprint();

	// Intended for temporary disabling of wall run
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	void SetWallRunAllowed(bool bAllowed) { bCanWallRun = bAllowed; }
	// Checks if wall run is allowed
	bool CanWallRun() const { return bCanWallRun; }
	// Intended for temporary disabling of climb
	UFUNCTION(BlueprintCallable, Category="Character|Movement")
	void SetClimbAllowed(bool bAllowed) { bCanClimb = bAllowed; }
	// Checks if Climb is allowed
	bool CanClimb() const { return bCanClimb; }
	// Intended for temporary disabling of jump
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	void SetJumpAllowed(bool bAllowed) { bCanJump = bAllowed; }
	// Checks if Slide is allowed
	bool CanSlide() const { return bCanSlide; }
	// Intended for temporary disabling of slide
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	void SetSlideAllowed(bool bAllowed) { bCanSlide = bAllowed; }
#pragma endregion

#pragma region AbilitySystem
	/** Begin Ability Interface		*/
public:
	// 
	void InitializeAbilitySystem();
	//
	void AquireAbility(TSubclassOf<UGameplayAbility> InAbility, FGameplayAbilitySpecHandle &OutHandle);
	// 
	void AddAbilityToUI(TSubclassOf<UGameplayAbilityBase> InAbility, EAbilityType AbilityType, FGameplayAbilitySpecHandle InHandle, bool bFromItem);
	// Calls AquireAbility on every ability passed in array
	void AquireAbilities(TArray<TSubclassOf<UGameplayAbility>> InAbilities, TArray< FGameplayAbilitySpecHandle > &OutHandles);
	//
	UFUNCTION()
	void AquirePassives(TArray<TSubclassOf<UGameplayAbility>> InPassives);
	UFUNCTION()
	void AquirePassive(TSubclassOf<UGameplayAbility> InPassive);
	void RemoveAbility(FGameplayAbilitySpecHandle InHandle);
	void RemoveAbilityFromUI(FGameplayAbilitySpecHandle InHandle);
	void RemoveAbilities(TArray< FGameplayAbilitySpecHandle > InHandles);
	// Used to add gameplay tags to this character
	UFUNCTION(BlueprintCallable, Category = "Character|Abilities|Tags")
	void AddGameplayTag(const FGameplayTag& NewTag);
	// Used to remove gameplay tags from this character
	UFUNCTION(BlueprintCallable, Category = "Character|Abilities|Tags")
	void RemoveGameplayTag(const FGameplayTag& OldTag);
	UFUNCTION()
	void AbilityEnded(const FAbilityEndedData& AbilityEndedData);
	UPROPERTY(BlueprintAssignable)
	FGAEnded OnAbilityEnded;
#pragma region Attributes
	/** Attributes */
public:
	// 
	void InitializeAttributeSet(UAttributeSet* Set);
	//
	virtual bool GetSavedAttributesCurrentValues(TArray<FSavedAttribute>& OutAttributes);
	virtual void PopulateSavedAttributes(const TArray<FSavedAttribute>& Attributes);
	bool AttributeCheck(FGameplayAttribute TestAttribute, uint8 Difficulty, EAdvantageType Advantage = EAdvantageType::Neutral);

	// Called when Health Attribute is <= 0, 
	// will call blueprint version.
	UFUNCTION()
	void Death(const AActor* ResponsibleActor, const AActor* Victim);
	// Called internally when health attribute <= 0
	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
	void OnDeath();

	UFUNCTION()
	void OnStaminaModified(float Stamina, float MaxStamina);
	UFUNCTION()
	void OnManaModified(float Mana, float MaxMana);
	// void OnMaxModified(float MaxHealth, float MaxStamina, float MaxMana);
	void OnAmmoModified(float Ammo, float MaxAmmo);
	void OnSpreadAngleModified(float SpreadAngle);
	void OnHipAccuracyModified(float SpreadMin, float SpreadMax, float SpreadDecreaseSpeed, float SpreadIncreaseSpeed, float SpreadDeltaMultipiler);
	void OnADSAccuracyModified(float Sway, float HorizontalRecoil, float VerticalRecoil);
	void OnHandlingModified(float ReloadSpeed, float DrawSpeed, float ADSSpeed);
	void OnRangeModified(float MinRange, float MaxRange);
	void OnDamageModified(float MinDamage, float MaxDamage);
	void OnCoreWeaponStatsModified(float Cooldown, float Cost, float CastTime);
	void OnUltimateModified(float UltimateCharge, float MaxUltimateCharge);
	void OnExperienceModified(float OldExperience, float NewExperience);
	void OnStatModified(float Charisma, float Constitution, float Dexterity, float Intelligence, float Strength, float Wisdom);

	/** End Attributes */
#pragma endregion

	/** End Ability Interface		*/
#pragma endregion

#pragma region ItemSystem
	// Members
public:
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	float ItemDropRadius;

	/** Begin Interaction Interface	*/
	//
	virtual void NotifyCanInteract_Implementation(FName InteractibleName, bool CanPickup) override;
	virtual void InteractedWith_Implementation(AActor* OtherActor) override;
	virtual void AddItemToInventory_Implementation(AGAActor* Item) override; 
	virtual void AddItemToRepository_Implementation(const FString& Category, const FItemKey& Item) override;

	/** Equipment Interface */

	UFUNCTION(Blueprintpure, BlueprintCallable, Category = "Inventory")
	AGAActor* GetHeldItem() const;
	UFUNCTION(Blueprintpure, BlueprintCallable, Category = "Inventory")
	AGAActor* GetStowedItem() const;

	/** Inventory Interface */
	
	void AddPickupPrompt();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PickupItem(AActor* Item);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void EquipItem(AActor* Item);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void StowItem(AActor* Item);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SwitchEquippedItems();
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropEquippedItem(const FVector& DropVelocity);

	void EquipItemFromInventory(uint8 Slot);
	void StowItemFromInventory(uint8 Slot);
	void AddEquippedItemToInventory();
	void AddStowedItemToInventory();
	void MoveItemWithinInventory(uint8 From, uint8 To);
	void DropItemFromInventory(uint8 Slot);

	/** Repository Interface */

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Repository")
	bool GetAllItems(const FString Category, TArray<FItemKey>& Items) const;

	void AddItemRepository(const FString Category, const FItemKey& Item);
	UFUNCTION(BlueprintCallable, Category = "Repository")
	void AddItemRepository(const FString Category, const FName Name, const uint8 Quantity);

	void SubtractItemRepository(const FString Category, const FItemKey& Item);
	UFUNCTION(BlueprintCallable, Category = "Repository")
	void SubtractItemRepository(const FString Category, const FName Name, const uint8 Quantity);
	UFUNCTION(BlueprintCallable, Category = "Repository")
	void SubtractItemAtIndexRepository(const FString Category, const uint8 Index, const uint8 Quantity);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Repository")
	bool GetItemQuantityFromRepository(const FString Category, const int32 Index, int32& Quantity) const;
	void SpawnItemFromRepository();
#pragma endregion

#pragma region DialogueSystem
	/** Dialogue System Interface */
public:
	virtual FName GetParticipantName_Implementation() const override { return DialogueParticipantInfo->ParticipantName; };
	virtual FText GetParticipantDisplayName_Implementation(FName ActiveSpeaker) const override { return DialogueParticipantInfo->ParticipantDisplayName; };
	virtual UTexture2D* GetParticipantIcon_Implementation(FName ActiveSpeaker, FName ActiveSpeakerState) const override { return DialogueParticipantInfo->ParticipantIcon; };

	
	 virtual bool ModifyIntValue_Implementation(const FName ValueName, bool bDelta, int32 Value) override;
	 virtual bool ModifyFloatValue_Implementation(const FName ValueName, bool bDelta, float Value) override;
	 virtual bool ModifyBoolValue_Implementation(const FName ValueName, bool bValue) override;
	 virtual bool ModifyNameValue_Implementation(const FName ValueName, const FName NameValue) override;

	virtual int32 GetIntValue_Implementation(const FName ValueName) const override;
	virtual float GetFloatValue_Implementation(const FName ValueName) const override;
	virtual bool GetBoolValue_Implementation(const FName ValueName) const override;
	virtual FName GetNameValue_Implementation(const FName ValueName) const override;

	virtual bool OnDialogueEvent_Implementation(UDlgContext* Context, const FName EventName) override { return false; };
	virtual bool CheckCondition_Implementation(const UDlgContext* Context, FName ConditionName) const override { return false; };
#pragma endregion

#pragma region AIHandling
public:
	//
	UPROPERTY()
	FAICharacterInfo AIInfo;
	//
	UFUNCTION(BlueprintPure)
	FAICharacterStats GetAIStats() { return FAICharacterStats(AIInfo.CurrentAITargetStats); }
	//
	UFUNCTION()
	void OnSensed(const USensorBase* Sensor, int32 Channel, EOnSenseEvent SenseEvent);
	UFUNCTION()
	void OnHeard();
	UFUNCTION()
	void OnSeen();

	UFUNCTION()
	void CheckVisibleCharacters();
	UFUNCTION(BlueprintCallable, Category = "Character|Abilities|Combat")
	bool IsOtherHostile(AGACharacter* Other);
	UFUNCTION()
	void CalculateShadowed();
	UFUNCTION()
	void UpdateDesiredLocation(FVector DesiredLocation);
	UFUNCTION()
	void UpdateCurrentBuilding(AGameplayBuilding* CurrentBuilding);
	UFUNCTION()
	void UpdateTargetBuilding(AGameplayBuilding* TargetBuilding);
#pragma endregion

#pragma region Combat
	/** Combat */
public:
	//
	UFUNCTION(BlueprintCallable, Category = "Character|Abilities|Combat")
	void SimulateHit(const FHitResult& Hit, const FVector& ImpactForce);
	//
	UFUNCTION()
	void SimulatePhysicalAnimation();
	// 
	UFUNCTION(BlueprintCallable, Category = "Character|Abilities|Combat")
	void SpawnNumbers(float Numbers);
	// 
	UFUNCTION(BlueprintCallable, Category = "Character|Abilities|Combat")
	void SpawnValueBar(EAttributeType BarType, float CurrentValue, float MaxValue);
	//
	UFUNCTION(BlueprintCallable, Category = "Character|Abilities|Combat")
	void Puncture(FVector ImpactLocation, FVector ImpactNormal, FVector ImpactExitPoint, float ImpactRadius, FVector ImpactForce);

	uint8 GetTeamID() const;
#pragma endregion
};
