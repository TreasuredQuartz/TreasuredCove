// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "InteractionInterface.h"
#include "RepositoryInterface.h"
#include "DlgDialogueParticipant.h"
#include "TownSystemInterface.h"
#include "GameFramework/Character.h"
#include "Abilities/GameplayAbility.h"

#include "DialogueComponent.h"

#include "Element.h"
#include "TownJob.h"
#include "WarPartyRole.h"
#include "WeaponType.h"
#include "AbilityType.h"
#include "AttributeType.h"
#include "NavType.h"

#include "Components/TimelineComponent.h"
#include "SavedAttribute.h"
#include "GAActiveLoadoutStruct.h"
#include "AICharacterInfoStruct.h"
#include "ItemKeyStruct.h"

#include "GACharacter.generated.h"

#pragma region ForwardDeclarations
class AGAActor;
class AGAPlayerController;
class AGAAIController;
class AGravityWidget;
class UAbilitySet;
class UGAWidget;
class UGASystemComponent;
class UGASkillTreeComponent;
class UTownSystemComponent;

class AGameplayBuilding;

class APhysicsConstraintActor;
class UGameplayAbilityBase;

class UComboComponent;
class UCraftingComponent;
class UInventoryComponent;
class URepositoryComponent;
class UPickupMagnetComponent;
class USenseComponent;
class UGACharacterMovementComponent;
class UFloatingTextComponent;

class UParticleSystemComponent;
class UProceduralMeshComponent;

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

UCLASS(Blueprintable)
class TREASUREDCOVE_API AGACharacter
	: public ACharacter,
	public IAbilitySystemInterface,
	public IInteractionInterface,
	public IDlgDialogueParticipant,
	public ITownSystemInterface,
	public IRepositoryInterface
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
	// Helper function for Character Movment Component
	UGACharacterMovementComponent* GetGAMovementComponent() const;
	// Helper function for Skill Tree Component
	UGASkillTreeComponent* GetSkillTreeComponent() const;

#pragma region PrivateMembers
private:
	bool bInitialized = false;
	bool bIsPaused = false;
	UPROPERTY()
	bool bShouldUpdateConstructionScript = false;
	UPROPERTY()
	bool bIsDead = false;
	bool bCanEverWallRun = true;
	bool bCanWallRun = true;
	bool bCanEverClimb = true;
	bool bCanClimb = true;
	bool bCanEverJump = true;
	bool bCanJump = true;

	uint8 Shadowed : 1;

	// int MaxJumps; JumpMaxCount		Character Movement Component Variables
	// int CurJumps; JumpCurrentCount	Character Movement Component Variables

	float Scale;
	float HoldTime;
	float ReactionTime;
	float CameraTiltTime;
	float ForwardAxisValue;
	float RightAxisValue;

	FTimerHandle WallRunTimerHandle;
	FTimerHandle CameraTiltTimerHandle;

	FTimerHandle MenuUpTimerHandle;
	FTimerHandle MenuDownTimerHandle;
	FTimerHandle MenuRightTimerHandle;
	FTimerHandle MenuLeftTimerHandle;

	FTimerHandle InputTimerHandle;

	FTimeline WallRunCameraTiltTimeline;

	AGAPlayerController* PC;
	AGAAIController* AC;
	AActor* Sun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* MeshInsideColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AGravityWidget> OnDamagedNumbersWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AGravityWidget> OnDamagedHealthBarClass;

	AGravityWidget* OnDamagedHealthBar;

	FVector WallRunDirection;
	ENavType WallRunSide;

	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<int32> Tris;

	TArray<int> MaxActiveMenuSlots;
	TArray<int> MaxSubMenuSlots;
	TArray<FAICharacterInfo> AwareOfCharacters;
#pragma endregion

	// Private and unfinshed
	void InitProceduralMesh();
	// Private and unfinshed
	void UpdateProceduralMesh();

