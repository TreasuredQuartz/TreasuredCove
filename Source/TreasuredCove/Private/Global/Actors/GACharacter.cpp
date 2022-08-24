// Fill out your copyright notice in the Description page of Project Settings.

// Gameplay Ability Dependencies
#include "GACharacter.h"
#include "GAActor.h"
#include "GAWeapon.h"
#include "GAAIController.h"
#include "GAPlayerController.h"

// Attribute sets
#include "ASHealth.h"
#include "ASAmmo.h"
#include "ASStats.h"
#include "ASUltimate.h"
#include "ASWeaponStats.h"

// Abilities
#include "GameplayAbilityBase.h"
#include "AbilitySet.h"
#include "GA_DecreaseSpread.h"
#include "GA_IncreaseSpread.h"
#include "GA_Interact.h"

// Custom Components
#include "GACharacterMovementComponent.h"
#include "GASystemComponent.h"
#include "GASkillTreeComponent.h"
#include "ComboComponent.h"
#include "TownSystemComponent.h"
#include "SenseComponent.h"
#include "PickupInterface.h"
#include "InventoryComponent.h"
#include "RepositoryComponent.h"
#include "PickupMagnetComponent.h"
#include "CraftingComponent.h"
#include "FloatingTextComponent.h"
#include "MaterialTrailComponent.h"

// Misc
#include "GameplayBuilding.h"

#include "GravityWidget.h"
#include "GAWidget.h"

#include "Animation/AnimInstance.h"
#include "GameFramework/DamageType.h"

#include "CustomMovementMode.h"

// Engine Components
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/PhysicsConstraintActor.h"
#include "GameFramework/CharacterMovementComponent.h"

// Procedural Mesh Plugin
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODRenderData.h"

// Global
#include "AbilitySystemGlobals.h"
#include "DrawDebugHelpers.h"
#include "GameplayAbilitiesModule.h"
#include "Kismet/GameplayStatics.h"
#include "StaticLibrary.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Engine/DirectionalLight.h"
#include "UObject/ConstructorHelpers.h"

#pragma region Defaults
// Sets default values
AGACharacter::AGACharacter(const FObjectInitializer& ObjectInitializer) : 
	ACharacter(ObjectInitializer.SetDefaultSubobjectClass<UGACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	// Mesh
	/*RealMesh = 
		CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Real Mesh"));*/

	// Combo System Component
	ComboSystem =
		CreateDefaultSubobject<UComboComponent>(TEXT("Combo System"));

	// Ability System Component
	RenamedAbilitySystem =
		CreateDefaultSubobject<UGASystemComponent>(TEXT("Ability System"));
	RenamedAbilitySystem->SetIsReplicated(true);

	// Skill Tree Component
	SkillTrees =
		CreateDefaultSubobject<UGASkillTreeComponent>(TEXT("Skill Trees"));

	// Pawn Sensing Component
	Sight = 
		CreateDefaultSubobject<USenseComponent>(TEXT("Senses"));
	Sight->OwningCharacter = this;
	Sight->SetupAttachment(RootComponent);
	// Sight->SetRelativeRotation(FRotator(0, 0, -90));
	Sight->SetRelativeLocation(FVector(0, 0, 68));

	// Inventory Component
	Inventory =
		CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	Inventory->SetInventorySize(5);

	Repository =
		CreateDefaultSubobject<URepositoryComponent>(TEXT("Repository"));
	Magnet =
		CreateDefaultSubobject<UPickupMagnetComponent>(TEXT("Magnet"));
	// Magnet->SetupAttachment(RootComponent);

	// Crafting Component
	Crafting =
		CreateDefaultSubobject<UCraftingComponent>(TEXT("Crafting"));

	// Dialogue System Component
	DialogueParticipantInfo =
		CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueParticipantInfo"));

	// Floating Text Component
	FloatingTextComponent =
		CreateDefaultSubobject<UFloatingTextComponent>(TEXT("Floating Text"));

	// Movement Trail Component
	/*static ConstructorHelpers::FObjectFinder<UParticleSystem> PSTrailObject(TEXT("/Game/GameplayAbilities/FX/ParticleSystems/PS_AffectingTrail"));
	PSTrail =
		CreateDefaultSubobject<UParticleSystemComponent>("PSTrail");
	PSTrail->SetupAttachment(RootComponent);
	PSTrail->bOwnerNoSee = true;
	PSTrail->ComponentTags.Add("GrassAffector");
	PSTrail->ComponentTags.Add("WaterAffector");

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PSPunctureObject(TEXT("/Game/GameplayAbilities/FX/ParticleSystems/PS_RainbowBurst"));
	PSPuncture =
		CreateDefaultSubobject<UParticleSystemComponent>("PSPuncture");
	PSPuncture->SetAutoActivate(false);

	if (PSTrailObject.Succeeded())
	{
		PSTrail->SetTemplate(PSTrailObject.Object);
	}

	if (PSPunctureObject.Succeeded())
	{
		PSPuncture->SetTemplate(PSPunctureObject.Object);
		PSPuncture->SetHiddenInGame(true);
	}*/

	/* Default Character values */

	// Default AIController
	TSubclassOf<AGAAIController> DefaultAIC;
	AIControllerClass = DefaultAIC;

	// IKFootTrace Variables
	Scale = GetActorTransform().GetScale3D().Z;
	IKFootTraceDistance = GetCapsuleComponent()->GetScaledCapsuleHalfHeight()* Scale;

	// Name values
	Tags.Add(FName("GrassAffector"));
	DominantHand = FName("grip_r");
	HeadSocket = FName("head");

	TeamID = 255;

	SubMenuCount = 1;
	ActiveMenuCount = 1;

	DecreaseSpreadAbility = UGA_DecreaseSpread::StaticClass();
	IncreaseSpreadAbility = UGA_IncreaseSpread::StaticClass();
	ActiveAbilityHandles.SetNum(9);

	// Mesh Collision To Block Visibility Channel
	if (GetMesh())
	{
		// RealMesh->SetupAttachment(GetMesh());

		if (GetMesh()->DoesSocketExist(HeadSocket))
			Sight->SetupAttachment(GetMesh(), HeadSocket);

		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	}
}

// Called when the game starts or when spawned
void AGACharacter::BeginPlay()
{
	Super::BeginPlay();

	ChangeCameraType(bFirstPerson);

	if (WallRunTimelineCurve) 
	{
		FOnTimelineFloat TimelineCallback;
		// FOnTimelineEventStatic TimelineFinishedCallback;

		TimelineCallback.BindUFunction(this, FName("UpdateCameraTilt"));
		// TimelineFinishedCallback.BindUFunction(this, FName(TEXT("")));

		WallRunCameraTiltTimeline.AddInterpFloat(WallRunTimelineCurve, TimelineCallback);
		WallRunCameraTiltTimeline.SetLooping(true);
	}

	SightInitialRotation = Sight->GetComponentRotation();

	AutoDetermineTeamIDByControllerType();
	PC = Cast<AGAPlayerController>(GetController());
	AC = Cast<AGAAIController>(GetController());
	
	if (PC) PC->SetItemPriority(bWeaponPriority);

	bIsPaused = false;

	if (TownInfo != nullptr)
	{
		TownInfo->UpdateDesiredLocation.AddDynamic(this, &AGACharacter::UpdateDesiredLocation);
		TownInfo->UpdateCurrentBuilding.AddDynamic(this, &AGACharacter::UpdateCurrentBuilding);
		TownInfo->UpdateTargetBuilding.AddDynamic(this, &AGACharacter::UpdateTargetBuilding);
	}

	if (HeldItem)
	{
		EquipItem(HeldItem);
	}

	check(RenamedAbilitySystem);
}

