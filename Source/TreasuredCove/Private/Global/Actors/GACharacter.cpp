// Fill out your copyright notice in the Description page of Project Settings.

// Gameplay Ability Dependencies
#include "Global/Actors/GACharacter.h"
#include "Global/Actors/GAActor.h"
#include "Global/Actors/Items/GAWeapon.h"
#include "Global/Framework/GAAIController.h"
#include "Global/Framework/GAPlayerController.h"

// Attribute sets
#include "Global/AbilitySystem/AttributeSets/ASHealth.h"
#include "Global/AbilitySystem/AttributeSets/ASAmmo.h"
#include "Global/AbilitySystem/AttributeSets/ASStats.h"
#include "Global/AbilitySystem/AttributeSets/ASUltimate.h"
#include "Global/AbilitySystem/AttributeSets/ASWeaponStats.h"

// Abilities
#include "Global/AbilitySystem/AbilitySet.h"
#include "Global/AbilitySystem/Abilities/GameplayAbilityBase.h"
#include "Global/AbilitySystem/Abilities/GA_DecreaseSpread.h"
#include "Global/AbilitySystem/Abilities/GA_IncreaseSpread.h"
#include "Global/AbilitySystem/Abilities/GA_Interact.h"
#include "GameplayTagContainer.h"

// Custom Components
#include "Global/Components/Characters/GACharacterMovementComponent.h"
// #include "GameFramework/CharacterMovementComponent.h"
#include "Global/Components/GAEnhancedInputComponent.h"

// Abilities
#include "Global/AbilitySystem/GASystemComponent.h"
#include "Global/AbilitySystem/SkillTree/GASkillTreeComponent.h"
#include "Global/Config/GAInputConfigData.h"

// Attributes
#include "Global/Components/Characters/HealthComponent.h"
#include "Global/Components/ComboComponent.h"

// AI or UI
#include "TownSystem/TownSystemComponent.h"
#include "Global/Components/Characters/FloatingTextComponent.h"
#include "Global/Components/Characters/FloatingBarComponent.h"
#include "Global/Components/Characters/FloatingItemInfoComponent.h"
#include "Global/Components/Characters/FootprintComponent.h"
#include "Global/Components/Characters/MovementTrailComponent.h"

// Item interaction
#include "Global/Interfaces/PickupInterface.h"
#include "Global/Components/Items/InventoryComponent.h"
#include "Global/Components/Items/RepositoryComponent.h"
#include "Global/Components/Items/EquipmentComponent.h"
#include "Global/Components/Items/EquippableComponent.h"
#include "Global/Components/Items/PickupMagnetComponent.h"
#include "Global/Components/Items/PickupComponent.h"
#include "Global/Components/Items/CraftingComponent.h"
#include "Global/Components/LaunchingComponent.h"

// Misc
#include "Animation/AnimInstance.h"
#include "GameFramework/DamageType.h"

#include "Global/Enumerations/CustomMovementMode.h"

// Engine Components
#include "BrainComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/PhysicsConstraintActor.h"

// Procedural Mesh Plugin
// #include "ProceduralMeshComponent.h"
// #include "KismetProceduralMeshLibrary.h"
// #include "Rendering/SkeletalMeshRenderData.h"
// #include "Rendering/SkeletalMeshLODRenderData.h"

// Sense System Plugin
#include "SenseReceiverComponent.h"
#include "SenseStimulusComponent.h"
#include "SenseStimulusBase.h"

// Global
#include "AbilitySystemGlobals.h"
#include "DrawDebugHelpers.h"
#include "GameplayAbilitiesModule.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "Global/Libraries/StaticLibrary.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
// #include "Engine/DirectionalLight.h"

DEFINE_LOG_CATEGORY_STATIC(LogCharacter, Log, All);

#pragma region Defaults
// Sets default values
AGACharacter::AGACharacter(const FObjectInitializer& ObjectInitializer) : 
	ACharacter(ObjectInitializer.SetDefaultSubobjectClass<UGACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	/*bInitialized = false;
	bIsPaused = false;
	bShouldUpdateConstructionScript = false;
	bShouldUpdatePhysicalAnimation = false;
	bIsDead = false;
	bCanJump = false;
	bCanSlide = false;
	bCanWallRun = false;
	bCanClimb = false;
	bShadowed = false;*/

	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);

	// Procedural Mesh
	/*RealMesh = 
		CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Real Mesh"));*/

	// Town System Component
	TownInfo =
		CreateDefaultSubobject<UTownSystemComponent>(TEXT("Town System"));

	// Combo System Component
	ComboSystem =
		CreateDefaultSubobject<UComboComponent>(TEXT("Combo System"));

	// Ability System Component
	RenamedAbilitySystem =
		CreateDefaultSubobject<UGASystemComponent>(TEXT("Ability System"));
	RenamedAbilitySystem->SetIsReplicated(true);
	RenamedAbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Skill Tree Component
	SkillTrees =
		CreateDefaultSubobject<UGASkillTreeComponent>(TEXT("Skill Trees"));

	// Pawn Sensing Component
	AISenses = 
		CreateDefaultSubobject<USenseReceiverComponent>(TEXT("AISenses"));
	AISenses->SetupAttachment(RootComponent);
	// AISenses->OnNewSense.AddUniqueDynamic(this, &AGACharacter::OnNewSense);
	// AISenses->SetRelativeRotation(FRotator(0, 0, -90));
	// AISenses->SetRelativeLocation(FVector(0, 0, 68));

	// Stimulus Component
	AIStimulus =
		CreateDefaultSubobject<USenseStimulusComponent>(TEXT("SenseStimulus"));

	// Inventory Component
	Inventory =
		CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	Inventory->SetInventorySize(5);

	// Equipment Component
	Equipment =
		CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment"));
	Equipment->SetInventorySize(5);

	// Repository Component
	Repository =
		CreateDefaultSubobject<URepositoryComponent>(TEXT("Repository"));

	// Magnet Component
	Magnet =
		CreateDefaultSubobject<UPickupMagnetComponent>(TEXT("Magnet"));
	Magnet->SetupAttachment(RootComponent);

	// Crafting Component
	Crafting =
		CreateDefaultSubobject<UCraftingComponent>(TEXT("Crafting"));

	// Dialogue System Component
	DialogueParticipantInfo =
		CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueParticipantInfo"));

	// Phycial Animation Component
	PhysicalAnimation =
		CreateDefaultSubobject<UPhysicalAnimationComponent>(TEXT("Physical Animation"));

	// Floating Text Component
	FloatingTextComponent =
		CreateDefaultSubobject<UFloatingTextComponent>(TEXT("Floating Text"));

	// Floating Bar Component
	FloatingBarComponent =
		CreateDefaultSubobject<UFloatingBarComponent>(TEXT("Floating Bar"));

	// Item Info Popup Component
	FloatingItemInfoComponent =
		CreateDefaultSubobject<UFloatingItemInfoComponent>(TEXT("Floating Item Info"));

	// Footprint Component
	FootprintComponent =
		CreateDefaultSubobject<UFootprintComponent>(TEXT("Footprints"));

	// Launching Component
	LaunchingComponent =
		CreateDefaultSubobject<ULaunchingComponent>(TEXT("Launch Capability"));

	// Movement Trail Component
	MovementTrail =
		CreateDefaultSubobject<UMovementTrailComponent>(TEXT("Movement Trail"));
	MovementTrail->SetSpawnActors(false);

	// Health Component
	HealthComponent =
		CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));

	/* Default Character values */

	// Camera tilt timeline
	CameraTiltTimeline = 
		CreateDefaultSubobject<UTimelineComponent>(TEXT("Camera Tilt Timeline"));

	// Default AIController
	TSubclassOf<AGAAIController> DefaultAIC;
	AIControllerClass = DefaultAIC;

	// Name values
	Tags.Add(FName("GrassAffector"));
	HeadSocket = FName("head");

	TeamID = 255;

	DecreaseSpreadAbility = UGA_DecreaseSpread::StaticClass();
	IncreaseSpreadAbility = UGA_IncreaseSpread::StaticClass();
	ActiveAbilityHandles.SetNum(9);

	// Mesh Collision To Block Visibility Channel
	if (GetMesh())
	{
		// RealMesh->SetupAttachment(GetMesh());

		if (GetMesh()->DoesSocketExist(HeadSocket))
			AISenses->SetupAttachment(GetMesh(), HeadSocket);

		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	}
}

// Called when the game starts or when spawned
void AGACharacter::BeginPlay()
{
	Super::BeginPlay();

	bIsPaused = false;
	// ChangeViewpoint(true /*bFirstPerson*/);
	PhysicalAnimation->SetSkeletalMeshComponent(GetMesh());

	// SightInitialRotation = Sight->GetComponentRotation();

	AutoDetermineTeamIDByControllerType();
	
	if (HealthComponent != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Health Component OnHealthZeroed Regsitered..."));
		HealthComponent->OnAttributeZeroed.AddUniqueDynamic(this, &AGACharacter::Death);
	}
	if (WallRunTimelineCurve)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindDynamic(this, &AGACharacter::UpdateCameraTilt);
		CameraTiltTimeline->AddInterpFloat(WallRunTimelineCurve, TimelineCallback);

		CameraTiltTimeline->SetLooping(false);
		CameraTiltTimeline->SetTimelineLength(0.2f);
		CameraTiltTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
		CameraTiltTimeline->SetPlaybackPosition(0.f, false);
	}
	if (TownInfo != nullptr)
	{
		TownInfo->UpdateDesiredLocation.AddUniqueDynamic(this, &AGACharacter::UpdateDesiredLocation);
		TownInfo->UpdateCurrentBuilding.AddUniqueDynamic(this, &AGACharacter::UpdateCurrentBuilding);
		TownInfo->UpdateTargetBuilding.AddUniqueDynamic(this, &AGACharacter::UpdateTargetBuilding);
	}
	if (AIStimulus != nullptr)
	{
		AIStimulus->OnSensedFromSensor.AddUniqueDynamic(this, &AGACharacter::OnSensed);
		if (AIStimulus->IsRegisteredForSense())
		{
			UE_LOG(LogTemp, Warning, TEXT("SenseStimulus is registered..."));
		} else UE_LOG(LogTemp, Warning, TEXT("SenseStimulus is NOT registered..."));
	}
	if (Equipment != nullptr)
	{
		// Equipment->OnItemEquipped.AddUniqueDynamic(this, &AGACharacter::OnEquipped);
	}

	check(RenamedAbilitySystem);
}

