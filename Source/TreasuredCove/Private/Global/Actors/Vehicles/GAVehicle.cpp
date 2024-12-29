// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Actors/Vehicles/GAVehicle.h"
#include "Global/AbilitySystem/GASystemComponent.h"
#include "Global/Actors/GACharacter.h"
#include "ChaosVehicleMovementComponent.h"
#include "VehicleMovementComponent.h"

#include "GameFramework/Controller.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

#include "Global/AbilitySystem/AbilitySet.h"
#include "Global/AbilitySystem/Abilities/GameplayAbilityBase.h"

// Sets default values
AGAVehicle::AGAVehicle()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//
	/*RootComponent =
		CreateDefaultSubobject<USceneComponent>(TEXT("Root"));*/

	// 
	Collision =
		CreateDefaultSubobject<UBoxComponent>("Collision");
	Collision->SetupAttachment(RootComponent);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AGAVehicle::OnOverlapBegin);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AGAVehicle::OnOverlapEnd);

	//
	AbilitySystem =
		CreateDefaultSubobject<UGASystemComponent>(TEXT("Ability System"));

	//
	//Movement =
	//	CreateDefaultSubobject<UVehicleMovementComponent>("Movement");
	//Movement->UpdatedComponent = Mesh;

	Name = FName();
	bAutoPilot = false;
	MaxPassengers = 0;
	ThrustSpeed = 1024.f;

	CurrentSpeed = 0.f;
	Driver = nullptr;
	Passengers = TArray<AGACharacter*>();
}

UAbilitySystemComponent* AGAVehicle::GetAbilitySystemComponent() const
{
	return Cast<UAbilitySystemComponent>(AbilitySystem);
}

void AGAVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Primary", IE_Pressed, this, &AGAVehicle::PrimaryPressed);
	PlayerInputComponent->BindAction("Primary", IE_Released, this, &AGAVehicle::PrimaryReleased);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AGAVehicle::Throttle);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AGAVehicle::Exit);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGAVehicle::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGAVehicle::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AGAVehicle::LookUp);
	PlayerInputComponent->BindAxis("LookRight", this, &AGAVehicle::LookRight);
}

// Called when possessed by a new controller
void AGAVehicle::PossessedBy(AController* NewController)
{
	UE_LOG(LogTemp, Warning, TEXT("Possessed..."));

	if (AbilitySystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability System initializing..."));
		InitializeAbilitySystem();
	}

	Super::PossessedBy(NewController);
}

// Called when control is taken away from this pawn
void AGAVehicle::UnPossessed()
{
	Super::UnPossessed();
}

// Called when server replicates controller to clients
void AGAVehicle::OnRep_Controller()
{
	if (AbilitySystem)
	{
		AbilitySystem->RefreshAbilityActorInfo();
	}

	Super::OnRep_Controller();
}

// */
//////////////////////////////////////////////////////
/////			Ability Gameplay Tags			//////
//////////////////////////////////////////////////////

// Called to give a gameplay tag to self
void AGAVehicle::AddGameplayTag(const FGameplayTag& NewTag)
{
	GetAbilitySystemComponent()->AddLooseGameplayTag(NewTag);
	GetAbilitySystemComponent()->SetTagMapCount(NewTag, 1);
}

// Called to remove a gameplay tag from self
void AGAVehicle::RemoveGameplayTag(const FGameplayTag& OldTag)
{
	GetAbilitySystemComponent()->RemoveLooseGameplayTag(OldTag);
}

// */
//////////////////////////////////////////////////////
/////			VEHICLE ABILITIES				//////
//////////////////////////////////////////////////////