// Called every frame
void AGACharacter::Tick(float InDeltaTime)
{
	Super::Tick(InDeltaTime);
	DeltaTime = InDeltaTime;

	WallRunCameraTiltTimeline.TickTimeline(DeltaTime);
	// ClampHorizontalVelocity();

	/*if (CumalitiveAbilityHandles.Num() > 0)
	{
		for (FGameplayAbilitySpecHandle Handle : CumalitiveAbilityHandles)
		{
			if (Handle.IsValid())
				RenamedAbilitySystem->TryActivateAbility(Handle);
		}
	}*/
	/*if (HeldItem)
	{
		if (ItemCumalitiveAbilityHandles.Num() > 0)
		{
			for (FGameplayAbilitySpecHandle Handle : ItemCumalitiveAbilityHandles)
			{
				if (Handle.IsValid())
					RenamedAbilitySystem->TryActivateAbility(Handle);
			}
		}
	}*/
	/*if (GetVelocity().Size() > 0)
	{
		RenamedAbilitySystem->TryActivateAbility(IncreaseSpreadAbilityHandle);
	}
	else
	{
		RenamedAbilitySystem->TryActivateAbility(DecreaseSpreadAbilityHandle);
	}*/

	if (bIsWalking)
	{
		float Result = IKFootTrace(IKFootTraceDistance, FName("foot_r"));
		IKOffsetRightFoot = FMath::FInterpTo(IKOffsetRightFoot, Result, DeltaTime, IKInterpSpeed);

		Result = IKFootTrace(IKFootTraceDistance, FName("foot_l"));
		IKOffsetLeftFoot = FMath::FInterpTo(IKOffsetLeftFoot, Result, DeltaTime, IKInterpSpeed);
	}

}

// Called to bind functionality to input
void AGACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("QuickSelect", IE_Pressed, this, &AGACharacter::BeginQuickSelect);
	PlayerInputComponent->BindAction("QuickSelect", IE_Released, this, &AGACharacter::EndQuickSelect);

	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AGACharacter::BeginPause);
	PlayerInputComponent->BindAction("Pause", IE_Released, this, &AGACharacter::EndPause);

	PlayerInputComponent->BindAction("Primary", IE_Pressed, this, &AGACharacter::BeginPrimary);
	PlayerInputComponent->BindAction("Primary", IE_Released, this, &AGACharacter::EndPrimary);

	PlayerInputComponent->BindAction("Secondary", IE_Pressed, this, &AGACharacter::BeginSecondary);
	PlayerInputComponent->BindAction("Secondary", IE_Released, this, &AGACharacter::EndSecondary);

	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &AGACharacter::BeginThrow);
	PlayerInputComponent->BindAction("Throw", IE_Released, this, &AGACharacter::EndThrow);

	PlayerInputComponent->BindAction("Melee", IE_Pressed, this, &AGACharacter::BeginMelee);
	PlayerInputComponent->BindAction("Melee", IE_Released, this, &AGACharacter::EndMelee);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AGACharacter::BeginInteract);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AGACharacter::EndInteract);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AGACharacter::BeginJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AGACharacter::EndJump);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AGACharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AGACharacter::EndCrouch);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AGACharacter::BeginSprint);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AGACharacter::EndSprint);

	PlayerInputComponent->BindAxis("Switch", this, &AGACharacter::Switch);

	PlayerInputComponent->BindAxis("MenuUp", this, &AGACharacter::MenuUp);
	PlayerInputComponent->BindAxis("MenuRight", this, &AGACharacter::MenuRight);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGACharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGACharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookRight", this, &AGACharacter::LookRight);
	PlayerInputComponent->BindAxis("LookRightRate", this, &AGACharacter::LookRight);

	PlayerInputComponent->BindAxis("LookUp", this, &AGACharacter::LookUp);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGACharacter::LookUp);
}

// Called when possessed by a new controller
void AGACharacter::PossessedBy(AController* NewController)
{
	UE_LOG(LogTemp, Warning, TEXT("Possessed..."));

	if (RenamedAbilitySystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability System initializing..."));
		InitializeAbilitySystem();
	}

	Super::PossessedBy(NewController);
}

// Called when control is taken away from this pawn
void AGACharacter::UnPossessed()
{
	Super::UnPossessed();
}

// Called 
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
			InitProceduralMesh();
		}
		else
		{
			UpdateProceduralMesh();
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

#pragma region AnimationInverseKinematics
// */
//////////////////////////////////////////////////////
/////			IK HELPER FUNCTIONS				//////
//////////////////////////////////////////////////////

float AGACharacter::IKFootTrace(float Distance, FName Bone)
{
	FVector LocationA = GetMesh()->GetBoneLocation(Bone);
	FVector LocationB = GetActorLocation();

	FHitResult Hit;
	FVector Start = FVector(LocationA.X, LocationA.Y, LocationB.Z);
	FVector End = FVector(LocationA.X, LocationA.Y, LocationB.Z - Distance);
	FCollisionQueryParams CollisionParems = FCollisionQueryParams(FName("IKTrace"), true, this);

	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility, CollisionParems);
	// DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 1, 0, 1);

	float Result = 0.0f;

	if (Hit.bBlockingHit)
	{
		// DrawDebugPoint(GetWorld(), Hit.Location, 10, FColor::Magenta, true, 1);
		FVector Difference = End - Hit.Location;
		Result = Difference.Size() / Scale;
	}

	return Result;
}

float AGACharacter::IKFingerTrace(float Distance, FName Bone)
{
	FVector LocationA = GetMesh()->GetBoneLocation(Bone);

	FHitResult Hit;
	FVector Start = FVector(LocationA.X, LocationA.Y, 0);
	FVector End = FVector(LocationA.X, LocationA.Y, 0 - Distance);
	FCollisionQueryParams CollisionParems = FCollisionQueryParams(FName("IKTrace"), true, this);

	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility, CollisionParems);
	DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 1, 0, 1);

	float Result = 0.0f;

	if (Hit.bBlockingHit)
	{
		FVector Difference = End - Hit.Location;
		Result = Difference.Size();
	}
	else
	{
		FVector FingerVector;
		Start = FVector(Hit.Location);
		End = FVector(Hit.Location + FVector());
		CollisionParems = FCollisionQueryParams(FName("IKTrace"), true, this);

		GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility, CollisionParems);
		DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 1, 0, 1);

		if (Hit.bBlockingHit)
		{
			FVector Difference = End - Hit.Location;
			Result = Difference.Size();
		}
		else
		{
			Start = FVector(Hit.Location);
			End = FVector(Hit.Location + FVector());
			CollisionParems = FCollisionQueryParams(FName("IKTrace"), true, this);

			GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility, CollisionParems);
			DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 1, 0, 1);

			if (Hit.bBlockingHit)
			{
				FVector Difference = End - Hit.Location;
				Result = Difference.Size();
			}
		}
	}

	return Result;
}