// Called every frame
void AGACharacter::Tick(float InDeltaTime)
{
	Super::Tick(InDeltaTime);
	DeltaTime = InDeltaTime;

	if (CameraTiltTimeline && GetController())
	{
		if (CameraTiltTimeline->IsPlaying())
		{
			CameraTiltTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
			GetController()->SetControlRotation(FMath::RInterpTo(GetControlRotation(), ViewRotation, DeltaTime, 10));
		}
		else if (CameraTiltTimeline->GetPlaybackPosition() == 0.f)
		{
			if (!ViewRotation.Equals(GetControlRotation()))
			{
				ViewRotation = GetControlRotation();
				ViewRotation.Roll = 0; 
				GetController()->SetControlRotation(FMath::RInterpTo(GetControlRotation(), ViewRotation, DeltaTime, 10));
			}
		}
	}
}

// Called to bind functionality to input
void AGACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PC = Cast<AGAPlayerController>(GetController());
	if (PC == nullptr)
		return;

	// Get the local player subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

	if (Subsystem == nullptr)
		return;
	
	// Clear out existing mapping, and add our mapping
	Subsystem->ClearAllMappings();
	// Subsystem->AddPlayerMappableConfig();
	Subsystem->AddMappingContext(InputMapping, 0);
	// Subsystem->AddMappingContext(Unarmed_InputMapping, 1);

	// Get the EnhancedInputComponent
	UGAEnhancedInputComponent* PEI = Cast<UGAEnhancedInputComponent>(PlayerInputComponent);
	
	TArray<uint32> BindHandles;

	// Bind Abilities
	if (InputAbilityActions) PEI->BindAbilityActions(InputAbilityActions, this, &AGACharacter::AbilityInputTagPressed, &AGACharacter::AbilityInputTagReleased, /*out*/ BindHandles);
	// PEI->BindAction(InputActions->InputJump, ETriggerEvent::Triggered, this, &AGACharacter::AbilityInputTagPressed);

	// Bind the actions
	if (InputActions)
	{
		PEI->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &AGACharacter::Move);
		PEI->BindAction(InputActions->InputLook, ETriggerEvent::Triggered, this, &AGACharacter::Look);
		PEI->BindAction(InputActions->InputJump, ETriggerEvent::Triggered, this, &AGACharacter::Jump_Input);
		PEI->BindAction(InputActions->InputCrouch, ETriggerEvent::Triggered, this, &AGACharacter::Crouch_Input);
		PEI->BindAction(InputActions->InputSlide, ETriggerEvent::Triggered, this, &AGACharacter::Slide_Input);
		PEI->BindAction(InputActions->InputSprint, ETriggerEvent::Triggered, this, &AGACharacter::Sprint_Input);
		PEI->BindAction(InputActions->InputSwitch, ETriggerEvent::Triggered, this, &AGACharacter::Switch_Input);
	}

	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AGACharacter::BeginPause);
}

// Called when possessed by a new controller
void AGACharacter::PossessedBy(AController* NewController)
{
	UE_LOG(LogTemp, Warning, TEXT("Possessed..."));

	PC = Cast<AGAPlayerController>(NewController);
	AC = Cast<AGAAIController>(NewController);

	if (RenamedAbilitySystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability System initializing..."));
		InitializeAbilitySystem();

		if (HealthComponent)
		{
			// HealthComponent->InitializeComponent
		}
		if (PC)
		{
			PC->OnPostProcessInput.AddUniqueDynamic(RenamedAbilitySystem, &UGASystemComponent::ProcessAbilityInput);
		}
	}

	if (AC)
	{
		if (AISenses != nullptr)
		{
			// AISenses->OnNewSense.AddUniqueDynamic(AC, &AGAAIController::OnNewSense);
			// AISenses->OnCurrentSense.AddUniqueDynamic(AC, &AGAAIController::OnCurrentSense);
			// AISenses->OnLostSense.AddUniqueDynamic(AC, &AGAAIController::OnLostSense);
			// AISenses->OnForgetSense.AddUniqueDynamic(AC, &AGAAIController::OnForgetSense);
		}
	}

	Super::PossessedBy(NewController);
}

// Called when control is taken away from this pawn
void AGACharacter::UnPossessed()
{
	if (PC) 
	{
		PC->OnPostProcessInput.RemoveAll(RenamedAbilitySystem);
		if (AGAActor* Item = GetHeldItem()) PC->OnPostProcessInput.RemoveAll(Item->GetAbilitySystemComponent());
	}

	Super::UnPossessed();
}

// Replication
void AGACharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGACharacter, ActiveAbilityHandles);
}

// Called when server replicates controller to clients
void AGACharacter::OnRep_Controller()
{
	if (RenamedAbilitySystem)
	{
		RenamedAbilitySystem->RefreshAbilityActorInfo();
	}

	Super::OnRep_Controller();
}

#if WITH_EDITOR
// Called after intializing components, in editor and before begin play
void AGACharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// Called in editor and on begin play
void AGACharacter::OnConstruction(const FTransform& Transform)
{
	if (bShouldUpdateConstructionScript)
	{
		if (!bInitialized)
		{
			;
		}
		else
		{
			;
		}
	}
}

// Called by editor for changes made to UPROPERTYs in TArrays
void AGACharacter::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e)
{
	// Guard Against null properties
	if (e.Property == NULL) return;

	FName MemberPropertyName = e.MemberProperty->GetFName();
	FName PropertyName = e.Property->GetFName();

	Super::PostEditChangeChainProperty(e);
}
#endif
#pragma endregion

#pragma region VisualEffects
// */
//////////////////////////////////////////////////////
/////			CHARACTER VISUAL FX   			//////
//////////////////////////////////////////////////////

void AGACharacter::SpawnNumbers(float Numbers)
{
	FVector Top = FVector(0, 0, GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * 2);
	FloatingTextComponent->AddFloatingText_Client(FText::FromString(FString::SanitizeFloat(Numbers)), GetActorLocation() + Top);
}

void AGACharacter::SpawnValueBar(EAttributeType BarType, float CurrentValue, float MaxValue)
{
	if (FloatingBarComponent->HasIndex((uint8)BarType))
	{
		FloatingBarComponent->UpdateProgress((uint8)BarType, CurrentValue, MaxValue);
	}
	else
	{
		FVector Top = FVector(0, 0, GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * 2);
		FloatingBarComponent->AddFloatingBar_Client(CurrentValue, MaxValue, GetActorLocation() + Top);
	}
}

void AGACharacter::SimulateHit(const FHitResult& Hit, const FVector& ImpactForce)
{
	const FName PhysicsBone = Hit.BoneName == FName("pelvis") ? FName("spine_01") : Hit.BoneName;

	HitReactionTimeRemaining += 0.5;
	GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("pelvis"), true, false);
	PhysicalAnimation->ApplyPhysicalAnimationProfileBelow(PhysicsBone, FName("HitReactionProfile"), false);
	PhysicalAnimation->SetStrengthMultiplyer(1.0f);

	// HitCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->AddImpulseToAllBodiesBelow(ImpactForce, PhysicsBone, true, true);
	if (!GetWorldTimerManager().IsTimerActive(PhysicalAnimationHandle))
	{
		GetWorldTimerManager().SetTimer(PhysicalAnimationHandle, this, &AGACharacter::SimulatePhysicalAnimation, 0.033, true);
	}
}

void AGACharacter::SimulatePhysicalAnimation()
{
	if (bIsDead)
	{
		GetWorldTimerManager().ClearTimer(PhysicalAnimationHandle);
		return;
	}

	HitReactionTimeRemaining -= 0.033;
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("pelvis"), FMath::Min(HitReactionTimeRemaining, 1.0f), false, false);

	if (HitReactionTimeRemaining <= 0)
	{
		HitReactionTimeRemaining = 0;
		GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("pelvis"), false);
		GetWorldTimerManager().ClearTimer(PhysicalAnimationHandle);
	}
}

void AGACharacter::Puncture(FVector ImpactLocation, FVector ImpactNormal, FVector ImpactExitPoint, float ImpactRadius, FVector ImpactForce)
{
	// UStaticLibrary::HolePunchProceduralMesh(RealMesh, ImpactLocation, ImpactNormal, ImpactExitPoint, ImpactRadius, EProcMeshSliceCapOption::CreateNewSectionForCap, MeshInsideColor, 6);
	UParticleSystem* EmitterTemplate = PSPuncture->Template;
	PSPuncture =
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EmitterTemplate, FTransform(FRotator(), ImpactLocation, FVector(1, 1, 1)), true);
	PSPuncture->SetVectorParameter(FName("Velocity"), ImpactForce);
}

void AGACharacter::BeginCameraTilt()
{
	/* Would like this on a timer
	* Not so much a timeline. 
	CameraTiltTime = 0.0;
	FTimerDelegate TiltDel;
	TiltDel.BindUFunction(this, FName("UpdateCameraTilt"), CameraTiltTime, increment, endValue, endTime);
	GetWorldTimerManager().SetTimer(CameraTiltTimerHandle, increment, true); */
	// UE_LOG(LogTemp, Warning, TEXT("Begin Camera Tilt!"));

	CameraTiltTimeline->Play();
}