#pragma region ProtectedMembers
protected:
	uint8 TeamID;
	uint8 PreviousActiveMenuIndex;
	uint8 ActiveMenuCount;
	uint8 SubMenuCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Run")
	UCurveFloat* WallRunTimelineCurve;

	//////////////////////////////////////////////////////

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
	TSubclassOf<UAbilitySet> InitialActiveAbilitySetClass;
	UAbilitySet* InitialActiveAbilities;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
	FGAActivatedInfo SwitchAbility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
	TArray< TSubclassOf<UGameplayAbilityBase> > Passives;

	TSubclassOf<UGameplayAbility> DecreaseSpreadAbility;
	TSubclassOf<UGameplayAbility> IncreaseSpreadAbility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
	UDataTable* InitialStatsTable;

	TArray< FGameplayAbilitySpecHandle > ActiveAbilityHandles;

	FGameplayAbilitySpecHandle SwitchAbilityHandle;
	FGameplayAbilitySpecHandle DecreaseSpreadAbilityHandle;
	FGameplayAbilitySpecHandle IncreaseSpreadAbilityHandle;

	//////////////////////////////////////////////////////

public:
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Inventory|Equipment")
	AGAActor* HeldItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Inventory|Equipment")
	AGAActor* StowedItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Inventory|Equipment")
	AGAActor* Armor;

	//////////////////////////////////////////////////////

protected:
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* SlideMontage;
#pragma endregion

#pragma region InputHandling
	// In single player terms,
	// All AI Controlled characters are seperated from
	// Player controlled ones.
	void AutoDetermineTeamIDByControllerType();
	// 
	void BeginQuickSelect();
	// 
	void EndQuickSelect();
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

	// 
	void MenuUp(float Val);
	// 
	void MenuRight(float Val);
	// 
	UFUNCTION(BlueprintCallable)
	void MenuUpDelayed();
	// 
	UFUNCTION(BlueprintCallable)
	void MenuDownDelayed();
	// 
	UFUNCTION(BlueprintCallable)
	void MenuRightDelayed();
	// 
	UFUNCTION(BlueprintCallable)
	void MenuLeftDelayed();
	// 
	void ReturnToMainMenu();
	// 
	void BeginPause();
	// 
	void EndPause();
	// 
	UFUNCTION()
	void StartHeldInputTimer(EHeldInputType InputType);

	//////////////////////////////////////////////////////

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
	// Called when "Crouch" is pressed whilst Sprinting
	void BeginSlide();
	// Called when "Crouch" is released or velocity is low enough
	void EndSlide();
	// Called when "MoveForawrd" passed a value
	void MoveForward(float Val);
	// Called when "MoveRight" passed a value
	void MoveRight(float Val);
	// Called when "LookUp" is passed a value
	void LookUp(float Val);
	// Called when "LookRight" is passed a value
	void LookRight(float Val);
	// Called to update AI Sight component
	void UpdateSightRotation();
#pragma endregion

	// 
	void BeginGrip();
	// 
	void SwapGrip();
	// 
	void EndGrip();
	// 
	void OnBeginWallRun();
	// 
	void OnEndWallRun();
	// 
	void ClampHorizontalVelocity();
	// Visual effect only for wall running rn
	void BeginCameraTilt();
	// Visual effect only for wall running rn
	UFUNCTION()
	void UpdateCameraTilt();
	// Visual effect only for wall running rn
	void EndCameraTilt();
	// Called when Health Attribute is <= 0
	void Death();
	// Called internally when health attribute <= 0
	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
	void OnDeath();
	UFUNCTION()
	FTransform GetPawnSenseTransform();

#pragma region UserPreferrenceSettings
public:
	//
	bool bIsUsingController = false;
	bool bWeaponPriority = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	bool bFirstPerson = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	bool bToggleCrouch = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	bool bToggleQuickSelect = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	bool bToggleSprint = false;

	uint8 LookUpperLimit = 80;
	int8 InvertUpDown = -1;

	float DeltaTime;
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Locomotion", meta = (AllowPrivateAccess = "true"))
		float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Locomotion", meta = (AllowPrivateAccess = "true"))
		float SprintSpeed;*/
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	float VerticalLookRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	float HorizontalLookRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Settings", meta = (AllowPrivateAccess = "true"))
	FName DominantHand;
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

#pragma region GameplayTagMembers
public:
	/** Gameplay Tags */
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities|Tags")
	FGameplayTag FullHealthTag;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities|Tags")
	FGameplayTag FullAmmoTag;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities|Tags")
	FGameplayTag FullManaTag;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities|Tags")
	FGameplayTag FullStaminaTag;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities|Tags")
	FGameplayTag FullExperienceTag;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities|Tags")
	FGameplayTag FullUltimateTag;