float AGACharacter::IKHandToLocation(FVector Location, FName Bone)
{
	FVector LocationA = GetMesh()->GetBoneLocation(Bone);

	FHitResult Hit;
	FVector Start = FVector(LocationA);
	FVector End = FVector(Location);
	FCollisionQueryParams CollisionParems = FCollisionQueryParams(FName("IKTrace"), true, this);

	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility, CollisionParems);
	DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 1, 0, 1);

	float Result = 0.0f;

	if (Hit.bBlockingHit)
	{
		FVector Difference = End - Hit.Location;
		Result = Difference.Size() / Scale;
	}

	return Result;
}
#pragma endregion

#pragma region VisualEffects
// */
//////////////////////////////////////////////////////
/////			CHARACTER VISUAL FX   			//////
//////////////////////////////////////////////////////

void AGACharacter::SpawnNumbers(float Numbers)
{
	FloatingTextComponent->AddFloatingText_Client(FText::FromString(FString::SanitizeFloat(Numbers)), GetActorLocation());
}

void AGACharacter::SpawnValueBar(EAttributeType BarType, float CurrentValue, float MaxValue)
{
	if (!OnDamagedHealthBar)
	{
		OnDamagedHealthBar = GetWorld()->SpawnActor<AGravityWidget>(OnDamagedHealthBarClass,
			FTransform(FRotator(), GetActorLocation(), FVector()));
		OnDamagedHealthBar->Gravity = FVector(0);
		OnDamagedHealthBar->SetOwner(this);
		OnDamagedHealthBar->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::SnapToTarget, true),
			GetMesh()->DoesSocketExist(HeadSocket) ? HeadSocket : FName());
	}
}

void AGACharacter::InitProceduralMesh()
{
	/*
	if (!GetMesh())
	{
		return;
	}

	FSkeletalMeshRenderData* RenderData =
		GetMesh()->GetSkeletalMeshRenderData();

	if (!RenderData)
	{
		return;
	}

	FSkeletalMeshLODRenderData& DataArray =
		RenderData->LODRenderData[0];

	int32 A =
		DataArray.RenderSections[0].NumVertices;

	FSkinWeightVertexBuffer* SkinBuffer = GetMesh()->GetSkinWeightBuffer(0);

	if (SkinBuffer == nullptr)
	{
		return;
	}

	for (int32 i = 0; i < A; i++)
	{
		FVector B =
			USkinnedMeshComponent::GetSkinnedVertexPosition(GetMesh()
			, i
			, DataArray
			, *SkinBuffer );
		FVector C =
			DataArray.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(i);
		FVector2D D =
			DataArray.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(i, 0);

		Vertices.Add(B);
		Normals.Add(C);
		UVs.Add(D);
	}

	FMultiSizeIndexContainerData IndicesData;
	DataArray.MultiSizeIndexContainer.GetIndexBufferData(IndicesData);

	for (int32 i = 0; i < IndicesData.Indices.Num(); i++)
	{
		int32 a = 0;
		UInt32ToInt32(IndicesData.Indices[i], &a);
		Tris.Add(a);
	}

	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	RealMesh->CreateMeshSection(0, Vertices, Tris, Normals, UVs, VertexColors, Tangents, false);
	*/
}

void AGACharacter::UpdateProceduralMesh()
{
	/*
	if (!GetMesh())
	{
		return;
	}

	FSkeletalMeshRenderData* RenderData =
		GetMesh()->GetSkeletalMeshRenderData();

	if (!RenderData)
	{
		return;
	}

	FSkeletalMeshLODRenderData& DataArray =
		RenderData->LODRenderData[0];

	int32 A =
		DataArray.RenderSections[0].NumVertices;

	if (!GetMesh()->GetSkinWeightBuffer(0))
	{
		return;
	}

	for (int32 i = 0; i < A; i++)
	{
		FVector B =
			GetMesh()->GetSkinnedVertexPosition(GetMesh()
				, i
				, DataArray
				, *(GetMesh()->GetSkinWeightBuffer(0)));
		FVector C =
			DataArray.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(i);
		FVector2D D =
			DataArray.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(i, 0);

		Vertices[i] = B;
		Normals[i] = C;
		UVs[i] = D;
	}

	FMultiSizeIndexContainerData IndicesData;
	DataArray.MultiSizeIndexContainer.GetIndexBufferData(IndicesData);

	for (int32 i = 0; i < IndicesData.Indices.Num(); i++)
	{
		int32 a = 0;
		UInt32ToInt32(IndicesData.Indices[i], &a);
		Tris[i] = a;
	}

	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	RealMesh->UpdateMeshSection(0, Vertices, Normals, UVs, VertexColors, Tangents);

	bInitialized = true;
	*/
}

void AGACharacter::Puncture(FVector ImpactLocation, FVector ImpactNormal, FVector ImpactExitPoint, float ImpactRadius, FVector ImpactForce)
{
	// UStaticLibrary::HolePunchProceduralMesh(RealMesh, ImpactLocation, ImpactNormal, ImpactExitPoint, ImpactRadius, EProcMeshSliceCapOption::CreateNewSectionForCap, MeshInsideColor, 6);
	UParticleSystem* EmitterTemplate = PSPuncture->Template;
	PSPuncture =
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EmitterTemplate, FTransform(FRotator(), ImpactLocation, FVector(1, 1, 1)), true);
	PSPuncture->SetVectorParameter(FName("Velocity"), ImpactForce);
}

void AGACharacter::ChangeCameraType_Implementation(bool bInFirstPerson)
{
	bFirstPerson = bInFirstPerson;
	// bUseControllerRotationYaw = bFirstPerson;
}

void AGACharacter::BeginCameraTilt()
{
	/* Would like this on a timer
	* Not so much a timeline.
	CameraTiltTime = 0.0;
	FTimerDelegate TiltDel;
	TiltDel.BindUFunction(this, "UpdateCameraTilt", CameraTiltTime, increment, endValue, endTime);
	GetWorldTimerManager().SetTimer(CameraTiltTimerHandle, increment, true);
	*/

	WallRunCameraTiltTimeline.Play();
}

void AGACharacter::UpdateCameraTilt()
{
	float TimelineValue = WallRunCameraTiltTimeline.GetPlaybackPosition();
	float progress = WallRunTimelineCurve->GetFloatValue(TimelineValue);
	
	FRotator Rotation = GetController()->GetControlRotation();

	switch (WallRunSide)
	{
	case ENavType::Left:
		Rotation.Roll = -1 * progress;
			break;
	case ENavType::Right:
		Rotation.Roll = progress;
		break;
	}

	GetController()->SetControlRotation(Rotation);
}