void AGACharacter::UpdateCameraTilt(float progress)
{
	if (!WallRunTimelineCurve || !GetController())
		return;

	// float TimelineValue = WallRunCameraTiltTimeline.GetPlaybackPosition();
	// float progress = WallRunTimelineCurve->GetFloatValue(TimelineValue);
	
	FRotator Rotation = GetControlRotation();

	if (GetGACharacterMovement())
	{
		if (false)
		{
			if (bIsSliding)
			{
				// ENavType FloorSide = GetGACharacterMovement()->CurrentFloor.HitResult.Normal;
				switch (ENavType::Right)
				{
				case ENavType::Left:
					Rotation.Roll = 0 - progress;
					break;
				case ENavType::Right:
					Rotation.Roll = progress;
					break;
				}
			}
			else if (GetGACharacterMovement()->IsWallRunning())
			{
				switch (GetGACharacterMovement()->GetWallRunSide())
				{
				case ENavType::Left:
					Rotation.Roll = 0 - progress;
					break;
				case ENavType::Right:
					Rotation.Roll = progress;
					break;
				}
			}
			else
			{
				Rotation.Roll = progress;
			}
		}

		switch (GetGACharacterMovement()->GetWallRunSide())
		{
		case ENavType::Front:
		case ENavType::Left:
			Rotation.Roll = 0 - progress;
			break;
		case ENavType::Right:
			Rotation.Roll = progress;
			break;
		default:
			Rotation.Roll = progress;
			break;
		}
	}

	ViewRotation = Rotation;
}

void AGACharacter::EndCameraTilt()
{
	/* Prefer a timer, but I am not smart
	CameraTiltTime = endTime;
	FTimerDelegate TiltDel;
	TiltDel.BindUFunction(this, "UpdateCameraTilt", CameraTiltTime, increment, endValue, 0);
	GetWorldTimerManager().SetTimer(CameraTiltTimerHandle, increment, true);*/

	if (CameraTiltTimeline->IsPlaying())
	{
		CameraTiltTimeline->Reverse();
	}
	else
	{
		CameraTiltTimeline->ReverseFromEnd();
	}

	// if (GEngine) GEngine->AddOnScreenDebugMessage(4, 0.5f, FColor::Red, "End camera tilt");
}
#pragma endregion

#pragma region ComponentGetters
// 
UAbilitySystemComponent* AGACharacter::GetAbilitySystemComponent() const
{
	return RenamedAbilitySystem;
}

//
UTownSystemComponent* AGACharacter::GetTownSystemComponent() const
{
	if (!TownInfo) UE_LOG(LogTemp, Warning, TEXT("Town Component nullptr"));
	return TownInfo;
}

//
URepositoryComponent* AGACharacter::GetRepositoryComponent_Implementation() const
{
	return Repository;
}

//
UInventoryComponent* AGACharacter::GetInventoryComponent_Implementation() const
{
	return Inventory;
}

//
UGACharacterMovementComponent* AGACharacter::GetGACharacterMovement() const
{
	return GetCharacterMovement<UGACharacterMovementComponent>();
}

// 
UGASkillTreeComponent* AGACharacter::GetSkillTreeComponent() const
{
	return SkillTrees;
}
#pragma endregion

#pragma region AbilitiesSystem
// */
//////////////////////////////////////////////////////
/////			Ability Gameplay Tags			//////
//////////////////////////////////////////////////////

// Called to give a gameplay tag to self
void AGACharacter::AddGameplayTag(const FGameplayTag& NewTag)
{
	GetAbilitySystemComponent()->AddLooseGameplayTag(NewTag);
	GetAbilitySystemComponent()->SetTagMapCount(NewTag, 1);
}

// Called to remove a gameplay tag from self
void AGACharacter::RemoveGameplayTag(const FGameplayTag& OldTag)
{
	GetAbilitySystemComponent()->RemoveLooseGameplayTag(OldTag);
}

// */
//////////////////////////////////////////////////////
/////			CHARACTER ABILITIES				//////
//////////////////////////////////////////////////////

// Called during begin play or possession
void AGACharacter::InitializeAbilitySystem()
{
	RenamedAbilitySystem->InitAbilityActorInfo(this, this);

	if (!AttributeSetClasses.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Attribute Sets initializing..."));

		for (TSubclassOf<UAttributeSet>& SetClass : AttributeSetClasses)
		{
			// const_cast to access non-const properties within UAttributeSet such as:
			// - Dynamic Delegates
			UAttributeSet* Set = const_cast<UAttributeSet*>(RenamedAbilitySystem->InitStats(SetClass, InitialStatsTable));

			InitializeAttributeSet(Set);
		}

		// UAbilitySystemGlobals* ASG = IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals();
		// FAttributeSetInitter* ASI = ASG->GetAttributeSetInitter();
		// ASI->InitAttributeSetDefaults(AbilitySystem, FName("Default"), 1, true);

		if (AttributesToSave.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Override attributes with saved values..."));
			PopulateSavedAttributes(AttributesToSave);
		}
	}

	// This should be a UDataAsset Instance. Meaning there is no need to spawn anything
	// And data should be readily available for access.
	FAbilitySet_GrantedHandles* OutHandles = nullptr;
	if (InitialActiveAbilities) InitialActiveAbilities->GiveToAbilitySystem(GetAbilitySystemComponent(), OutHandles, this);
	RenamedAbilitySystem->OnAbilityEnded.AddUObject(this, &AGACharacter::AbilityEnded);
}

// Called during ability system initialization
void AGACharacter::InitializeAttributeSet(UAttributeSet* Set)
{
	UASStats* StatSet = Cast<UASStats>(Set);
	if (StatSet)
	{
		StatSet->OnExperienceModified.AddUniqueDynamic(this, &AGACharacter::OnExperienceModified);
		StatSet->OnStatModified.AddUniqueDynamic(this, &AGACharacter::OnStatModified);
		return;
	}

	UASWeaponStats* WeaponSet = Cast<UASWeaponStats>(Set);
	if (WeaponSet)
	{
		// WeaponSet->OnSpreadAngleModified.AddUniqueDynamic(this, &AGACharacter::OnSpreadAngleModified);
		WeaponSet->OnHipAccuracyModified.AddUniqueDynamic(this, &AGACharacter::OnHipAccuracyModified);
		WeaponSet->OnADSAccuracyModified.AddUniqueDynamic(this, &AGACharacter::OnADSAccuracyModified);
		WeaponSet->OnHandlingModified.AddUniqueDynamic(this, &AGACharacter::OnHandlingModified);
		WeaponSet->OnRangeModified.AddUniqueDynamic(this, &AGACharacter::OnRangeModified);
		WeaponSet->OnDamageModified.AddUniqueDynamic(this, &AGACharacter::OnDamageModified);
		return;
	}
}

// Called to give an ability to a player. This should be called by a skill tree asset or at begin play in a multiplayer match.
void AGACharacter::AquireAbility(TSubclassOf<UGameplayAbility> InAbility, FGameplayAbilitySpecHandle &OutHandle)
{
	if (RenamedAbilitySystem)
	{
		if (HasAuthority() && InAbility)
		{
			FGameplayAbilitySpecDef SpecDef = FGameplayAbilitySpecDef();
			SpecDef.Ability = InAbility;
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(SpecDef, 1);
			OutHandle = RenamedAbilitySystem->GiveAbility(AbilitySpec);
			RenamedAbilitySystem->OnAbilityEnded.AddUObject(this, &AGACharacter::AbilityEnded);

			if (InAbility->IsChildOf(UGameplayAbilityBase::StaticClass()))
			{
				TSubclassOf<UGameplayAbilityBase> AbilityBaseClass = *InAbility;
				if (AbilityBaseClass != nullptr)
				{
					AddAbilityToUI(AbilityBaseClass, EAbilityType::Primary, OutHandle, false);
				}
			}
		}
	}
}

// Called to give multiple abilities to a player at once.
void AGACharacter::AquireAbilities(TArray<TSubclassOf<UGameplayAbility>> InAbilities, TArray<FGameplayAbilitySpecHandle> &OutHandles)
{
	int i = -1;
	for (const TSubclassOf<UGameplayAbility>& InAbility : InAbilities)
	{
		AquireAbility(InAbility, OutHandles[++i]);
	}
}

void AGACharacter::AquirePassive(TSubclassOf<UGameplayAbility> Passive)
{
	if (RenamedAbilitySystem)
	{
		if (HasAuthority() && Passive)
		{
			FGameplayAbilitySpecHandle NewPassiveHandle;
			FGameplayAbilitySpecDef SpecDef = FGameplayAbilitySpecDef();
			SpecDef.Ability = Passive;
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(SpecDef, 1);
			NewPassiveHandle = RenamedAbilitySystem->GiveAbility(AbilitySpec);
			RenamedAbilitySystem->TryActivateAbility(NewPassiveHandle);

			if (Passive->IsChildOf(UGameplayAbilityBase::StaticClass()))
			{
				TSubclassOf<UGameplayAbilityBase> AbilityBaseClass = *Passive;
				if (AbilityBaseClass != nullptr)
				{
					AddAbilityToUI(AbilityBaseClass, EAbilityType::Passive, NewPassiveHandle, false);
				}
			}
			PassiveHandles.Add(NewPassiveHandle);
		}
	}
}

void AGACharacter::AquirePassives(TArray<TSubclassOf<UGameplayAbility>> InPassives)
{
	for (const TSubclassOf<UGameplayAbility>& InPassive : InPassives)
	{
		AquirePassive(InPassive);
	}
}

// Called to remove abilities (A specific case of item abilities that must be added when picked up)
void AGACharacter::RemoveAbility(FGameplayAbilitySpecHandle InHandle)
{
	if (RenamedAbilitySystem)
	{
		if (HasAuthority())
		{
			RenamedAbilitySystem->ClearAbility(InHandle);
			RemoveAbilityFromUI(InHandle);
		}
	}
}

// Called to remove multiple abilities at once
void AGACharacter::RemoveAbilities(TArray< FGameplayAbilitySpecHandle > InHandles)
{
	for (FGameplayAbilitySpecHandle InHandle : InHandles)
	{
		RemoveAbility(InHandle);
	}
}

void AGACharacter::AbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	OnAbilityEnded.Broadcast(AbilityEndedData.AbilityThatEnded);
}

// Called to pass the given ability information onto the player's HUD.
void AGACharacter::AddAbilityToUI(TSubclassOf<UGameplayAbilityBase> InAbility, EAbilityType AbilityType, FGameplayAbilitySpecHandle InHandle, bool bFromItem)
{
	if (PC)
	{
		UGameplayAbilityBase* AbilityInstance = InAbility.Get()->GetDefaultObject<UGameplayAbilityBase>();
		if (AbilityInstance)
		{
			FAbilityInfo AbilityInfo = AbilityInstance->GetAbilityInfo();
			// PC->AddAbilityToUI(AbilityInfo, AbilityType, InHandle, bFromItem);
		}
	}
}

