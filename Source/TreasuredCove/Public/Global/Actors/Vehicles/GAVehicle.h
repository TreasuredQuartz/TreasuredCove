// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
//#include "GameFramework/Pawn.h"
#include "Global/Components/Vehicles/VehicleMovementReplication.h"
#include "AbilitySystemInterface.h"
#include "Global/Interfaces/InteractionInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include "Global/Structs/SavedAttribute.h"
#include "GAVehicle.generated.h"

class UGameplayAbility;
class UAbilitySet;
class UAttributeSet;
class UBoxComponent;
class UMeshComponent;
class UVehicleMovementComponent;
class AGACharacter;
class UGASystemComponent;

UCLASS()
class TREASUREDCOVE_API AGAVehicle :
	/*public APawn,*/
	public AWheeledVehiclePawn,
	public IAbilitySystemInterface,
	public IInteractionInterface
{
	GENERATED_BODY()

public:
	// Override function from ability system interface
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

public:
	// Sets default values for this pawn's properties
	AGAVehicle();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vehicle", meta = (AllowPrivateAccess = "true"))
	FName Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vehicle", meta = (AllowPrivateAccess = "true"))
	uint8 bAutoPilot:1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vehicle", meta = (AllowPrivateAccess = "true"))
	int32 MaxPassengers;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vehicle", meta = (AllowPrivateAccess = "true"))
	float ThrustSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vehicle|Abilities", meta = (AllowPrivateAccess = "true"))
	UAbilitySet* InitialActiveAbilities;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vehicle|Abilities", meta = (AllowPrivateAccess = "true"))
	UDataTable* InitialStatsTable;
	// Attributes to save
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vehicle|Abilities", meta = (AllowPrivateAccess = "true"))
	TArray<FSavedAttribute> AttributesToSave;

	TArray<FGameplayAbilitySpecHandle> ActiveAbilityHandles;

#pragma region GameplayTagMembers
	/** Gameplay Tags */
private:
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Abilities|Tags", meta = (AllowPrivateAccess = "true"))
	FGameplayTag FullHealthTag;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Abilities|Tags", meta = (AllowPrivateAccess = "true"))
	FGameplayTag FullAmmoTag;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Abilities|Tags", meta = (AllowPrivateAccess = "true"))
	FGameplayTag FullManaTag;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Abilities|Tags", meta = (AllowPrivateAccess = "true"))
	FGameplayTag FullStaminaTag;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Abilities|Tags", meta = (AllowPrivateAccess = "true"))
	FGameplayTag FullExperienceTag;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Abilities|Tags", meta = (AllowPrivateAccess = "true"))
	FGameplayTag FullUltimateTag;
#pragma endregion

	float CurrentSpeed;
	AGACharacter* Driver;
	TArray<AGACharacter*> Passengers;

	//~ Components ~//
public:
	// Abilities Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UGASystemComponent* AbilitySystem;

	// Attribute Set Class Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UAttributeSet>> AttributeSetClasses;

	// Box Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Collision;

	// Movement Component
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UVehicleMovementComponent* Movement;*/

	// Collision interaction events
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	void LaunchVehicle(const FVector& LaunchVelocity);
	void AddForce(const FVector& Force);

	//~ InputHandling ~//
public:
	// Interaction
	virtual void InteractedWith_Implementation(AActor* OtherActor) override;
	void Drive(AGACharacter* Character);
	void Ride(AGACharacter* Character);
	void ExitPassenger(int32 Passenger);
	void Exit();

	// Abilities
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PrimaryPressed();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PrimaryReleased();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SecondaryPressed();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SecondaryReleased();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TertiaryPressed();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TertiaryReleased();

	// Movement
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MoveForward(float AxisValue);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MoveRight(float AxisValue);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LookUp(float AxisValue);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LookRight(float AxisValue);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Throttle();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Brake();

public:
	//Begin APawn Interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* Controller) override;
	virtual void UnPossessed() override;
	virtual void OnRep_Controller() override;
	//End APawn Interface

	//Begin AGAVehicle Interface
	float GetThrustSpeed() const { return ThrustSpeed; };
	void SetThrustSpeed(float InThrustSpeed) { ThrustSpeed = InThrustSpeed; }
	void AddGameplayTag(const FGameplayTag& NewTag);
	void RemoveGameplayTag(const FGameplayTag& OldTag);
	void InitializeAbilitySystem();
	void AquireAbility(TSubclassOf<UGameplayAbility> InAbility, FGameplayAbilitySpecHandle& OutHandle);
	void AquireAbilities(TArray<TSubclassOf<UGameplayAbility>> InAbilities, TArray<FGameplayAbilitySpecHandle>& OutHandles);
	void InitializeAttributeSet(UAttributeSet* Set);
	void PopulateSavedAttributes(const TArray<FSavedAttribute>& Attributes);
	//End AGAVehicle Interface

	//~ Animation Montages ~//
public:
	//
	UPROPERTY()
	TArray<UAnimMontage*> VehicleEnterMontages;
	UPROPERTY()
	TArray<UAnimMontage*> VehicleExitMontages;
	UPROPERTY()
	TArray<UAnimMontage*> CharacterEnterMontages;
	UPROPERTY()
	TArray<UAnimMontage*> CharacterExitMontages;

public:

	//////////////////////////////////////////////////////////////////////////
	// Server RPC that passes through to CharacterMovement (avoids RPC overhead for components).
	// The base RPC function (eg 'ServerMove') is auto-generated for clients to trigger the call to the server function,
	// eventually going to the _Implementation function (which we just pass to the CharacterMovementComponent).
	//////////////////////////////////////////////////////////////////////////

	UFUNCTION(unreliable, server, WithValidation)
	void ServerMovePacked(const FVehicleServerMovePackedBits& PackedBits);
	void ServerMovePacked_Implementation(const FVehicleServerMovePackedBits& PackedBits);
	bool ServerMovePacked_Validate(const FVehicleServerMovePackedBits& PackedBits);
};