void AGACharacter::EndCameraTilt()
{
	/* Prefer a timer, but I am not smart
	CameraTiltTime = endTime;
	FTimerDelegate TiltDel;
	TiltDel.BindUFunction(this, "UpdateCameraTilt", CameraTiltTime, increment, endValue, 0);
	GetWorldTimerManager().SetTimer(CameraTiltTimerHandle, increment, true);*/

	WallRunCameraTiltTimeline.Reverse();
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
	return TownInfo;
}

//
URepositoryComponent* AGACharacter::GetRepositoryComponent_Implementation() const
{
	return Repository;
}

//
UGACharacterMovementComponent* AGACharacter::GetGAMovementComponent() const
{
	return static_cast<UGACharacterMovementComponent*>(GetCharacterMovement());
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

// Called during begin play
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

		AddGameplayTag(FullHealthTag);
		AddGameplayTag(FullAmmoTag);
		AddGameplayTag(FullStaminaTag);
		AddGameplayTag(FullManaTag);
	}

	// AquireAbility(DecreaseSpreadAbility, EAbilityType::Cumalitive, DecreaseSpreadAbilityHandle, false);
	// AquireAbility(IncreaseSpreadAbility, EAbilityType::Reactive, IncreaseSpreadAbilityHandle, false);

	// if (InitialActiveAbilitySetClass) InitialActiveAbilities = NewObject<UAbilitySet>(InitialActiveAbilitySetClass);
	// 
	// This should be a UDataAsset Instance. Meaning there is no need to spawn anything
	// And data should be readily available for access.
	if (InitialActiveAbilities) AquireAbilities(InitialActiveAbilities->GetAbilities(), ActiveAbilityHandles);

	/*if (UGA_Interact::StaticClass() && (!ActiveAbilityHandles[0].IsValid()))
	{
		AquireAbility(UGA_Interact::StaticClass(), ActiveAbilityHandles[0]);
	}*/
}

// Called during ability system initialization
void AGACharacter::InitializeAttributeSet(UAttributeSet* Set)
{
	UASHealth* HealthSet = Cast<UASHealth>(Set);
	if (HealthSet)
	{
		HealthSet->OnDamaged.AddDynamic(this, &AGACharacter::OnDamaged);
		HealthSet->OnHealed.AddDynamic(this, &AGACharacter::OnHealed);

		HealthSet->OnHealthModified.AddDynamic(this, &AGACharacter::OnHealthModified);
		HealthSet->OnStaminaModified.AddDynamic(this, &AGACharacter::OnStaminaModified);
		HealthSet->OnManaModified.AddDynamic(this, &AGACharacter::OnManaModified);
		return;
	}

	UASStats* StatSet = Cast<UASStats>(Set);
	if (StatSet)
	{
		StatSet->OnExperienceModified.AddDynamic(this, &AGACharacter::OnExperienceModified);
		StatSet->OnStatModified.AddDynamic(this, &AGACharacter::OnStatModified);
		return;
	}

	UASAmmo* AmmoSet = Cast<UASAmmo>(Set);
	if (AmmoSet)
	{
		AmmoSet->OnAmmoModified.AddDynamic(this, &AGACharacter::OnAmmoModified);
		return;
	}

	UASWeaponStats* WeaponSet = Cast<UASWeaponStats>(Set);
	if (WeaponSet)
	{
		WeaponSet->OnSpreadAngleModified.AddDynamic(this, &AGACharacter::OnSpreadAngleModified);
		WeaponSet->OnHipAccuracyModified.AddDynamic(this, &AGACharacter::OnHipAccuracyModified);
		WeaponSet->OnADSAccuracyModified.AddDynamic(this, &AGACharacter::OnADSAccuracyModified);
		WeaponSet->OnHandlingModified.AddDynamic(this, &AGACharacter::OnHandlingModified);
		WeaponSet->OnRangeModified.AddDynamic(this, &AGACharacter::OnRangeModified);
		WeaponSet->OnDamageModified.AddDynamic(this, &AGACharacter::OnDamageModified);
		return;
	}

	UASUltimate* UltimateSet = Cast<UASUltimate>(Set);
	if (UltimateSet)
	{
		UltimateSet->OnUltimateChargeModified.AddDynamic(this, &AGACharacter::OnUltimateModified);
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

			// GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Purple, "Aquired Ability: " + AbilityName);

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
	/*else 
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Purple, "Ability System is null!!!");
	}*/
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
			PC->AddAbilityToUI(AbilityInfo, AbilityType, InHandle, bFromItem);
		}
	}
}