// Called to remove abilities from UI
void AGACharacter::RemoveAbilityFromUI(FGameplayAbilitySpecHandle InHandle)
{
	if (PC)
	{
		PC->RemoveAbilityFromUI_Client(InHandle);
	}
}
#pragma endregion

#pragma region InputHandling
// */
//////////////////////////////////////////////////////
/////				CHARACTER INPUT				//////
//////////////////////////////////////////////////////

void AGACharacter::BeginQuickSelect()
{
	/* if (bIsPaused)
	{
		if (PC)
		{
			// PC->RecieveTabRequest(false);
		}
	}
	else
	{
		if (!bToggleQuickSelect)
		{
			bWeaponPriority = true;
		}
		else
		{
			if (bWeaponPriority)
			{
				bWeaponPriority = false;
			}
			else
			{
				bWeaponPriority = true;
			}
		}

		if (PC)
		{
			// PC->SetItemPriority(bWeaponPriority);
		}
	} // */
}

void AGACharacter::EndQuickSelect()
{
	/* if (bIsPaused)
	{

	}
	else
	{
		if (!bToggleQuickSelect)
		{
			bWeaponPriority = true;

			if (PC)
			{
				// PC->SetItemPriority(bWeaponPriority);
			}
		}
		else
		{

		}
	} // */
}

// Called to check whether we want to activate our weapon or our personal ability.
bool AGACharacter::ShouldPrioritizeWeapon() const
{
	if (true /*bWeaponPriority*/)
	{
		if (AGAActor* HeldItem = GetHeldItem())
		{
			if (HeldItem->GetClass()->ImplementsInterface(UControlInputInterface::StaticClass()))
			{
				return true;
			}
		}
	}

	return false;
}

// Called to use the primary ability
void AGACharacter::BeginPrimary()
{
	if (bIsPaused)
	{

	}
	else
	{
		FTimerDelegate InputDel;

		InputDel.BindUFunction(this, FName("StartHeldInputTimer"), EHeldInputType::Primary);
		GetWorldTimerManager().ClearTimer(InputTimerHandle);

		GetWorldTimerManager().SetTimer(InputTimerHandle, InputDel, 0.1, true);

		if (ShouldPrioritizeWeapon())
		{
			AGAActor* HeldItem = GetHeldItem();
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Use Item Primary!"));
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, HeldItem->GetFName().ToString());

			IControlInputInterface::Execute_UsePrimary(HeldItem);
		}
		else if (ActiveAbilityHandles.IsValidIndex(0))
		{
			RenamedAbilitySystem->TryActivateAbility(ActiveAbilityHandles[0]);
		}
	}
}

void AGACharacter::EndPrimary()
{
	if (bIsPaused)
	{

	}
	else
	{
		if (ActiveAbilityHandles[0].IsValid())
		{
			if (AGAActor* HeldItem = GetHeldItem()) HeldItem->AbilitySystem->InputConfirm();
			RenamedAbilitySystem->CancelAbilityHandle(ActiveAbilityHandles[0]);
		}
		GetWorldTimerManager().ClearTimer(InputTimerHandle);
	}
}

// Called to use the secondary ability
void AGACharacter::BeginSecondary()
{
	if (bIsPaused)
	{

	}
	else
	{
		FTimerDelegate InputDel;

		InputDel.BindUFunction(this, FName("StartHeldInputTimer"), EHeldInputType::Secondary);
		GetWorldTimerManager().ClearTimer(InputTimerHandle);

		GetWorldTimerManager().SetTimer(InputTimerHandle, InputDel, 0.1, true);

		if (ShouldPrioritizeWeapon())
		{
			AGAActor* HeldItem = GetHeldItem();

			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Use Item Primary!"));
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, HeldItem->GetFName().ToString());

			IControlInputInterface::Execute_UseSecondary(HeldItem);
		}
		else if (ActiveAbilityHandles[1].IsValid())
		{
			RenamedAbilitySystem->TryActivateAbility(ActiveAbilityHandles[1]);
		}
	}
}

void AGACharacter::EndSecondary()
{
	if (bIsPaused)
	{

	}
	else
	{
		if (AGAActor* HeldItem = GetHeldItem()) HeldItem->AbilitySystem->InputConfirm();
		GetWorldTimerManager().ClearTimer(InputTimerHandle);
	}
}

// Called to Switch items
void AGACharacter::Switch(float Val)
{
	if (Val != 0)
	{
		if (bIsPaused)
		{

		}
		else
		{
			if (Val > 0)
			{
				BeginSwitch();
			}
			else
			{
				EndSwitch();
			}
		}
	}
}

// Called to switch items 
void AGACharacter::BeginSwitch()
{
	if (bIsPaused)
	{

	}
	else
	{
		FTimerDelegate InputDel;
		
		InputDel.BindUFunction(this, FName("StartHeldInputTimer"), EHeldInputType::Switch);
		GetWorldTimerManager().ClearTimer(InputTimerHandle);
		
		GetWorldTimerManager().SetTimer(InputTimerHandle, InputDel, 0.1, true);
		
		SwitchEquippedItems();
	}
}

void AGACharacter::EndSwitch()
{
	if (bIsPaused)
	{

	}
	else
	{
		SwitchEquippedItems();

		GetWorldTimerManager().ClearTimer(InputTimerHandle);
	}
}

// Called to use the throw ability
void AGACharacter::BeginThrow()
{
	if (bIsPaused)
	{

	}
	else
	{
		FTimerDelegate InputDel;

		InputDel.BindUFunction(this, FName("StartHeldInputTimer"), EHeldInputType::Throw);
		GetWorldTimerManager().ClearTimer(InputTimerHandle);

		GetWorldTimerManager().SetTimer(InputTimerHandle, InputDel, 0.1, true);

		if (ShouldPrioritizeWeapon())
		{
			AGAActor* HeldItem = GetHeldItem();
			// IControlInputInterface::Execute_UseThrow(HeldItem);
		}
		if (ActiveAbilityHandles.IsValidIndex(1))
		{
			RenamedAbilitySystem->TryActivateAbility(ActiveAbilityHandles[1]);
		}
	}
}

void AGACharacter::EndThrow()
{
	if (bIsPaused)
	{
		if (PC)
		{
			// PC->RecieveTabRequest(true);
		}
	}
	else
	{
		RenamedAbilitySystem->InputConfirm();

		GetWorldTimerManager().ClearTimer(InputTimerHandle);
	}
}

// Called to use the melee ability
void AGACharacter::BeginMelee()
{
	if (bIsPaused)
	{

	}
	else
	{
		FTimerDelegate InputDel;

		InputDel.BindUFunction(this, FName("StartHeldInputTimer"), EHeldInputType::Melee);
		GetWorldTimerManager().ClearTimer(InputTimerHandle);

		GetWorldTimerManager().SetTimer(InputTimerHandle, InputDel, 0.1, true);

		if (ShouldPrioritizeWeapon())
		{
			AGAActor* HeldItem = GetHeldItem();
			IControlInputInterface::Execute_UseMelee(HeldItem);
		}
		else if (ActiveAbilityHandles[3].IsValid())
		{
			RenamedAbilitySystem->TryActivateAbility(ActiveAbilityHandles[3]);
		}
	}
}

void AGACharacter::EndMelee()
{
	if (bIsPaused)
	{

	}
	else
	{
		GetWorldTimerManager().ClearTimer(InputTimerHandle);
	}
}

// Called to interact with items or people in the world
void AGACharacter::BeginInteract()
{
	if (bIsPaused)
	{

	}
	else
	{
		FTimerDelegate InputDel;

		InputDel.BindUFunction(this, FName("StartHeldInputTimer"), EHeldInputType::Interact);
		GetWorldTimerManager().ClearTimer(InputTimerHandle);

		GetWorldTimerManager().SetTimer(InputTimerHandle, InputDel, 0.01, true);

		if (ActiveAbilityHandles[0].IsValid())
		{
			// GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Emerald, TEXT("Interacting???"));
			if (!RenamedAbilitySystem->TryActivateAbility(ActiveAbilityHandles[0]))
			{
				GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Emerald, TEXT("Failed to activate interaction ability."));
			}
		}
		else 
		{
			GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Emerald, TEXT("Interaction handle is invalid."));
			// AquireAbility(InitialActiveAbilities.InteractAbility.Ability, EAbilityType::Interact, ActiveAbilityHandles[5], false);
		}
	}
}

// Called to interact with items or people in the world
void AGACharacter::EndInteract()
{
	if (bIsPaused)
	{

	}
	else
	{
		GetWorldTimerManager().ClearTimer(InputTimerHandle);
	}
}

void AGACharacter::BeginUltimate()
{
	if (bIsPaused)
	{

	}
	else
	{
		FTimerDelegate InputDel;

		InputDel.BindUFunction(this, FName("StartHeldInputTimer"), EHeldInputType::Ultimate);
		GetWorldTimerManager().ClearTimer(InputTimerHandle);

		GetWorldTimerManager().SetTimer(InputTimerHandle, InputDel, 0.1, true);

		if (ShouldPrioritizeWeapon())
		{
			AGAActor* HeldItem = GetHeldItem();
			// IControlInputInterface::Execute_UseUltimate(HeldItem, 1);
		}
		else if (ActiveAbilityHandles[5].IsValid())
		{
			RenamedAbilitySystem->TryActivateAbility(ActiveAbilityHandles[5]);
		}
	}
}

void AGACharacter::EndUltimate()
{
	if (bIsPaused)
	{

	}
	else
	{
		GetWorldTimerManager().ClearTimer(InputTimerHandle);
	}
}

void AGACharacter::StartHeldInputTimer(EHeldInputType InputType)
{
	if (PC == nullptr) return;

	switch (InputType)
	{
	case EHeldInputType::Primary:
		HoldTime = PC->GetInputKeyTimeDown("Primary");
		break;
	case EHeldInputType::Secondary:
		HoldTime = PC->GetInputKeyTimeDown("Secondary");
		break;
	case EHeldInputType::Switch:
		HoldTime = PC->GetInputKeyTimeDown("Switch");
		break;
	case EHeldInputType::Throw:
		HoldTime = PC->GetInputKeyTimeDown("Throw");
		break;
	case EHeldInputType::Melee:
		HoldTime = PC->GetInputKeyTimeDown("Melee");
		break;
	case EHeldInputType::Interact:
		HoldTime = PC->GetInputKeyTimeDown("Interact");
		break;
	case EHeldInputType::Ultimate:
		HoldTime = PC->GetInputKeyTimeDown("Ultimate");
		break;
	}
}