#pragma endregion

	// Attributes to save
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
	TArray<FSavedAttribute> AttributesToSave;

#pragma region Components
public:
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Interaction")
	AActor* CurrentInteractItem;

	//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UProceduralMeshComponent* RealMesh;

	// Sight Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USenseComponent* Sight;

	// Abilities Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UGASystemComponent* RenamedAbilitySystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities", meta = (AllowPrivateAccess = "true"))
	UGASkillTreeComponent* SkillTrees;
	
	// Attribute Set Classes to create components from
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UAttributeSet>> AttributeSetClasses;
	// Attribute Set Components
	TArray<UAttributeSet*> AttributeSets;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UComboComponent* ComboSystem;

	// Inventory Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* Inventory;

	// Repository Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	URepositoryComponent* Repository;

	// Magnet Component
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

	// Material Trail Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* PSTrail;

	// On Damaged Particle System Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* PSPuncture;

	//
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UFloatingTextComponent* FloatingTextComponent;

	//////////////////////////////////////////////////////
#pragma endregion

#pragma region AnimationChecks
public:
	/** Animation Settings */

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation|Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsWalking = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation|Locomotion", meta = (AllowPrivateAccess = "true"))
	bool bIsCrouching = false;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation|IK", meta = (AllowPrivateAccess = "true"))
	float IKFootTraceDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation|IK", meta = (AllowPrivateAccess = "true"))
	float IKOffsetRightFoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation|IK", meta = (AllowPrivateAccess = "true"))
	float IKOffsetLeftFoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation|IK", meta = (AllowPrivateAccess = "true"))
	float IKOffsetRightHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation|IK", meta = (AllowPrivateAccess = "true"))
	float IKOffsetLeftHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Animation|IK", meta = (AllowPrivateAccess = "true"))
	float IKInterpSpeed;

	/** Begin IK Helper Functions	*/
	//
	float IKFootTrace(float Distance, FName Bone);
	float IKHandToLocation(FVector Location, FName Bone);
	float IKFingerTrace(float Distance, FName Bone);
#pragma endregion

	// Data Table for attribute set
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities") 
	// UDataTable* AttrDataTable;

#pragma region ActorDefaultFunctions
public:
	/** Begin Actor Interface		*/

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	void Tick(float InDeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Called when possessed by a new controller
	virtual void PossessedBy(AController* NewController) override;
	// Called when control leaves this pawn (Typically to a vehicle or another character)
	virtual void UnPossessed() override;
	// Called on clients when controller is replicated
	virtual void OnRep_Controller() override;

#if WITH_EDITOR
	// Called in editor and on begin play
	virtual void OnConstruction(const FTransform& Transform) override;
	// Called after all components have been initialized
	virtual void PostInitializeComponents() override;
	// Called by editor for changes made to UPROPERTYs in TArrays
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e) override;
#endif

	/**	End Actor Interface			*/
#pragma endregion

#pragma region DefaultFunctions
	// Intended for temporary disabling of wall run
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	void SetWallRunAllowed(bool bAllowed) { bCanWallRun = bAllowed; }
	// Checks if wall run is allowed
	bool CanWallRun() const { return bCanEverWallRun && bCanWallRun; }
	// Intended for temporary disabling of wall run
	UFUNCTION(BlueprintCallable, Category="Character|Movement")
	void SetClimbAllowed(bool bAllowed) { bCanClimb = bAllowed; }
	// Checks if Climb is allowed
	bool CanClimb() const { return bCanEverClimb && bCanClimb; }
	// Intended for temporary disabling of wall run
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	void SetJumpAllowed(bool bAllowed) { bCanJump = bAllowed; }
	// Checks if jump is allowed
	bool CanJump() const { return bCanEverJump && bCanJump; }
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
	void RemoveAbility(FGameplayAbilitySpecHandle InHandle);
	void RemoveAbilityFromUI(FGameplayAbilitySpecHandle InHandle);
	void RemoveAbilities(TArray< FGameplayAbilitySpecHandle > InHandles);
	// Used to add gameplay tags to this character
	UFUNCTION(BlueprintCallable, Category = "Character|Abilities|Tags")
	void AddGameplayTag(const FGameplayTag& NewTag);
	// Used to remove gameplay tags from this character
	UFUNCTION(BlueprintCallable, Category = "Character|Abilities|Tags")
	void RemoveGameplayTag(const FGameplayTag& OldTag);