// Called to remove abilities from UI
void AGACharacter::RemoveAbilityFromUI(FGameplayAbilitySpecHandle InHandle)
{
	if (PC)
	{
		PC->RemoveAbilityFromUI(InHandle);
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
	if (bIsPaused)
	{
		if (PC)
		{
			PC->RecieveTabRequest(false);
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
			PC->SetItemPriority(bWeaponPriority);
		}
	}
}

void AGACharacter::EndQuickSelect()
{
	if (bIsPaused)
	{

	}
	else
	{
		if (!bToggleQuickSelect)
		{
			bWeaponPriority = true;

			if (PC)
			{
				PC->SetItemPriority(bWeaponPriority);
			}
		}
		else
		{

		}
	}
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

		if (bWeaponPriority && HeldItem && HeldItem->GetClass()->ImplementsInterface(UControlInputInterface::StaticClass()))
		{
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
			RenamedAbilitySystem->CancelAbilityHandle(ActiveAbilityHandles[0]);
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

		if (bWeaponPriority && HeldItem && HeldItem->GetClass()->ImplementsInterface(UControlInputInterface::StaticClass()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Use Item Secondary!"));
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
		GetWorldTimerManager().ClearTimer(InputTimerHandle);
	}
}

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
		
		if (StowedItem /*&& SwitchAbilityHandle.IsValid()*/)
		{
			// AbilitySystem->TryActivateAbility(SwitchAbilityHandle);
		
			SwitchEquippedItems();
		}
	}
}

void AGACharacter::EndSwitch()
{
	if (bIsPaused)
	{

	}
	else
	{
		if (HeldItem)
		{
			if (StowedItem)
			{
				SwitchEquippedItems();
			}
		}

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

		if (bWeaponPriority && HeldItem && HeldItem->GetClass()->ImplementsInterface(UControlInputInterface::StaticClass()))
		{
			IControlInputInterface::Execute_UseThrow(HeldItem);
		}
		else if (ActiveAbilityHandles.IsValidIndex(2))
		{
			RenamedAbilitySystem->TryActivateAbility(ActiveAbilityHandles[2]);
		}
	}
}

void AGACharacter::EndThrow()
{
	if (bIsPaused)
	{
		if (PC)
		{
			PC->RecieveTabRequest(true);
		}
	}
	else
	{
		RenamedAbilitySystem->TryActivateAbility(ActiveAbilityHandles[3]);

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

		if (bWeaponPriority && HeldItem && HeldItem->GetClass()->ImplementsInterface(UControlInputInterface::StaticClass()))
		{
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

		if (bWeaponPriority && HeldItem && HeldItem->GetClass()->ImplementsInterface(UControlInputInterface::StaticClass()))
		{
			IControlInputInterface::Execute_UseMelee(HeldItem);
		}

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

		if (bWeaponPriority && HeldItem && HeldItem->GetClass()->ImplementsInterface(UControlInputInterface::StaticClass()))
		{
			IControlInputInterface::Execute_UseUp(HeldItem, 1);
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

// Called When Pressing The "Jump" Action Key
void AGACharacter::BeginJump()
{
	if (bIsPaused)
	{
		if (PC)
		{
			PC->ConfirmPressed();
		}
	}
	else
	{
		if (CanJump()) Jump();
	}
}

// Called When Releasing The "Jump" Action Key
void AGACharacter::EndJump()
{
	if (bIsPaused)
	{
		if (PC)
		{
			PC->ConfirmReleased();
		}
	}
	else
	{
		StopJumping();
	}
}

//
void AGACharacter::ClampHorizontalVelocity()
{
	if (GetCharacterMovement()->IsFalling())
	{
		/*if (FVector2D(GetVelocity()).Length() / GetCharacterMovement()->GetMaxSpeed() > 1.0)
		{
			SetHorizontalVelocity(FVector2D(GetVelocity()) / GetCharacterMovement()->GetMaxSpeed());
		}*/
	}
}

//
void AGACharacter::OnBeginWallRun()
{
	BeginCameraTilt();
	bIsWallRunning = true;
}

//
void AGACharacter::OnEndWallRun()
{
	EndCameraTilt();
	bIsWallRunning = false;
}

// Called When Pressing The "Run" Action Key
void AGACharacter::BeginSprint()
{
	if (bIsPaused)
	{

	}
	else
	{
		if (bToggleSprint)
		{
			if (!bIsSprinting)
			{
				GetGAMovementComponent()->StartSprinting();
				bIsSprinting = true;
			}
			else
			{
				GetGAMovementComponent()->StopSprinting();
				bIsSprinting = false;
			}
		}
		else
		{
			GetGAMovementComponent()->StartSprinting();
			bIsSprinting = true;
		}
	}
}

// Called When Releasing The "Run" Action Key
void AGACharacter::EndSprint()
{
	if (bIsPaused)
	{

	}
	else
	{
		if (!bToggleSprint)
		{
			GetGAMovementComponent()->StopSprinting();
			bIsSprinting = false;
		}
	}
}

// Start crouching
void AGACharacter::BeginCrouch()
{
	if (bIsPaused)
	{
		if (PC)
		{
			PC->ReturnPressed();
		}
	}
	else
	{
		if (bIsSprinting)
		{
			BeginSlide();
		}
		else
		{
			if (bToggleCrouch)
			{
				if (!bIsCrouching)
				{
					Crouch();
					bIsCrouching = true;
				}
				else
				{
					UnCrouch();
					bIsCrouching = false;
				}
			}
			else
			{
				Crouch();
				bIsCrouching = true;
			}
		}
	}
}

// Stop crouching
void AGACharacter::EndCrouch()
{
	if (bIsPaused)
	{
		if (PC)
		{
			PC->ReturnReleased();
		}
	}
	else
	{
		if (bToggleCrouch)
		{
			return;
		}
		else
		{
			UnCrouch();
			bIsCrouching = false;
		}
	}
}

// Called to move the player's pawn forward
void AGACharacter::MoveForward(float Val)
{
	ForwardAxisValue = Val;

	if (Val != 0.0f)
	{
		if (bIsPaused)
		{

		}
		else
		{
			FVector Direction;
			FRotator Rotation;
			switch (GetCharacterMovement()->MovementMode)
			{
			case EMovementMode::MOVE_Custom:
				switch (GetCharacterMovement()->CustomMovementMode)
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
			default:
				Rotation = GetControlRotation();
				Rotation.Pitch = 0.0f;
				Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
			}

			AddMovementInput(Direction, Val);
		}
	}
}

// Called to move the player's pawn right
void AGACharacter::MoveRight(float Val)
{
	RightAxisValue = Val;

	if (Val != 0.0f)
	{
		if (bIsPaused)
		{

		}
		else
		{
			FVector Direction;
			FRotator Rotation;
			switch (GetCharacterMovement()->MovementMode)
			{
			case EMovementMode::MOVE_Custom:
				switch (GetCharacterMovement()->CustomMovementMode)
				{
				case ECustomMovementMode::MOVE_Climbing:
					Rotation = GetActorRotation();
					Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
					break;
				default:
					Rotation = Controller->GetControlRotation();
					Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
				}
				break;
			default:
				Rotation = Controller->GetControlRotation();
				Rotation.Pitch = 0.0f;
				Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
			}

			AddMovementInput(Direction, Val);
		}
	}
}

// Called to look up
void AGACharacter::LookUp(float Val)
{
	if (Val != 0)
	{
		if (bIsPaused)
		{
			if (PC)
				PC->CursorMoveUp(Val);
		}
		else
		{
			if (GetActorRotation().Pitch <= LookUpperLimit)
			{
				if (bIsUsingController)
				{
					AddControllerPitchInput(Val * VerticalLookRate);
				}
				else
				{
					AddControllerPitchInput(Val);
				}

				// Update Pitch on Server
				if (!IsLocallyControlled())
				{
					FRotator NewRot = GetMesh()->GetRelativeRotation();
					NewRot.Pitch = RemoteViewPitch * 360 / 255;

					GetMesh()->SetRelativeRotation(NewRot);
				}

				UpdateSightRotation();
			}
		}
	}
}

// Called to look right
void AGACharacter::LookRight(float Val)
{
	if (Val != 0)
	{
		if (bIsPaused)
		{
			if (PC)
				PC->CursorMoveRight(Val);
		}
		else
		{
			if (bIsUsingController)
			{
				AddControllerYawInput(Val * HorizontalLookRate);
			}
			else
			{
				AddControllerYawInput(Val);
			}

			UpdateSightRotation();
		}
	}
}
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

		if (PC)
		{
			PC->EndPause();
		}
		else if (AC)
		{

		}
	}
	else
	{
		bIsPaused = true;

		if (PC)
		{
			PC->BeginPause();
		}
		else if (AC)
		{

		}
	}
}

void AGACharacter::EndPause()
{
	/*
	bIsPaused = false;

	if (PC)
	{
		PC->EndPause();
	}
	else if (AC) 
	{

	}
	*/
}

void AGACharacter::MenuUp(float Val)
{
	if (Val != 0)
	{
		if (Val > 0)
		{
			if (bIsPaused)
			{
				if (PC)
				{
					PC->RecieveNavRequest(ENavType::Up);
				}
			}
			else
			{
				FTimerDelegate MenuUpDel;
				if (MenuUpTimerHandle.IsValid())
				{
					if (GetWorldTimerManager().IsTimerPaused(MenuUpTimerHandle))
						GetWorldTimerManager().UnPauseTimer(MenuUpTimerHandle);
				}
				else 
				{
					GetWorldTimerManager().SetTimer(MenuUpTimerHandle, this, &AGACharacter::MenuUpDelayed, 0.05f, true);
				}
			}
		}
		else if (Val < 0)
		{
			if (bIsPaused)
			{
				if (PC)
				{
					PC->RecieveNavRequest(ENavType::Down);
				}
			}
			else
			{
				if (MenuDownTimerHandle.IsValid())
				{
					if (GetWorldTimerManager().IsTimerPaused(MenuDownTimerHandle))
						GetWorldTimerManager().UnPauseTimer(MenuDownTimerHandle);
				}
				else
				{
					GetWorldTimerManager().SetTimer(MenuDownTimerHandle, this, &AGACharacter::MenuDownDelayed, 0.05f, true);
				}
			}
		}
	}
	else
	{
		// Value is 0; Meaning we are not touching the input
		// Clearing the timer handle will stop the menu selection
		GetWorldTimerManager().ClearTimer(MenuUpTimerHandle);
		GetWorldTimerManager().ClearTimer(MenuDownTimerHandle);
	}
}

void AGACharacter::MenuRight(float Val)
{
	if (Val != 0)
	{
		FTimerDelegate MenuDel;

		if (Val == 1)
		{
			if (bIsPaused)
			{
				if (PC)
				{
					PC->RecieveNavRequest(ENavType::Right);
				}
			}
			else
			{
				if (MenuRightTimerHandle.IsValid())
				{
					if (GetWorldTimerManager().IsTimerPaused(MenuRightTimerHandle))
						GetWorldTimerManager().UnPauseTimer(MenuRightTimerHandle);
				}
				else
				{
					GetWorldTimerManager().SetTimer(MenuRightTimerHandle, this, &AGACharacter::MenuRightDelayed, 0.1f, true);
				}
			}
		}
		else if (Val == -1)
		{
			if (bIsPaused)
			{
				if (PC)
				{
					PC->RecieveNavRequest(ENavType::Left);
				}
			}
			else
			{
				if (MenuLeftTimerHandle.IsValid())
				{
					if (GetWorldTimerManager().IsTimerPaused(MenuLeftTimerHandle))
						GetWorldTimerManager().PauseTimer(MenuLeftTimerHandle);
				}
				else
				{
					GetWorldTimerManager().SetTimer(MenuLeftTimerHandle, this, &AGACharacter::MenuLeftDelayed, 0.1f, true);
				}
			}
		}
	}
	else
	{
		// Value is 0; Meaning we are not touching the input
		// Clearing the timer handle will stop the menu selection
		GetWorldTimerManager().ClearTimer(MenuRightTimerHandle);
		GetWorldTimerManager().ClearTimer(MenuLeftTimerHandle);
	}
}

void AGACharacter::MenuUpDelayed()
{
	if (bWeaponPriority && HeldItem)
	{
		
	}
	else
	{
		if (SubMenuCount == 0)
			DetermineActiveMenuSelection(0, 1);
		else if (MaxSubMenuSlots.Num() > SubMenuCount)
			DetermineActiveMenuSelection(MaxActiveMenuSlots[SubMenuCount], 1);
	}
}

void AGACharacter::MenuDownDelayed()
{
	if (bWeaponPriority && HeldItem)
	{
		
	}
	else
	{
		if (SubMenuCount == 0)
			DetermineActiveMenuSelection(0, 1);
		else if (MaxSubMenuSlots.Num() > SubMenuCount)
			DetermineActiveMenuSelection(MaxActiveMenuSlots[SubMenuCount], -1);
	}
}

void AGACharacter::MenuRightDelayed()
{
	if (bWeaponPriority && HeldItem)
	{
		
	}
	else
	{
		if (ActiveMenuCount == 0)
			DetermineSubMenuSelection(0, 1);
		else if (MaxSubMenuSlots[ActiveMenuCount])
			DetermineSubMenuSelection(MaxSubMenuSlots[ActiveMenuCount], 1);
	}
}

void AGACharacter::MenuLeftDelayed()
{
	if (bWeaponPriority && HeldItem)
	{
		
	}
	else
	{
		if (ActiveMenuCount == 0)
			DetermineSubMenuSelection(0, -1);
		else if (MaxSubMenuSlots[ActiveMenuCount])
			DetermineSubMenuSelection(MaxSubMenuSlots[ActiveMenuCount], -1);
	}
}
#pragma endregion
#pragma endregion

#pragma region Combat
// */
//////////////////////////////////////////////////////
/////			CHARACTER COMBAT MENU			//////
//////////////////////////////////////////////////////

uint8 AGACharacter::GetActiveMenuCount() const
{
	return ActiveMenuCount;
}

uint8 AGACharacter::GetSubMenuCount() const
{
	return SubMenuCount;
}

void AGACharacter::ResetActiveMenuSelection()
{
	ActiveMenuCount = 1;

	if (PC)
	{
		PC->OnMenuUpdated(FVector2D(ActiveMenuCount, SubMenuCount));
	}
}

void AGACharacter::ResetSubMenuSelection()
{
	SubMenuCount = 1;

	if (PC)
	{
		PC->OnMenuUpdated(FVector2D(ActiveMenuCount, SubMenuCount));
	}
}

int AGACharacter::DetermineActiveMenuSelection(int MaxMenuCount, int Direction)
{
	int Value = -1;

	if (MaxMenuCount == 0)
	{
		return 1;

		if (PC)
		{
			PC->OnMenuUpdated(FVector2D(ActiveMenuCount, SubMenuCount));
		}
	}

	if (Direction > 0)
	{
		ActiveMenuCount += 1;

		if (ActiveMenuCount <= MaxMenuCount)
		{
			Value = ActiveMenuCount;
		}
		else if (ActiveMenuCount > MaxMenuCount)
		{
			Value = 1;
			ActiveMenuCount = 1;
		}
	}
	else if (Direction < 0)
	{
		ActiveMenuCount -= 1;

		if (ActiveMenuCount >= 1)
		{
			Value = ActiveMenuCount;
		}
		else if (ActiveMenuCount < 1)
		{
			Value = MaxMenuCount;
			ActiveMenuCount = MaxMenuCount;
		}
	}

	if (PC)
	{
		PC->OnMenuUpdated(FVector2D(ActiveMenuCount, SubMenuCount));
	}

	// ResetSubMenuSelection();

	return Value;
}

int AGACharacter::DetermineSubMenuSelection(int MaxMenuCount, int Direction)
{
	PreviousActiveMenuIndex = ActiveMenuCount;
	int Value = -1;

	if (MaxMenuCount == 0)
	{
		return 1;

		if (PC)
		{
			PC->OnMenuUpdated(FVector2D(ActiveMenuCount, SubMenuCount));
		}
	}

	if (Direction > 0)
	{
		SubMenuCount += 1;

		if (SubMenuCount <= MaxMenuCount)
		{
			Value = SubMenuCount;
		}
		else if (SubMenuCount > MaxMenuCount)
		{
			Value = 1;
			SubMenuCount = 1;
		}
	}
	else if (Direction < 0)
	{
		SubMenuCount -= 1;

		if (SubMenuCount >= 1)
		{
			Value = SubMenuCount;
		}
		else if (SubMenuCount < 1)
		{
			Value = MaxMenuCount;
			SubMenuCount = MaxMenuCount;
		}
	}

	if (PC)
	{
		PC->OnMenuUpdated(FVector2D(ActiveMenuCount, SubMenuCount));
	}

	ResetActiveMenuSelection();

	return Value;
}

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

void AGACharacter::OnHealthModified(float Health, float MaxHealth)
{
	AIInfo.CurrentAITargetStats.CurrentHealth = Health;
	AIInfo.CurrentAITargetStats.MaxHealth = MaxHealth;

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Health was modified!!");

	if (Health <= 0)
	{
		Death();
	}

	if (PC)
	{
		PC->OnHealthModified(Health, MaxHealth);
	}
}

void AGACharacter::OnStaminaModified(float Stamina, float MaxStamina)
{
	if(PC)
	{
		PC->OnStaminaModified(Stamina, MaxStamina);
	}
}

void AGACharacter::OnManaModified(float Mana, float MaxMana)
{
	if (PC)
	{
		PC->OnManaModified(Mana, MaxMana);
	}
}

// Called when experience has been changed
void AGACharacter::OnExperienceModified(float Experience, float MaxExperience)
{
	if (PC)
	{
		PC->OnExperienceModified(Experience, MaxExperience);
	}
}

// Called when a stat changes
void AGACharacter::OnStatModified(float Charisma, float Constitution, float Dexterity, float Intelligence, float Strength, float Wisdom)
{
	if (PC)
	{
		PC->OnStatModified(Charisma, Constitution, Dexterity, Intelligence, Strength, Wisdom);
	}
}

// Called when current ammo count changes
void AGACharacter::OnAmmoModified(float Ammo, float MaxAmmo)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Purple, "Ammo Modified from character");
	if (PC)
	{
		PC->OnCurrentWeaponAmmoModified(Ammo, MaxAmmo);
	}
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
void AGACharacter::Death()
{
	UE_LOG(LogTemp, Warning, TEXT("Character has died..."));

	if (PC)
	{
		DisableInput(PC);
		// PC->DisableInput(PC);
	}
	else if (AC)
	{
		AC->GetBrainComponent()->StopLogic("Dead");
	}

	OnDeath();
}
#pragma endregion

#pragma region Interaction
// */
//////////////////////////////////////////////////////
/////			CHARACTER INTERACTION			//////
//////////////////////////////////////////////////////

void AGACharacter::NotifyCanPickup_Implementation(AActor* Item, bool CanPickup)
{
	CurrentInteractItem = Item;

	if (PC)
	{
		PC->NotifyPickup(IPickupInterface::Execute_GetPickupName(Item), CanPickup);
	}
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
	if (!DialogueParticipantInfo->Dialogue)
	{
		return;
	}

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
			OtherPC->StartDialogue(DialogueParticipantInfo->Dialogue, this);
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
#pragma endregion

#pragma region ItemHandling
// */
//////////////////////////////////////////////////////
/////			CHARACTER INVENTORY				//////
//////////////////////////////////////////////////////

AActor* AGACharacter::GetHeldItem()
{
	return HeldItem;
}

AActor* AGACharacter::GetStowedItem()
{
	return StowedItem;
}

void AGACharacter::PickupItem(AGAActor* Item)
{
	if (Item)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString("We want to pick " + Item->GetName() + " up!"));
		}

		if (!HeldItem)
		{
			EquipItem(Item);
		}
		else if (!StowedItem)
		{
			StowItem(Item);
		}
		else if (Inventory->AddItem(Item))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString("We have space in our invetory!"));
			AGAWeapon* Weapon = Cast<AGAWeapon>(Item);
			if (Weapon && PC)
			{
				FGAItemInfo Info = FGAItemInfo();
				Weapon->GetItemInfo(Info);
				PC->OnPickupItem(Info); 
				Weapon->SetActorHiddenInGame(true);
				Weapon->SetActorEnableCollision(false);
			}
		}
	}
}

