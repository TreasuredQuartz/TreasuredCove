// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"
#include "InteractionInterface.h"
#include "GAVehicle.generated.h"

class UAttributeSet;
class UBoxComponent;
class UMeshComponent;
class UVehicleMovementComponent;
class AGACharacter;
class UGASystemComponent;

UCLASS()
class TREASUREDCOVE_API AGAVehicle :
	public APawn,
	public IAbilitySystemInterface,
	public IInteractionInterface
{
	GENERATED_BODY()

	// Override function from ability system interface
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// Override Getter from base Pawn class
	virtual UPawnMovementComponent* GetMovementComponent() const override;
public:
	// Sets default values for this pawn's properties
	AGAVehicle();

	float ThrustSpeed;
	float CurrentSpeed;

	TArray<AGACharacter*> Passengers;
	AGACharacter* Driver;

	int32 MaxPassengers;

	// Abilities Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	UGASystemComponent* AbilitySystem;
	// Attribute Set Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UAttributeSet>> AttributeSet;
	// Box Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Collision;
	// Movement Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UVehicleMovementComponent* Movement;

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
	//
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
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


};