#pragma region MovementInput
// */
//////////////////////////////////////////////////////
/////			CHARACTER MOVEMENT				//////
//////////////////////////////////////////////////////

#pragma region PlayerInputBindings
// Called to move the character
void AGACharacter::Move(const FInputActionValue& Value)
{
	if (bIsPaused) return;

	FVector2D MoveDirection = Value.Get<FVector2D>();
	if (MoveDirection.Size() > 0)
	{
		MoveForward(MoveDirection.Y);
		MoveRight(MoveDirection.X);
	}
}

// Called to turn the character
void AGACharacter::Look(const FInputActionValue& Value)
{
	if (bIsPaused) return;

	FVector2D LookDirection = Value.Get<FVector2D>();
	if (LookDirection.Size() > 0)
	{
		LookUp(LookDirection.Y);
		LookRight(LookDirection.X);
	}
}

// Called to crouch
void AGACharacter::Crouch_Input(const FInputActionValue& Value)
{
	if (bIsPaused) return;

	bool PressedCrouch = Value.Get<bool>();
	if (PressedCrouch)
	{
		BeginCrouch();
	}
	else
	{
		EndCrouch();
	}
}

// Called to jump
void AGACharacter::Jump_Input(const FInputActionValue& Value)
{
	if (bIsPaused) return;

	bool PressedJump = Value.Get<bool>();
	if (PressedJump)
	{
		BeginJump();
	}
	else
	{
		EndJump();
	}
}

// Called to run
void AGACharacter::Sprint_Input(const FInputActionValue& Value)
{
	if (bIsPaused) return;

	bool PressedSprint = Value.Get<bool>();
	if (PressedSprint)
	{
		BeginSprint();
	}
	else
	{
		EndSprint();
	}
}

void AGACharacter::Slide_Input(const FInputActionValue& Value)
{
}

// Called to switch
void AGACharacter::Switch_Input(const FInputActionValue& Value)
{

}
void AGACharacter::AbilityInputTagPressed(FGameplayTag Tag)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Pressed!");
	AGAActor* HeldItem = GetHeldItem();
	if (/*bWeaponPriority && */HeldItem && HeldItem->AbilitySystem && HeldItem->AbilitySystem->HasAbilityInputTag(Tag)) HeldItem->AbilitySystem->AbilityInputTagPressed(Tag);
	else RenamedAbilitySystem->AbilityInputTagPressed(Tag);
}
void AGACharacter::AbilityInputTagReleased(FGameplayTag Tag)
{
	AGAActor* HeldItem = GetHeldItem();
	if (/*bWeaponPriority && */HeldItem && HeldItem->AbilitySystem && HeldItem->AbilitySystem->HasAbilityInputTag(Tag)) HeldItem->AbilitySystem->AbilityInputTagReleased(Tag);
	else RenamedAbilitySystem->AbilityInputTagReleased(Tag);
}
#pragma endregion

#pragma region BasicMovement
// Called to move the player's pawn forward
void AGACharacter::MoveForward(float Val)
{
	ForwardAxisValue = Val;
	FVector Direction = FVector::ZeroVector;
	FRotator Rotation = FRotator::ZeroRotator;

	switch (GetGACharacterMovement()->MovementMode)
	{
	case MOVE_Swimming:
		Rotation = Controller->GetControlRotation();
		Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		break;
	case EMovementMode::MOVE_Custom:
		switch (GetGACharacterMovement()->CustomMovementMode)
		{
		case ECustomMovementMode::MOVE_Climbing:
			Rotation = GetActorRotation();
			Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Z);
			break;
		default:
			Rotation = Controller->GetControlRotation();
			Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		}
		break;
	case MOVE_Walking:
	case MOVE_NavWalking:
	case MOVE_Falling:
	default:
		Rotation = GetControlRotation();
		Rotation.Pitch = 0.0f;
		Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
	}

	AddMovementInput(Direction, Val);
}

// Called to move the player's pawn right
void AGACharacter::MoveRight(float Val)
{
	RightAxisValue = Val;
	FVector Direction = FVector::ZeroVector;
	FRotator Rotation = FRotator::ZeroRotator;

	switch (GetGACharacterMovement()->MovementMode)
	{
	case MOVE_Swimming:
		Rotation = Controller->GetControlRotation();
		Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		break;
	case EMovementMode::MOVE_Custom:
		switch (GetGACharacterMovement()->CustomMovementMode)
		{
		case ECustomMovementMode::MOVE_Climbing:
			Rotation = GetActorRotation();
			// Direction = FVector::UpVector ^ Rotation.Vector();
			Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
			break;
		default:
			Rotation = Controller->GetControlRotation();
			Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		}
		break;
	case MOVE_Walking:
	case MOVE_NavWalking:
	case MOVE_Falling:
	default:
		Rotation = Controller->GetControlRotation();
		Rotation.Pitch = 0.0f;
		Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
	}

	AddMovementInput(Direction, Val);
}
#pragma endregion

#pragma region LookingAround
// Called to look up
void AGACharacter::LookUp(float Val)
{
	if (GetActorRotation().Pitch <= LookUpperLimit || Val < 0)
	{
		if (bIsUsingController)
		{
			AddControllerPitchInput(Val * VerticalLookRate);
		}
		else
		{
			AddControllerPitchInput(Val);
		}

		UpdateSightRotation();
	}
}

// Called to look right
void AGACharacter::LookRight(float Val)
{
	if (bIsUsingController)
	{
		AddControllerYawInput(Val * HorizontalLookRate);
	}
	else
	{
		AddControllerYawInput(Val);
	}

	// Update Pitch on other players' clients
	if (!IsLocallyControlled())
	{
		FRotator NewRot = GetMesh()->GetRelativeRotation();
		NewRot.Pitch = RemoteViewPitch * 360 / 256;

		GetMesh()->SetRelativeRotation(NewRot);
	}

	UpdateSightRotation();
}
#pragma endregion

#pragma region Jumping
/** Jumping Functions */

// Overridden to allow jump from crouch
bool AGACharacter::CanJumpInternal_Implementation() const
{
	// if (JumpIsAllowedInternal()) UE_LOG(LogTemp, Warning, TEXT("Character CanJumpInternal()"));
	// if (bCanJump) UE_LOG(LogTemp, Warning, TEXT("Character Can Jump."));

	return bCanJump && JumpIsAllowedInternal();
}

void AGACharacter::OnJumped_Implementation()
{
	// UE_LOG(LogTemp, Warning, TEXT("We performed a jump!"));
}

// Called When Pressing The "Jump" Action Key
void AGACharacter::BeginJump()
{
	Jump();
}

// Called When Releasing The "Jump" Action Key
void AGACharacter::EndJump()
{
	StopJumping();
}
#pragma endregion

//
void AGACharacter::ClampHorizontalVelocity()
{
	if (GetGACharacterMovement()->IsFalling())
	{
		/*if (FVector2D(GetVelocity()).Length() / GetCharacterMovement()->GetMaxSpeed() > 1.0)
		{
			SetHorizontalVelocity(FVector2D(GetVelocity()) / GetCharacterMovement()->GetMaxSpeed());
		}*/
	}
}

#pragma region WallRunning
/** Wall Run Functions */

// Called when starting to WallRun
void AGACharacter::OnBeginWallRun()
{
	BeginCameraTilt();
	bIsWallRunning = true;
}

// Called when coming out of a WallRun
void AGACharacter::OnEndWallRun()
{
	EndCameraTilt();
	bIsWallRunning = false;
}
#pragma endregion

#pragma region Sliding
/** Slding Functions */

// Start Slide
void AGACharacter::OnBeginSlide()
{
	BeginCameraTilt();
	bIsSliding = true;
}

// Stop slide
void AGACharacter::OnEndSlide()
{
	EndCameraTilt();
	bIsSliding = false;
}
#pragma endregion

#pragma region Sprinting
// Called to validate character can run
bool AGACharacter::CanSprint() const 
{ 
	return !bIsSprinting && GetGACharacterMovement() && GetGACharacterMovement()->CanEverSprint() && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
}

// Called to start running
void AGACharacter::Sprint()
{
	if (GetGACharacterMovement())
	{
		if (CanSprint())
		{
			GetGACharacterMovement()->StartSprinting();
		}
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		else if (!GetGACharacterMovement()->CanEverSprint())
		{
			UE_LOG(LogCharacter, Log, TEXT("%s is trying to sprint, but sprinting is disabled on this character! (check CharacterMovement NavAgentSettings)"), *GetName());
		}
#endif
	}
}

// Called to stop running
void AGACharacter::UnSprint()
{
	if (GetGACharacterMovement())
	{
		GetGACharacterMovement()->StopSprinting();
	}
}

// Called When Pressing The "Run" Action Key
void AGACharacter::BeginSprint()
{
	if (bToggleSprint)
	{
		if (!bIsSprinting)
		{
			Sprint();
		}
		else
		{
			UnSprint();
		}
	}
	else
	{
		Sprint();
	}
}

// Called When Releasing The "Run" Action Key
void AGACharacter::EndSprint()
{
	if (!bToggleSprint) UnSprint();
}
#pragma endregion

#pragma region Crouching
/** Crouch Functions */

// Overriden for first person crouching
void AGACharacter::OnStartCrouch(float HeightAdjust, float ScaledHeightAdjust)
{
	Super::OnStartCrouch(HeightAdjust, ScaledHeightAdjust);
}

// Empty Override
void AGACharacter::OnEndCrouch(float HeightAdjust, float ScaledHeightAdjust)
{
	Super::OnEndCrouch(HeightAdjust, ScaledHeightAdjust);
}

// Called to Start crouching
void AGACharacter::BeginCrouch()
{
	if (bToggleCrouch)
	{
		if (!bIsCrouched)
		{
			Crouch();
		}
		else
		{
			UnCrouch();
		}
	}
	else
	{
		Crouch();
	}
}