void AGACharacter::EquipItem(AGAActor* Item)
{
	if (Item)
	{
		HeldItem = Item;
		HeldItem->SetOwner(this);

		if (PC)
		{
			PC->SetItemPriority(bWeaponPriority);
			AGAWeapon* Weapon = Cast<AGAWeapon>(Item);
			if (Weapon)
			{
				FGAItemInfo Info = FGAItemInfo();
				Weapon->GetItemInfo(Info);
				HeldWeaponType = Weapon->WeaponType;
				PC->OnEquipItem(Item, Info);
			}
		}
		else if (AC)
		{

		}

		FAttachmentTransformRules ItemAttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
		HeldItem->AttachToComponent(this->GetMesh(), ItemAttachmentRules, this->GetMesh()->DoesSocketExist(DominantHand) ? DominantHand : FName());
		HeldItem->SetActorHiddenInGame(false);
		HeldItem->SetActorEnableCollision(false);
		HeldItem->IntializeAbilitySystem();

		// Item->OnEquipped();
	}
}

void AGACharacter::StowItem(AGAActor* Item)
{
	if (Item)
	{
		StowedItem = Item;
		StowedItem->SetOwner(this);
		FAttachmentTransformRules ItemAttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
		StowedItem->AttachToComponent(this->GetMesh(), ItemAttachmentRules, this->GetMesh()->DoesSocketExist(FName("spine_03")) ? FName("spine_03") : FName());
		StowedItem->SetActorHiddenInGame(true);
		StowedItem->SetActorEnableCollision(false);

		if (PC)
		{
			PC->SetItemPriority(bWeaponPriority);
			AGAWeapon* Weapon = Cast<AGAWeapon>(Item);
			if (Weapon)
			{
				FGAItemInfo Info = FGAItemInfo();
				Weapon->GetItemInfo(Info);
				PC->OnStowedItem(Info);
			}
		}
		else if (AC)
		{

		}

		// Item->OnStowed();
	}
}

