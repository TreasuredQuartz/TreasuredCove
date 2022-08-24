// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "WeaponType.h"

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

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class AGACharacter* Character;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWeaponType HeldWeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator ControlDirection;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator HandRotation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator BodyDirection;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator HeadRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector MeshSpaceVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector RightHandEffectorLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector LeftHandEffectorLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector RightFootEffectorLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector LeftFootEffectorLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector PelvisOffsetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimYaw;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimPitch;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Direction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DominantHandTwist;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bFirstPerson;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bJumping;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bFalling;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCrouching;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAbilityShouldUseFullBody;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bPerformLegIKTraces;

	// Call this in the AnimBP's Event graph
	UFUNCTION(BlueprintCallable, Category = "Animation|Update")
	void UpdateAnimationProperties(float DeltaTime);
	bool CanJump(bool bShouldJump);
	UFUNCTION(BlueprintCallable, Category = "Animation|Notify")
	void AnimNotify_Jump();
	// UFUNCTION(BlueprintCallable, Category = "Animation|Notify")
	// void AnimNotify_IdleStart();
	UFUNCTION(BlueprintCallable, Category = "Animation|Notify")
	void AnimNotify_JogStart();
public:
	virtual void NativeBeginPlay() override;
};