// Called to Stop crouching
void AGACharacter::EndCrouch()
{
	if (!bToggleCrouch) UnCrouch();
}
#pragma endregion
#pragma endregion

#pragma region MenuInput
// */
//////////////////////////////////////////////////////
/////			CHARACTER MENU INPUT			//////
//////////////////////////////////////////////////////

void AGACharacter::BeginPause()
{
	if (bIsPaused)
	{
		bIsPaused = false;
	}
	else
	{
		bIsPaused = true;
	}
}

void AGACharacter::EndPause()
{

}

void AGACharacter::MenuUp(float Val)
{
	//if (Val != 0)
	//{
	//	if (Val > 0)
	//	{
	//		if (bIsPaused)
	//		{
	//			if (PC)
	//			{
	//				PC->RecieveNavRequest(ENavType::Up);
	//			}
	//		}
	//		else
	//		{
	//			FTimerDelegate MenuUpDel;
	//			if (MenuUpTimerHandle.IsValid())
	//			{
	//				if (GetWorldTimerManager().IsTimerPaused(MenuUpTimerHandle))
	//					GetWorldTimerManager().UnPauseTimer(MenuUpTimerHandle);
	//			}
	//			else 
	//			{
	//				GetWorldTimerManager().SetTimer(MenuUpTimerHandle, this, &AGACharacter::MenuUpDelayed, 0.05f, true);
	//			}
	//		}
	//	}
	//	else if (Val < 0)
	//	{
	//		if (bIsPaused)
	//		{
	//			if (PC)
	//			{
	//				PC->RecieveNavRequest(ENavType::Down);
	//			}
	//		}
	//		else
	//		{
	//			if (MenuDownTimerHandle.IsValid())
	//			{
	//				if (GetWorldTimerManager().IsTimerPaused(MenuDownTimerHandle))
	//					GetWorldTimerManager().UnPauseTimer(MenuDownTimerHandle);
	//			}
	//			else
	//			{
	//				GetWorldTimerManager().SetTimer(MenuDownTimerHandle, this, &AGACharacter::MenuDownDelayed, 0.05f, true);
	//			}
	//		}
	//	}
	//}
	//else
	//{
	//	// Value is 0; Meaning we are not touching the input
	//	// Clearing the timer handle will stop the menu selection
	//	GetWorldTimerManager().ClearTimer(MenuUpTimerHandle);
	//	GetWorldTimerManager().ClearTimer(MenuDownTimerHandle);
	//}
}

void AGACharacter::MenuRight(float Val)
{
	//if (Val != 0)
	//{
	//	FTimerDelegate MenuDel;

	//	if (Val == 1)
	//	{
	//		if (bIsPaused)
	//		{
	//			if (PC)
	//			{
	//				PC->RecieveNavRequest(ENavType::Right);
	//			}
	//		}
	//		else
	//		{
	//			if (MenuRightTimerHandle.IsValid())
	//			{
	//				if (GetWorldTimerManager().IsTimerPaused(MenuRightTimerHandle))
	//					GetWorldTimerManager().UnPauseTimer(MenuRightTimerHandle);
	//			}
	//			else
	//			{
	//				GetWorldTimerManager().SetTimer(MenuRightTimerHandle, this, &AGACharacter::MenuRightDelayed, 0.1f, true);
	//			}
	//		}
	//	}
	//	else if (Val == -1)
	//	{
	//		if (bIsPaused)
	//		{
	//			if (PC)
	//			{
	//				PC->RecieveNavRequest(ENavType::Left);
	//			}
	//		}
	//		else
	//		{
	//			if (MenuLeftTimerHandle.IsValid())
	//			{
	//				if (GetWorldTimerManager().IsTimerPaused(MenuLeftTimerHandle))
	//					GetWorldTimerManager().PauseTimer(MenuLeftTimerHandle);
	//			}
	//			else
	//			{
	//				GetWorldTimerManager().SetTimer(MenuLeftTimerHandle, this, &AGACharacter::MenuLeftDelayed, 0.1f, true);
	//			}
	//		}
	//	}
	//}
	//else
	//{
	//	// Value is 0; Meaning we are not touching the input
	//	// Clearing the timer handle will stop the menu selection
	//	GetWorldTimerManager().ClearTimer(MenuRightTimerHandle);
	//	GetWorldTimerManager().ClearTimer(MenuLeftTimerHandle);
	//}
}
#pragma endregion
#pragma endregion

#pragma region Combat
// */
//////////////////////////////////////////////////////
/////			CHARACTER COMBO					//////
//////////////////////////////////////////////////////

// */
//////////////////////////////////////////////////////
/////			CHARACTER TEAMS 				//////
//////////////////////////////////////////////////////

// Returns TeamID
uint8 AGACharacter::GetTeamID() const
{
	return TeamID;
}

// Team ID based on player controller vs AI controller
void AGACharacter::AutoDetermineTeamIDByControllerType()
{
	if (GetController() && GetController()->IsPlayerController())
	{
		TeamID = 0;
	}
}

// Friendly fire check
bool AGACharacter::IsOtherHostile(AGACharacter* Other)
{
	return TeamID != Other->GetTeamID();
}

// */
//////////////////////////////////////////////////////
/////			CHARACTER ATTRIBUTES			//////
//////////////////////////////////////////////////////

bool AGACharacter::AttributeCheck(FGameplayAttribute TestAttribute, uint8 Difficulty, EAdvantageType Advantage)
{
	bool LocBool = false;

	if (RenamedAbilitySystem->HasAttributeSetForAttribute(TestAttribute))
	{
		float Roll = 0;
		float Modifier = RenamedAbilitySystem->GetNumericAttribute(TestAttribute);
		Modifier = -5 + floorf(Modifier / 2);
		float FirstRoll = 0;
		float SecondRoll = 0;

		switch (Advantage)
		{
		case EAdvantageType::Advantage:

			FirstRoll = FMath::RandRange(1.0f, 20.0f);
			SecondRoll = FMath::RandRange(1.0f, 20.0f);

			if (FirstRoll > SecondRoll)
			{
				Roll = FirstRoll;
			}
			else
			{
				Roll = SecondRoll;
			}

			break;
		case EAdvantageType::Neutral:

			Roll = FMath::RandRange(1.0f, 20.0f);

			break;
		case EAdvantageType::Disadvantage:

			FirstRoll = FMath::RandRange(1.0f, 20.0f);
			SecondRoll = FMath::RandRange(1.0f, 20.0f);

			if (FirstRoll < SecondRoll)
			{
				Roll = FirstRoll;
			}
			else
			{
				Roll = SecondRoll;
			}

			break;
		}

		float TestValue = Modifier + Roll;

		if (TestValue > Difficulty)
		{
			LocBool = true;
		}
	}

	return LocBool;
}

void AGACharacter::OnStaminaModified(float Stamina, float MaxStamina)
{
}

void AGACharacter::OnManaModified(float Mana, float MaxMana)
{
}

// Called when experience has been changed
void AGACharacter::OnExperienceModified(float Experience, float MaxExperience)
{
}

// Called when a stat changes
void AGACharacter::OnStatModified(float Charisma, float Constitution, float Dexterity, float Intelligence, float Strength, float Wisdom)
{
}

// Called when current ammo count changes
void AGACharacter::OnAmmoModified(float Ammo, float MaxAmmo)
{
}

// Called when Spread Angle Changes
void AGACharacter::OnSpreadAngleModified(float SpreadAngle)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Purple, "SpreadAngle Modified from character");

}

void AGACharacter::OnHipAccuracyModified(float SpreadMin, float SpreadMax, float SpreadDecreaseSpeed, float SpreadIncreaseSpeed, float SpreadDeltaMultiplier)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Purple, "HipAccuracy Modified from character");
}

void AGACharacter::OnADSAccuracyModified(float Sway, float HorizontalRecoil, float VerticalRecoil)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Purple, "ADSAccuracy Modified from character");
}

void AGACharacter::OnHandlingModified(float ReloadSpeed, float DrawSpeed, float ADSSpeed)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Purple, "Handling Modeified from character");
}

void AGACharacter::OnRangeModified(float MinRange, float MaxRange)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Purple, "Range Modified from character");
}

void AGACharacter::OnDamageModified(float MinDamage, float MaxDamage)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Purple, "Damage Modified from character");
}

void AGACharacter::OnCoreWeaponStatsModified(float Cooldown, float Cost, float CastTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Purple, "Cooldown/Cost/CastTime Modified from character");
}

// Called on Ultimate charge change
void AGACharacter::OnUltimateModified(float UltimateCharge, float MaxUltimateCharge)
{

}

// Called when saving
bool AGACharacter::GetSavedAttributesCurrentValues(TArray<FSavedAttribute>& OutAttributes)
{
	if (RenamedAbilitySystem == nullptr)
	{
		return false;
	}

	for (FSavedAttribute& SA : AttributesToSave)
	{
		if (RenamedAbilitySystem->HasAttributeSetForAttribute(SA.Attribute))
		{
			SA.SavedValue = RenamedAbilitySystem->GetNumericAttributeBase(SA.Attribute);
		}
	}

	OutAttributes = AttributesToSave;
	return OutAttributes.Num() > 0;
}

// Called when loading
void AGACharacter::PopulateSavedAttributes(const TArray<FSavedAttribute>& Attributes)
{
	if (RenamedAbilitySystem == nullptr)
	{
		return;
	}

	for (const FSavedAttribute& Attr : Attributes)
	{
		RenamedAbilitySystem->SetNumericAttributeBase(Attr.Attribute, Attr.SavedValue);
	}
}

// Called when damage exceeds health
void AGACharacter::Death(const AActor* ResponsibleActor, const AActor* Victim)
{
	bIsDead = true;
	GEngine->AddOnScreenDebugMessage(-2, 5, FColor::Blue, FString("GACharacter: HealthZeroedCalled"));
	UE_LOG(LogTemp, Warning, TEXT("Character has died..."));

	if (PC)
	{
		FInputModeUIOnly UIOnly;
		PC->SetInputMode(UIOnly);
	}

	FloatingBarComponent->DestroyAllBars();
	FVector LaunchVelocity;
	LaunchingComponent->EndLaunch(LaunchVelocity);
	GetMovementComponent()->StopMovementImmediately();
	GetMovementComponent()->Deactivate();

	GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("pelvis"), true, true);
	PhysicalAnimation->ApplyPhysicalAnimationProfileBelow(FName("pelvis"), FName("Default"), true);
	PhysicalAnimation->SetStrengthMultiplyer(1.0f);

	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetEnableGravityOnAllBodiesBelow(true, FName("pelvis"));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	OnDeath();
}
#pragma endregion