// Called during begin play or possession
void AGAVehicle::InitializeAbilitySystem()
{
	AbilitySystem->InitAbilityActorInfo(this, this);

	if (!AttributeSetClasses.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Attribute Sets initializing..."));

		for (TSubclassOf<UAttributeSet>& SetClass : AttributeSetClasses)
		{
			// const_cast to access non-const properties within UAttributeSet such as:
			// - Dynamic Delegates
			UAttributeSet* Set = const_cast<UAttributeSet*>(AbilitySystem->InitStats(SetClass, InitialStatsTable));

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

	// This should be a UDataAsset Instance. Meaning there is no need to spawn anything
	// And data should be readily available for access.
	if (InitialActiveAbilities) AquireAbilities(InitialActiveAbilities->GetAbilities(), ActiveAbilityHandles);
	// if (InitialPassiveAbilities) AquirePassives(InitialPassiveAbilities->GetAbilities());
}

// Called to give an ability to a player. This should be called by a skill tree asset or at begin play in a multiplayer match.
void AGAVehicle::AquireAbility(TSubclassOf<UGameplayAbility> InAbility, FGameplayAbilitySpecHandle& OutHandle)
{
	if (AbilitySystem)
	{
		if (HasAuthority() && InAbility)
		{
			FGameplayAbilitySpecDef SpecDef = FGameplayAbilitySpecDef();
			SpecDef.Ability = InAbility;
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(SpecDef, 1);
			OutHandle = AbilitySystem->GiveAbility(AbilitySpec);
			// AbilitySystem->OnAbilityEnded.AddUObject(this, &AGAVehicle::AbilityEnded);

			if (InAbility->IsChildOf(UGameplayAbilityBase::StaticClass()))
			{
				TSubclassOf<UGameplayAbilityBase> AbilityBaseClass = *InAbility;
				if (AbilityBaseClass != nullptr)
				{
					// AddAbilityToUI(AbilityBaseClass, EAbilityType::Primary, OutHandle, false);
				}
			}
		}
	}
}

// Called to give multiple abilities to a player at once.
void AGAVehicle::AquireAbilities(TArray<TSubclassOf<UGameplayAbility>> InAbilities, TArray<FGameplayAbilitySpecHandle>& OutHandles)
{
	int i = -1;
	for (const TSubclassOf<UGameplayAbility>& InAbility : InAbilities)
	{
		AquireAbility(InAbility, OutHandles[++i]);
	}
}

// Called during ability system initialization
void AGAVehicle::InitializeAttributeSet(UAttributeSet* Set)
{
	/*
	UASHealth* HealthSet = Cast<UASHealth>(Set);
	if (HealthSet)
	{
		/*HealthSet->OnDamaged.AddDynamic(this, &AGACharacter::OnDamaged);
		HealthSet->OnHealed.AddDynamic(this, &AGACharacter::OnHealed);

		HealthSet->OnHealthModified.AddDynamic(this, &AGACharacter::OnHealthModified);
		HealthSet->OnStaminaModified.AddDynamic(this, &AGACharacter::OnStaminaModified);
		HealthSet->OnManaModified.AddDynamic(this, &AGACharacter::OnManaModified);
		return;
	}

	UASStats* StatSet = Cast<UASStats>(Set);
	if (StatSet)
	{
		/*StatSet->OnExperienceModified.AddDynamic(this, &AGACharacter::OnExperienceModified);
		StatSet->OnStatModified.AddDynamic(this, &AGACharacter::OnStatModified);
		return;
	}

	UASAmmo* AmmoSet = Cast<UASAmmo>(Set);
	if (AmmoSet)
	{
		/*AmmoSet->OnAmmoModified.AddDynamic(this, &AGACharacter::OnAmmoModified);
		return;
	}

	UASWeaponStats* WeaponSet = Cast<UASWeaponStats>(Set);
	if (WeaponSet)
	{
		/*WeaponSet->OnSpreadAngleModified.AddDynamic(this, &AGACharacter::OnSpreadAngleModified);
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
		/*UltimateSet->OnUltimateChargeModified.AddDynamic(this, &AGACharacter::OnUltimateModified);
		return;
	} // */
}

// Called when loading
void AGAVehicle::PopulateSavedAttributes(const TArray<FSavedAttribute>& Attributes)
{
	if (AbilitySystem == nullptr)
	{
		return;
	}

	for (const FSavedAttribute& Attr : Attributes)
	{
		AbilitySystem->SetNumericAttributeBase(Attr.Attribute, Attr.SavedValue);
	}
}

// */
//////////////////////////////////////////////////////
/////			VEHICLE OVERLAPS				//////
//////////////////////////////////////////////////////

void AGAVehicle::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		IInteractionInterface::Execute_NotifyCanInteract(OtherActor, Name, true);
	}
}

void AGAVehicle::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		IInteractionInterface::Execute_NotifyCanInteract(OtherActor, Name, false);
	}
}

void AGAVehicle::LaunchVehicle(const FVector& LaunchVelocity)
{
	// GetVehicleMovementComponent()->Set;
}

void AGAVehicle::AddForce(const FVector& Force)
{
}

// */
//////////////////////////////////////////////////////
/////			  INPUT HANDLING				//////
//////////////////////////////////////////////////////