#pragma region Attributes
	/** Attributes */
public:
	// 
	void InitializeAttributeSet(UAttributeSet* Set);
	//
	virtual bool GetSavedAttributesCurrentValues(TArray<FSavedAttribute>& OutAttributes);
	virtual void PopulateSavedAttributes(const TArray<FSavedAttribute>& Attributes);
	bool AttributeCheck(FGameplayAttribute TestAttribute, uint8 Difficulty, EAdvantageType Advantage = EAdvantageType::Neutral);
	//
	UFUNCTION()
	void OnHealthModified(float Health, float MaxHealth);
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
	virtual void NotifyCanPickup_Implementation(AActor* Item, bool CanPickup) override;
	virtual void InteractedWith_Implementation(AActor* OtherActor) override;
	virtual void AddItemToInventory_Implementation(AGAActor* Item) override; 
	virtual void AddItemToRepository_Implementation(const FString& Category, const FItemKey& Item) override;

	/** Inventory Interface */

	AActor* GetHeldItem();
	AActor* GetStowedItem();
	
	void AddPickupPrompt();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PickupItem(AGAActor* Item);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void EquipItem(AGAActor* Item);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void StowItem(AGAActor* Item);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SwitchEquippedItems();
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropEquippedItem();

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
	void OnDamaged(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue);
	UFUNCTION()
	void OnHealed(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue);
	//
	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);
	//
	UFUNCTION()
	void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);
	void CheckVisibleCharacters();
	UFUNCTION(BlueprintCallable, Category = "Character|Abilities|Combat")
	bool IsOtherHostile(AGACharacter* Other);
	UFUNCTION()
	void CalculateShadowed();
	UFUNCTION()
	void UpdateDesiredLocation(FVector DesiredLocation);
	void UpdateCurrentBuilding(AGameplayBuilding* CurrentBuilding);
	void UpdateTargetBuilding(AGameplayBuilding* TargetBuilding);
#pragma endregion

#pragma region Combat
	/** Combat */
public:
	// 
	UFUNCTION(BlueprintCallable, Category = "Character|Abilities|Combat")
	void SpawnNumbers(float Numbers);
	// 
	UFUNCTION(BlueprintCallable, Category = "Character|Abilities|Combat")
	void SpawnValueBar(EAttributeType BarType, float CurrentValue, float MaxValue);
	// 
	UFUNCTION(BlueprintCallable, Category = "Character|Abilities|Combat")
	int DetermineActiveMenuSelection(int MaxMenuCount, int Direction);
	// 
	UFUNCTION(BlueprintCallable, Category = "Character|Abilities|Combat")
	void ResetActiveMenuSelection();
	// 
	UFUNCTION(BlueprintCallable, Category = "Character|Abilities|Combat")
	int DetermineSubMenuSelection(int MaxMenuCount, int Direction);
	// 
	UFUNCTION(BlueprintCallable, Category = "Character|Abilities|Combat")
	void ResetSubMenuSelection();
	// 
	UFUNCTION(BlueprintPure, Category = "Character|Abilities|Combat")
	FVector2D GetCurrentMenuSelection()
	{
		return FVector2D(ActiveMenuCount, SubMenuCount);
	}
	//
	UFUNCTION(BlueprintPure, Category = "Character|Abilities|Combat")
	int GetPreviousActiveMenuIndex()
	{
		return PreviousActiveMenuIndex;
	}
	//
	UFUNCTION(BlueprintCallable, Category = "Character|Abilities|Combat")
	void Puncture(FVector ImpactLocation, FVector ImpactNormal, FVector ImpactExitPoint, float ImpactRadius, FVector ImpactForce);

	uint8 GetTeamID() const;
	uint8 GetActiveMenuCount() const;
	uint8 GetSubMenuCount() const;
#pragma endregion
};