#pragma region Interaction
// */
//////////////////////////////////////////////////////
/////			CHARACTER INTERACTION			//////
//////////////////////////////////////////////////////

void AGACharacter::NotifyCanInteract_Implementation(FName InteractibleName, bool CanPickup)
{
}

void AGACharacter::AddItemToInventory_Implementation(AGAActor* Item)
{
	PickupItem(Item);
}

void AGACharacter::AddItemToRepository_Implementation(const FString& Category, const FItemKey& Item)
{
	AddItemRepository(Category, Item);
}

void AGACharacter::InteractedWith_Implementation(AActor* OtherActor)
{
	/*if (!DialogueParticipantInfo->Dialogue)
	{
		return;
	}*/

	if (PC)
	{
		return;
	}
	
	if (AC)
	{
		AGAPlayerController* OtherPC = Cast<AGAPlayerController>(OtherActor->GetInstigatorController());
		AGAAIController* OtherAC = Cast<AGAAIController>(OtherActor->GetInstigatorController());

		if (OtherPC)
		{
			// OtherPC->StartDialogue(DialogueParticipantInfo->Dialogue, this);
		}
		else if (OtherAC)
		{
			return;
		}
	}
}

// */
//////////////////////////////////////////////////////
/////			CHARACTER DIALOGUE				//////
//////////////////////////////////////////////////////

bool AGACharacter::ModifyIntValue_Implementation(const FName ValueName, bool bDelta, int32 Value)
{
	if (!DialogueParticipantInfo->DialogueData.Integers.Contains(ValueName))
	{
		DialogueParticipantInfo->DialogueData.Integers.Add(ValueName, 0);
	}

	if (bDelta)
	{
		DialogueParticipantInfo->DialogueData.Integers[ValueName] += Value;
	}
	else
	{
		DialogueParticipantInfo->DialogueData.Integers[ValueName] = Value;
	}

	return true;
}

bool AGACharacter::ModifyFloatValue_Implementation(const FName ValueName, bool bDelta, float Value)
{
	if (!DialogueParticipantInfo->DialogueData.Floats.Contains(ValueName))
	{
		DialogueParticipantInfo->DialogueData.Floats.Add(ValueName, 0);
	}

	if (bDelta)
	{
		DialogueParticipantInfo->DialogueData.Floats[ValueName] += Value;
	}
	else
	{
		DialogueParticipantInfo->DialogueData.Floats[ValueName] = Value;
	}

	return true;
}

bool AGACharacter::ModifyBoolValue_Implementation(const FName ValueName, bool bValue)
{
	if (bValue)
	{
		DialogueParticipantInfo->DialogueData.TrueBools.Add(ValueName);
	}
	else
	{
		DialogueParticipantInfo->DialogueData.TrueBools.Remove(ValueName);
	}

	return true;
}

bool AGACharacter::ModifyNameValue_Implementation(const FName ValueName, const FName NameValue)
{
	if (DialogueParticipantInfo->DialogueData.Names.Contains(ValueName))
	{
		DialogueParticipantInfo->DialogueData.Names[ValueName] = NameValue;
	}
	else
	{
		DialogueParticipantInfo->DialogueData.Names.Add(ValueName, NameValue);
	}

	return true;
}

int32 AGACharacter::GetIntValue_Implementation(const FName ValueName) const
{
	return DialogueParticipantInfo->DialogueData.Integers.Contains(ValueName) ? DialogueParticipantInfo->DialogueData.Integers[ValueName] : 0;
}

float AGACharacter::GetFloatValue_Implementation(const FName ValueName) const
{
	return DialogueParticipantInfo->DialogueData.Floats.Contains(ValueName) ? DialogueParticipantInfo->DialogueData.Floats[ValueName] : 0.f;
}

bool AGACharacter::GetBoolValue_Implementation(const FName ValueName) const
{
	return DialogueParticipantInfo->DialogueData.TrueBools.Contains(ValueName);
}

FName AGACharacter::GetNameValue_Implementation(const FName ValueName) const
{
	return DialogueParticipantInfo->DialogueData.Names.Contains(ValueName) ? DialogueParticipantInfo->DialogueData.Names[ValueName] : NAME_None;
}
// 
#pragma endregion

#pragma region ItemHandling
// */
//////////////////////////////////////////////////////
/////			CHARACTER INVENTORY				//////
//////////////////////////////////////////////////////

AGAActor* AGACharacter::GetHeldItem() const
{
	FString HeldItemSlotName = FString("HeldItem");
	return Cast<AGAActor>(Equipment->GetItemInNameSlot(HeldItemSlotName));
}

AGAActor* AGACharacter::GetStowedItem() const
{
	FString StowedItemSlotName = FString("StowedItem");
	return Cast<AGAActor>(Equipment->GetItemInNameSlot(StowedItemSlotName));
}

void AGACharacter::PickupItem(AActor* Item)
{
	if (Item)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString("We want to pick " + Item->GetName() + " up!"));
		}

		if (UPickupComponent* Pickup = Item->GetComponentByClass<UPickupComponent>())
		{
			Pickup->Pickup();
		}

		if (!GetHeldItem())
		{
			EquipItem(Item);
		}
		else if (!GetStowedItem())
		{
			StowItem(Item);
		}
		else if (Inventory->AddItem(Item))
		{
			Item->SetActorHiddenInGame(true);
		}
	}
}

void AGACharacter::EquipItem(AActor* Item)
{
	if (Item)
	{
		UEquippableComponent* Equippable = Item->GetComponentByClass<UEquippableComponent>();
		if (Equippable)
		{
			FString SlotName = "HeldItem";
			Equipment->EquipItem(Item, SlotName);
			Equippable->Equipped(SlotName);
		}
	}
}

void AGACharacter::StowItem(AActor* Item)
{
	if (Item)
	{
		FAttachmentTransformRules ItemAttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
		Item->AttachToComponent(this->GetMesh(), ItemAttachmentRules, this->GetMesh()->DoesSocketExist(FName("spine_03")) ? FName() : FName());

		FString SlotName = FString("StowedItem");
		Equipment->EquipItem(Item, SlotName);
		// Item->OnStowed();
	}
}

void AGACharacter::SwitchEquippedItems()
{
	AGAActor* TempItem = GetStowedItem();
	GetHeldItem()->OnUnEquipped();
	if (PC) PC->OnPostProcessInput.RemoveAll(GetHeldItem()->GetAbilitySystemComponent());
	StowItem(GetHeldItem());
	EquipItem(TempItem);
}

void AGACharacter::EquipItemFromInventory(uint8 Slot)
{
	AActor* TempItem = Inventory->GetItem(Slot);
	Inventory->RemoveItem(Slot);
	AddEquippedItemToInventory();
	EquipItem(TempItem);
}

void AGACharacter::StowItemFromInventory(uint8 Slot)
{
	AActor* TempItem = Inventory->GetItem(Slot);
	Inventory->RemoveItem(Slot);
	AddStowedItemToInventory();
	StowItem(TempItem);
}

void AGACharacter::AddStowedItemToInventory()
{
	if (AGAActor* StowedItem = GetStowedItem())
	{
		StowedItem->SetActorHiddenInGame(true);
		Inventory->AddItem(StowedItem);
		FString SlotName = FString("StowedItem");
		Equipment->RemoveItem(SlotName);
	}
}

void AGACharacter::AddEquippedItemToInventory()
{
	if (AGAActor* HeldItem = GetHeldItem())
	{
		HeldItem->OnUnEquipped();
		HeldItem->SetActorHiddenInGame(true);
		Inventory->AddItem(HeldItem);
		FString SlotName = FString("HeldItem");
		Equipment->RemoveItem(SlotName);
	}
}

void AGACharacter::MoveItemWithinInventory(uint8 From, uint8 To)
{
	Inventory->MoveItem(From, To);
}

void AGACharacter::DropEquippedItem(const FVector& DropVelocity)
{
	if (AGAActor* HeldItem = GetHeldItem())
	{
		FVector EyesLocation;
		FRotator EyesRotation;
		GetActorEyesViewPoint(EyesLocation, EyesRotation);
		FVector ItemLocation = EyesLocation + GetActorForwardVector() * 25;

		FString SlotName = FString("HeldItem");
		Equipment->RemoveItem(SlotName);

		if (UEquippableComponent* Equippable = HeldItem->GetComponentByClass<UEquippableComponent>())
		{
			Equippable->UnEquipped();
		}

		if (UPickupComponent* Pickup = HeldItem->GetComponentByClass<UPickupComponent>())
		{
			Pickup->Drop();
		}

		HeldItem->SetActorLocation(ItemLocation);
		HeldItem->LaunchItem(DropVelocity, true, true);
	}
}

void AGACharacter::DropItemFromInventory(uint8 Slot)
{
	AActor* Item = Inventory->GetItem(Slot);

	float RandomX = FMath::RandRange(-100, 100);
	float RandomY = FMath::RandRange(-100, 100);
	FVector2D NewLocation = FVector2D(RandomX, RandomY).GetSafeNormal() * ItemDropRadius;

	FVector ItemLocation = GetActorLocation() + FVector(NewLocation, 0);

	Item->SetActorLocation(ItemLocation); 
	if (UPickupComponent* Pickup = Item->GetComponentByClass<UPickupComponent>())
	{
		Pickup->Drop();
	}

	Inventory->RemoveItem(Slot);
}

// */
//////////////////////////////////////////////////////
/////			CHARACTER REPOSITORY			//////
//////////////////////////////////////////////////////

bool AGACharacter::GetAllItems(const FString Category, TArray<FItemKey>& Items) const
{
	if (Repository->GetAllItems(Category, Items)) return true;

	return false;
}