void AGACharacter::SwitchEquippedItems()
{
	AGAActor* TempItem = StowedItem;
	StowItem(HeldItem);
	EquipItem(TempItem);
}

void AGACharacter::EquipItemFromInventory(uint8 Slot)
{
	AGAActor* TempItem = Inventory->GetItem(Slot);
	Inventory->RemoveItem(Slot);
	AddEquippedItemToInventory();
	EquipItem(TempItem);
}

void AGACharacter::StowItemFromInventory(uint8 Slot)
{
	if (!StowedItem)
	{
		StowItem(Inventory->GetItem(Slot));
	}
	else
	{
		AddStowedItemToInventory();
		StowItem(Inventory->GetItem(Slot));
	}
}

void AGACharacter::AddStowedItemToInventory()
{
	StowedItem->SetActorHiddenInGame(true);
	Inventory->AddItem(StowedItem);

	if (PC)
	{
		AGAWeapon* Weapon = Cast<AGAWeapon>(StowedItem);
		if (Weapon)
		{
			FGAItemInfo Info = FGAItemInfo();
			Weapon->GetItemInfo(Info);
			PC->OnPickupItem(Info);
		}
	}

	StowedItem = nullptr;
}

void AGACharacter::AddEquippedItemToInventory()
{
	HeldItem->SetActorHiddenInGame(true);
	Inventory->AddItem(HeldItem);

	if (PC)
	{
		AGAWeapon* Weapon = Cast<AGAWeapon>(HeldItem);
		if (Weapon)
		{
			FGAItemInfo Info = FGAItemInfo();
			Weapon->GetItemInfo(Info);
			PC->OnPickupItem(Info);
		}
	}

	HeldItem = nullptr;
}

