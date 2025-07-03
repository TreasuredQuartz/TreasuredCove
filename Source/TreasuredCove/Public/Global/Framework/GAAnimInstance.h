// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "Global/Enumerations/WeaponType.h"

#include "GAAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UGAAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
private:
	UFUNCTION()
	void OnMovementModeChanged(class ACharacter* InCharacter, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);
	
	bool FloorLineTrace(FHitResult& Hit, const FVector& SocketLocation, float Distance) const;
	bool WallLineTrace(FHitResult& Hit, const FVector& SocketLocation, float Distance) const;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class AGACharacter* Character;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class AGAActor* CharacterItem;
	UPROPERTY(EditAnywhere, Category = "Character Movement: MovementMode", BlueprintReadOnly)
	TEnumAsByte<enum EMovementMode> CharacterMovementMode;
	UPROPERTY(EditAnywhere, Category = "Character Movement: MovementMode", BlueprintReadOnly)
	uint8 CharacterCustomMovementMode;

	TEnumAsByte<enum EMovementMode> MovementGait;		// Walking, Jogging, Running, Sprinting
	TEnumAsByte<enum EMovementMode> MovementStance;		// Standing, Crouching, Prone, Supine

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWeaponType HeldWeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform RightHandEffectorTransform;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform LeftHandEffectorTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator ControlDirection;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator HandRotation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator BodyDirection;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator HeadRotation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator DominantHandTwist;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector MeshSpaceVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector RightFootEffectorLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector LeftFootEffectorLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector PelvisOffsetLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector AimOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimYaw;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimPitch;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimOffsetAlpha;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimOffsetAlphaLerp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsPlayerControlled;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bFirstPerson;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCrouching;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAbilityShouldUseFullBody;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bPerformIKTraces = true;

	// Call this in the AnimBP's Event graph
	UFUNCTION(BlueprintCallable, Category = "Animation|Update")
	void UpdateAnimationProperties(float DeltaTime);

	/* Procedural Animation */
	void UpdateIKPlacements(float DeltaTime);

	bool ShouldUpdateIKPlacements();

	// Specific IK functionality related to MovementMode
	void IKPlacements_Walking(float DeltaTime);
	void IKPlacements_WallRunning(float DeltaTime);
	void IKPlacements_Climbing(float DeltaTime);
	void IKPlacements_Hanging(float DeltaTime);
	void IKPlacements_AimItem(float DeltaTime);

	// Reset IK Variables
	void ResetIK();
	void ResetBodyIK();
	void ResetArmIK();
	void ResetLegIK();

	bool CanJump(bool bShouldJump);

	/** Anim Notify events */
	UFUNCTION(BlueprintCallable, Category = "Animation|Notify")
	void AnimNotify_Jump();
	// UFUNCTION(BlueprintCallable, Category = "Animation|Notify")
	// void AnimNotify_IdleStart();
	UFUNCTION(BlueprintCallable, Category = "Animation|Notify")
	void AnimNotify_JogStart();
public:
	virtual void NativeBeginPlay() override;
};