void AGAVehicle::InteractedWith_Implementation(AActor* OtherActor)
{
	AGACharacter* Character = Cast<AGACharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	int32 Index = 0;
	bool bIsPassenger = false;

	for (AGACharacter* Passenger : Passengers)
	{
		if (Passenger == Character)
		{
			bIsPassenger = true;
			Index = Passengers.Find(Character);
		}
	}

	if (!bIsPassenger)
	{
		// Some Pawn wants in
		if (Driver)
		{
			// We already have a driver
			Ride(Character);
		}
		else
		{
			// We do not have a driver
			Drive(Character);
		}
	}
	else
	{
		ExitPassenger(Index);
	}
}

void AGAVehicle::Drive(AGACharacter* Character)
{
	AController* C = Character->GetController();
	C->UnPossess();
	C->Possess(this);

	Driver = Character;

	if ((VehicleEnterMontages.IsValidIndex(0) && VehicleEnterMontages[0]) && 
		(CharacterEnterMontages.IsValidIndex(0) && CharacterEnterMontages[0]))
	{
		Character->PlayAnimMontage(CharacterEnterMontages[0]);
		// Mesh->GetAnimInstance()->Montage_Play(VehicleEnterMontages[0]);
	}
}

void AGAVehicle::Ride(AGACharacter* Character)
{
	if (Passengers.Num() < MaxPassengers)
	{

		if (VehicleEnterMontages[Passengers.Num() + 1] && CharacterEnterMontages[Passengers.Num() + 1])
		{
			Character->PlayAnimMontage(CharacterEnterMontages[Passengers.Num() + 1]);
			// Mesh->GetAnimInstance()->Montage_Play(VehicleEnterMontages[Passengers.Num() + 1]);

			Passengers.Add(Character);
		}
	}
}

void AGAVehicle::ExitPassenger(int32 Index)
{
	if (CharacterExitMontages.IsValidIndex(Index) && CharacterExitMontages[Index])
	{
		Passengers[Index]->PlayAnimMontage(CharacterExitMontages[Index]);
		// Mesh->GetAnimInstance()->Montage_Play(VehicleExitMontages[Index]);

		Passengers.RemoveAt(Index);
	}
}

void AGAVehicle::Exit()
{
	AController* C = GetController();
	C->UnPossess();
	C->Possess(Driver);
	Driver = nullptr;

	// 
	if (VehicleExitMontages.IsValidIndex(0) && VehicleExitMontages[0])
	{
		Driver->PlayAnimMontage(CharacterExitMontages[0]);
		// Mesh->GetAnimInstance()->Montage_Play(VehicleExitMontages[0]);
	}
}

void AGAVehicle::PrimaryPressed_Implementation()
{
}

void AGAVehicle::PrimaryReleased_Implementation()
{
}

void AGAVehicle::SecondaryPressed_Implementation()
{
}

void AGAVehicle::SecondaryReleased_Implementation()
{
}

void AGAVehicle::TertiaryPressed_Implementation()
{
}

void AGAVehicle::TertiaryReleased_Implementation()
{
}

void AGAVehicle::MoveForward_Implementation(float AxisValue)
{
	if (AxisValue != 0)
	{
		// Our control rotation
		FRotator Rotation = GetActorRotation();

		// This essentially gets from our control rotation a Up Vector
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Z);

		// We want to move in this direction
		AddMovementInput(Direction, AxisValue);
	}
}

void AGAVehicle::MoveRight_Implementation(float AxisValue)
{
	if (AxisValue != 0)
	{
		// Our rotation
		FRotator Rotation = GetActorRotation();

		// This essentially gets from our control rotation a Right Vector
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);

		// We want to move in this direction
		AddMovementInput(Direction, AxisValue);
	}
}

void AGAVehicle::LookUp_Implementation(float AxisValue)
{
	if (AxisValue != 0)
	{
		AddControllerPitchInput(AxisValue);
	}
}

void AGAVehicle::LookRight_Implementation(float AxisValue)
{
	if (AxisValue != 0)
	{
		AddControllerYawInput(AxisValue);
	}
}

void AGAVehicle::Throttle_Implementation()
{
	
}

void AGAVehicle::Brake_Implementation()
{
	
}

// */
//////////////////////////////////////////////////////
/////			 SERVER REPLICATION				//////
//////////////////////////////////////////////////////


// ServerMovePacked
void AGAVehicle::ServerMovePacked_Implementation(const FVehicleServerMovePackedBits& PackedBits)
{
	// GetVehicleMovement()->ServerMovePacked_ServerReceive(PackedBits);
}

bool AGAVehicle::ServerMovePacked_Validate(const FVehicleServerMovePackedBits& PackedBits)
{
	// Can't really validate the bit stream without unpacking, and that is done in ServerMovePacked_ServerReceive() and can be rejected after unpacking.
	return true;
}



