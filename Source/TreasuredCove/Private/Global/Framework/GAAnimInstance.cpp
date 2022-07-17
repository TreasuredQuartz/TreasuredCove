// Fill out your copyright notice in the Description page of Project Settings.


#include "GAAnimInstance.h"
#include "GACharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"
#include "Engine/Engine.h"

void UGAAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	Character = Cast<AGACharacter>(TryGetPawnOwner());

	if (!Character) return;

	// Character->MovementModeChangedDelegate.AddDynamic(this, &UGAAnimInstance::OnMovementModeChanged);
}

void UGAAnimInstance::OnMovementModeChanged(class ACharacter* inCharacter, EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	//if (PreviousMovementMode != MOVE_Falling)
	//{
	//	switch (PreviousCustomMode)
	//	{
	//		// If we just finished wall running...
	//	case ECustomMovementMode::MOVE_WallRunning:
	//		bConstrainToPlane = false;
	//		break;
	//	}
	//}

	//Character->GetMovementMode
}

void UGAAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (!Character)
	{
		Character = Cast<AGACharacter>(TryGetPawnOwner());
		return;
	}

	// Character is valid at this point

	// The direction the camera is facing
	ControlDirection = 
		Character->GetControlRotation();

	Speed = 
		Character->GetVelocity().Size2D();

	// Direction = UKismetAnimationLibrary::CalculateDirection(Character->GetVelocity(), Character->GetActorRotation());

	FRotator TempRotator = 
		FMath::RInterpTo(FRotator(AimPitch, AimYaw, 0) 
			, (ControlDirection - Character->GetActorRotation()).GetNormalized() 
			, DeltaTime 
			, 15.f);

	AimYaw = 
		FMath::ClampAngle(TempRotator.Yaw, -90, 90);
		
	AimPitch =
		FMath::ClampAngle(TempRotator.Pitch, -90, 90);

	bJumping =
		// CanJump(Character->bPressedJump); Wish this would work...
		Character->GetCharacterMovement()->IsFalling();

	bFalling = 
		Character->GetCharacterMovement()->IsFalling();

	bCrouching =
		Character->bIsCrouching;

	HeldWeaponType =
		Character->HeldWeaponType;

	USkeletalMeshComponent* CharacterMesh = Character->GetMesh();

	if (CharacterMesh)
	{
		MeshSpaceVelocity =
			UKismetMathLibrary::InverseTransformDirection(
				CharacterMesh->GetComponentTransform(),
				Character->GetVelocity());

		RightFootEffectorLocation =
			FVector(Character->IKOffsetRightFoot * -1, 0, 0);

		LeftFootEffectorLocation =
			FVector(Character->IKOffsetLeftFoot, 0, 0);

		BodyDirection =
			MeshSpaceVelocity.ForwardVector.Rotation();

		FRotator AngleA = 
			BodyDirection - ControlDirection;
		FRotator AngleB = 
			BodyDirection + ControlDirection;

		AngleA.Yaw = AngleA.Yaw + 90;
		AngleB.Yaw = AngleB.Yaw + 90;

		if (AngleA.Yaw > BodyDirection.Yaw + 270 || AngleA.Yaw < BodyDirection.Yaw + 90)
		{
			// GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Purple, AngleA.ToString());
			HeadRotation =
				FRotator(AngleA);
		}
		else
		{
			// This never gets run rn
			HeadRotation =
				FRotator(AngleB);
		}
	}

	// AGAActor* Item = Character->HeldItem;
	//
	// if(!Item)
	// { return; }
	//
	// The item is valid at this point
	//
	// USkeletalMeshComponent* ItemMesh = Item->Mesh;
	//
	// if(ItemMesh)
	// {
	//		FVector Location = ItemMesh->GetSocketLocation(FName("second_grip"));
	//		FVector OutLocation;
	//		FRotator OutRotation;
	//		CharacterMesh->TransformToBoneSpace(FName("hand_l"), Location, OutLocation, OutRotation);
	//		IKLeftHandTranform = FTransform(OutLocation, FRotator, FVector(1));
	// }
}

bool UGAAnimInstance::CanJump(bool bShouldJump)
{
	return bShouldJump && bJumping == false;
}

void UGAAnimInstance::AnimNotify_Jump()
{
	if (bJumping)
	{
		Character->Jump();
	}
}
/*
void UGAAnimInstance::AnimNotify_IdleStart()
{
	bJumping = false;

	Character->bPressedJump = false;
}
*/
void UGAAnimInstance::AnimNotify_JogStart()
{
	// bJumping = false;

	// Character->bPressedJump = false;
}