void AGACharacter::AddItemRepository(const FString Category, const FItemKey& Item)
{
	Repository->AddItem(Category, Item);
}

void AGACharacter::AddItemRepository(const FString Category, const FName Name, const uint8 Quantity)
{
	Repository->AddItem(Category, Name, Quantity);
}

void AGACharacter::SubtractItemRepository(const FString Category, const FItemKey& Item)
{
	Repository->SubtractItem(Category, Item);
}

void AGACharacter::SubtractItemRepository(const FString Category, const FName Name, const uint8 Quantity)
{
	Repository->SubtractItem(Category, Name, Quantity);
}

void AGACharacter::SubtractItemAtIndexRepository(const FString Category, const uint8 Index, const uint8 Quantity)
{
	Repository->SubtractItemAtIndex(Category, Index, Quantity);
}

bool AGACharacter::GetItemQuantityFromRepository(const FString Category, const int32 Index, int32& Quantity) const
{
	return Repository->GetItemQuantity(Category, Index, Quantity);
}

void AGACharacter::SpawnItemFromRepository()
{

}

// */
//////////////////////////////////////////////////////
/////			CHARACTER QUICK SELECT			//////
//////////////////////////////////////////////////////



#pragma endregion

#pragma region Climbing
// Start climbing
void AGACharacter::BeginGrip()
{
	if(bIsDominantGripping)
	{

	}
	else
	{

	}
}

// Continue climbing
void AGACharacter::SwapGrip()
{
	if (bIsDominantGripping)
	{
		bIsDominantGripping = false;

		GripConstraint_s = GetWorld()->SpawnActor<APhysicsConstraintActor>();

		bIsSubmisiveGripping = true;
	}
	else
	{
		bIsSubmisiveGripping = false;



		bIsDominantGripping = true;
	}
}

// Stop Climbing
void AGACharacter::EndGrip()
{
	bIsClimbing = false;
}
#pragma endregion

#pragma region AIReactionEvents
// */
//////////////////////////////////////////////////////
/////			CHARACTER SENSES				//////
//////////////////////////////////////////////////////

// Called to update sight component to match control rotation
void AGACharacter::UpdateSightRotation()
{
	if (PC)
	{
		FRotator DesiredRotation = PC->PlayerCameraManager->GetCameraRotation();
		// AISenses->SetWorldRotation(DesiredRotation);
	}
	else if (AC)
	{

	}
}

// Called on Attribute decrease
/*void AGACharacter::OnDamaged(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue)
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Damage was taken!!");

	SpawnNumbers(DeltaAmount);
	SpawnValueBar(AttributeType, NewValue, AIInfo.CurrentAITargetStats.MaxHealth);

	if (PC)
	{
		PC->OnDamaged_Client(SourceActor, AttributeType, DeltaAmount, NewValue);
	}
	else if (AC)
	{
		if (SourceActor == this)
		{
			return;
		}

		if (AGACharacter* DamageCharacter = Cast<AGACharacter>(SourceActor))
		{
			bool bIsNewInstigator = true;

			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Damage was taken by an AI!!");

			for (FAICharacterInfo VisibleCharacter : AwareOfCharacters)
			{
				if (VisibleCharacter.Target == DamageCharacter)
				{
					switch (AttributeType)
					{
					case EAttributeType::Health:
						VisibleCharacter.CurrentAIRelationshipTowardTarget.HealthDamaged += DeltaAmount;
						break;
					case EAttributeType::Stamina:
						VisibleCharacter.CurrentAIRelationshipTowardTarget.StaminaDamaged += DeltaAmount;
						break;
					case EAttributeType::Mana:
						VisibleCharacter.CurrentAIRelationshipTowardTarget.ManaDamaged += DeltaAmount;
						break;
					}

					FAICharacterAwareness& AIState = VisibleCharacter.CurrentAIAwarenessTowardTarget;

					AIState.SetCurrentSuspicion(AIState.GetCurrentSuspicion() + DeltaAmount);
				}
			}
		}

		AC->OnDamaged(SourceActor, AttributeType, DeltaAmount, NewValue);
	}

	// AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser
	// UGameplayStatics::ApplyDamage(this, DeltaAmount, SourceActor->GetInstigatorController(), SourceActor, UDamageType::StaticClass());
}*/

// Called when sensed by another character
void AGACharacter::OnSensed(const USensorBase* Sensor, int32 Channel, EOnSenseEvent SenseEvent)
{
	switch (SenseEvent)
	{
	case EOnSenseEvent::SenseNew:
		if (Sensor->SensorTag.IsEqual(FName("SensorHearing")))
		{
			OnHeard();
		}
		else if (Sensor->SensorTag.IsEqual(FName("SensorSight")))
		{
			OnSeen();
		}
		break;
	}
}

// Called when heard by another character
void AGACharacter::OnHeard()
{

}

// Called when Seen by another character
void AGACharacter::OnSeen()
{

}

void AGACharacter::CheckVisibleCharacters()
{
	/* for (FAICharacterInfo VisibleCharacter : AwareOfCharacters)
	{
		FVector Start = AISenses->GetComponentLocation();
		FVector End = VisibleCharacter.Target->AISenses->GetComponentLocation();
		FVector End2 = VisibleCharacter.Target->GetActorLocation();

		FHitResult HitResult;
		FHitResult HitResult2;

		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
		GetWorld()->LineTraceSingleByChannel(HitResult2, Start, End2, ECollisionChannel::ECC_Visibility);

		bool bIsVisible = false;

		if (HitResult.bBlockingHit)
		{
			// Can we see their head
			if (HitResult.GetActor() == VisibleCharacter.Target)
			{
				FVector Vector1 = AISenses->GetComponentRotation().Vector().GetSafeNormal();
				FVector Vector2 = (End - Start).GetSafeNormal();
				float Angle = 
					(FVector::CrossProduct(Vector1, Vector2).Z > 0 ? 1 : -1)
					* (FMath::Acos( FVector::DotProduct(Vector1, Vector2) ));

				// if (abs(Angle) < AISenses->GetFoV())
				{
					bIsVisible = true;
				}
			}
		}
		else if (HitResult2.bBlockingHit)
		{
			// Can we see their feet
			if (HitResult.GetActor() == VisibleCharacter.Target)
			{
				FVector Vector1 = AISenses->GetComponentRotation().Vector().GetSafeNormal();
				FVector Vector2 = (End - Start).GetSafeNormal();
				float Angle =
					(FVector::CrossProduct(Vector1, Vector2).Z > 0 ? 1 : -1)
					* (FMath::Acos(FVector::DotProduct(Vector1, Vector2)));

				// if (abs(Angle) < AISenses->GetFoV())
				{
					bIsVisible = true;
				}
			}
		}
		
		FAICharacterAwareness& AIState = VisibleCharacter.CurrentAIAwarenessTowardTarget;

		if (!bIsVisible)
		{
			if (AIState.CurrentAwarenessState == EAIAwarenessState::UnAware)
			{
				GetWorldTimerManager().ClearTimer(VisibleCharacter.AwarenessTimerHandle);
				AwareOfCharacters.Remove(VisibleCharacter);
			}
			else
			{
				AIState.SetCurrentSuspicion(AIState.GetCurrentSuspicion() - 1);
			}
		}
		else
		{
			if (AIState.GetCurrentSuspicion() >= 15)
			{
				AIState.SetCurrentSuspicion(15);
			}
			else
			{
				VisibleCharacter.Target->CalculateShadowed();

				if (bShadowed)
				{
					AIState.SetCurrentSuspicion(1 + AIState.GetCurrentSuspicion());
				}
				else
				{
					AIState.SetCurrentSuspicion(4 + AIState.GetCurrentSuspicion());
				}

			}
		}

		DialogueParticipantInfo->Relationships.Contains(VisibleCharacter.Target);
	} // */
}

void AGACharacter::CalculateShadowed()
{
	/*if (!Sun)
	{
		TArray<AActor*> TempActorArray;
		UGameplayStatics::GetAllActorsOfClass(this, ADirectionalLight::StaticClass(), TempActorArray);
		if (TempActorArray[0])
		{
			Sun = TempActorArray[0];
		}
		else
		{
			return;
		}
	}

	FVector Start = Sight->GetComponentLocation();
	FVector End = Start + Sun->GetActorRotation().Vector() * -5000;

	FHitResult HitResult;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);

	FVector Start2 = GetActorLocation();
	FVector End2 = Start2 + Sun->GetActorRotation().Vector() * -5000;

	FHitResult HitResult2;

	GetWorld()->LineTraceSingleByChannel(HitResult2, Start2, End2, ECollisionChannel::ECC_Visibility);

	if (HitResult.bBlockingHit)
	{
		if (HitResult2.bBlockingHit)
		{
			Shadowed = 2;
		}
		else
		{
			Shadowed = 1;
		}
	}
	else if(HitResult2.bBlockingHit)
	{
		Shadowed = 1;
	}
	else
	{
		Shadowed = 0;
	}*/
}

void AGACharacter::UpdateCurrentBuilding(AGameplayBuilding* CurrentBuilding)
{
	UE_LOG(LogTemp, Warning, TEXT("Updated Current Building!"));
	if (!CurrentBuilding) return;

	if (PC)
	{
		// PC->OnUpdateCurrentBuilding_Client(CurrentBuilding->BuildingName);
	}
	else if (AC)
	{
		AC->UpdateCurrentBuilding(CurrentBuilding);
	}
}

void AGACharacter::UpdateDesiredLocation(FVector DesiredLocation)
{
	if (PC)
	{
		// PC->OnDesiredLocationSet_Client(DesiredLocation);
	}
	else if (AC)
	{
		AC->UpdateDesiredLocation(DesiredLocation);
	}
}

void AGACharacter::UpdateTargetBuilding(AGameplayBuilding* TargetBuilding)
{
	if (PC)
	{
		// PC->OnUpdateTargetBuilding_Client(TargetBuilding->BuildingName);
	}
	else if (AC)
	{
		AC->UpdateTargetBuilding(TargetBuilding);
	}
}

FTransform AGACharacter::GetPawnSenseTransform()
{
	FTransform ReturnTransform;
	return ReturnTransform;
}
#pragma endregion