void AGACharacter::MoveItemWithinInventory(uint8 From, uint8 To)
{
	Inventory->MoveItem(From, To);
}

void AGACharacter::DropEquippedItem()
{
	if (HeldItem)
	{
		HeldItem->SetOwner(nullptr);
		HeldItem->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepRelative, true));
		HeldItem->SetActorEnableCollision(true);

		HeldItem = nullptr;
	}
}

void AGACharacter::DropItemFromInventory(uint8 Slot)
{
	AGAActor* Item = Inventory->GetItem(Slot);

	float RandomX = FMath::RandRange(-100, 100);
	float RandomY = FMath::RandRange(-100, 100);
	FVector2D NewLocation = FVector2D(RandomX, RandomY).GetSafeNormal() * ItemDropRadius;

	FVector ItemLocation = GetActorLocation() + FVector(NewLocation, 0);

	Item->SetActorHiddenInGame(false);
	Item->SetActorEnableCollision(true);
	Item->SetActorLocation(ItemLocation);

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
	if (PC) {
		PC->OnAddItemToRepository(Item);
	}
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

// Start Slide
void AGACharacter::BeginSlide()
{
	bIsSliding = true;
	Crouch();
	PlayAnimMontage(SlideMontage);
}

// Stop slide
void AGACharacter::EndSlide()
{
	bIsSliding = false;
	UnCrouch();
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

		Sight->SetWorldRotation(FRotator(DesiredRotation.Pitch, DesiredRotation.Yaw, DesiredRotation.Roll));
	}
	else if (AC)
	{

	}
}

// Called on Attribute decrease
void AGACharacter::OnDamaged(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue)
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Damage was taken!!");

	SpawnNumbers(DeltaAmount);
	SpawnValueBar(AttributeType, NewValue, AIInfo.CurrentAITargetStats.MaxHealth);

	if (PC)
	{
		PC->OnDamaged(SourceActor, AttributeType, DeltaAmount, NewValue);
	}
	else if (AC)
	{
		if (SourceActor == this)
		{
			return;
		}

		AGACharacter* DamageCharacter = Cast<AGACharacter>(SourceActor);

		if (DamageCharacter)
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
	UGameplayStatics::ApplyDamage(this, DeltaAmount, SourceActor->GetInstigatorController(), SourceActor, UDamageType::StaticClass());
}

// Called on Attribute increase
void AGACharacter::OnHealed(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue)
{
	if (PC)
	{
		PC->OnHealed(SourceActor, AttributeType, DeltaAmount, NewValue);
	}
	else if (AC)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Health was given!!");

		AC->OnHealed(SourceActor, AttributeType, DeltaAmount, NewValue);
	}
}

void AGACharacter::OnPawnSeen_Implementation(APawn* SeenPawn)
{
	if (!SeenPawn)
	{
		return;
	}

	AGACharacter* SeenCharacter = Cast<AGACharacter>(SeenPawn);

	if (SeenCharacter)
	{
		// FGameplayAttribute Perception;

		// if (AttributeCheck(Perception, 1))
		// {

		// }

		if (PC)
		{

		}

		if (AC)
		{
			bool bIsNewCharacter = true;

			for (FAICharacterInfo VisibleCharacter : AwareOfCharacters)
			{
				if (VisibleCharacter.Target == SeenCharacter)
				{
					bIsNewCharacter = false;
					break;
				}
			}

			if (bIsNewCharacter)
			{
				FAICharacterInfo SeenInfo(SeenCharacter, SeenCharacter->GetActorLocation(), 0);

				FAICharacterAwareness& AIState = SeenInfo.CurrentAIAwarenessTowardTarget;

				AIState.SetCurrentSuspicion(1);

				GetWorldTimerManager().SetTimer(SeenInfo.AwarenessTimerHandle, ReactionTime, true);
				AwareOfCharacters.Add(SeenInfo);
			}

			AC->OnPawnSeen(SeenPawn);
		}
	}
}

void AGACharacter::OnNoiseHeard_Implementation(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (!NoiseInstigator)
	{
		return;
	}

	if (PC)
	{

	}

	if (AC)
	{
		AGACharacter* NoiseCharacter = Cast<AGACharacter>(NoiseInstigator);

		if (NoiseCharacter)
		{
			bool bIsNewInstigator = true;

			for (FAICharacterInfo AwareOfCharacter : AwareOfCharacters)
			{
				if (AwareOfCharacter.Target != nullptr)
				{
					if (AwareOfCharacter.Target == NoiseCharacter)
					{
						FAICharacterAwareness& AIState = AwareOfCharacter.CurrentAIAwarenessTowardTarget;

						AIState.SetCurrentSuspicion(AIState.GetCurrentSuspicion() + Volume);

						bIsNewInstigator = false;
						break;
					}
				}
			}

			if (bIsNewInstigator)
			{
				AwareOfCharacters.Add(FAICharacterInfo(NoiseCharacter, Location, Volume));
			}
		}

		AC->OnNoiseHeard(NoiseInstigator, Location, Volume);
	}
}

void AGACharacter::CheckVisibleCharacters()
{
	for (FAICharacterInfo VisibleCharacter : AwareOfCharacters)
	{
		FVector Start = Sight->GetComponentLocation();
		FVector End = VisibleCharacter.Target->Sight->GetComponentLocation();
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
				FVector Vector1 = Sight->GetComponentRotation().Vector().GetSafeNormal();
				FVector Vector2 = (End - Start).GetSafeNormal();
				float Angle = 
					(FVector::CrossProduct(Vector1, Vector2).Z > 0 ? 1 : -1)
					* (FMath::Acos( FVector::DotProduct(Vector1, Vector2) ));

				if (abs(Angle) < Sight->GetFoV())
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
				FVector Vector1 = Sight->GetComponentRotation().Vector().GetSafeNormal();
				FVector Vector2 = (End - Start).GetSafeNormal();
				float Angle =
					(FVector::CrossProduct(Vector1, Vector2).Z > 0 ? 1 : -1)
					* (FMath::Acos(FVector::DotProduct(Vector1, Vector2)));

				if (abs(Angle) < Sight->GetFoV())
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

				if (Shadowed == 2)
				{
					AIState.SetCurrentSuspicion(1 + AIState.GetCurrentSuspicion());
				}
				else if (Shadowed == 1)
				{
					AIState.SetCurrentSuspicion(4 + AIState.GetCurrentSuspicion());
				}
				else
				{
					AIState.SetCurrentSuspicion(6 + AIState.GetCurrentSuspicion());
				}
			}
		}

		DialogueParticipantInfo->Relationships.Contains(VisibleCharacter.Target);
	}
}

void AGACharacter::CalculateShadowed()
{
	if (!Sun)
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
	}
}

void AGACharacter::UpdateCurrentBuilding(AGameplayBuilding* CurrentBuilding)
{
	if (PC)
	{
		PC->OnUpdateCurrentBuilding(CurrentBuilding->BuildingName);
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
		PC->OnDesiredLocationSet(DesiredLocation);
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
		PC->OnUpdateTargetBuilding(TargetBuilding->BuildingName);
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
