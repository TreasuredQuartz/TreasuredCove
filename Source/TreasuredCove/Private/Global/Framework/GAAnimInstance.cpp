// Fill out your copyright notice in the Description page of Project Settings.


#include "GAAnimInstance.h"
#include "GAActor.h"
#include "GACharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"

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

	// Camera mode
	bFirstPerson =
		Character->bFirstPerson;

	// Multiply by weapon's Twist/Turn multiplier
	/*DominantHandTwist =
		(ControlDirection - Character->GetControlRotation()).Yaw * 10.f;*/

	// The direction the camera is facing
	ControlDirection =
		Character->GetControlRotation();

	// Character's velocity size
	Speed = 
		Character->GetVelocity().Size2D();

	// AnimInstance throws warning "Use KismetAnimationLibrary instead"
	// - UKismetAnimationLibrary throws static linker error.
	// -- This lambda is simply the copy-pasted of the above.
	auto CalcDirection = [&](const FVector& Velocity, const FRotator& BaseRotation)
	{
		if (!Velocity.IsNearlyZero())
		{
			FMatrix RotMatrix = FRotationMatrix(BaseRotation);
			FVector ForwardVector = RotMatrix.GetScaledAxis(EAxis::X);
			FVector RightVector = RotMatrix.GetScaledAxis(EAxis::Y);
			FVector NormalizedVel = Velocity.GetSafeNormal2D();

			// get a cos(alpha) of forward vector vs velocity
			float ForwardCosAngle = FVector::DotProduct(ForwardVector, NormalizedVel);
			// now get the alpha and convert to degree
			float ForwardDeltaDegree = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));

			// depending on where right vector is, flip it
			float RightCosAngle = FVector::DotProduct(RightVector, NormalizedVel);
			if (RightCosAngle > 0)
			{
				ForwardDeltaDegree *= -1;
			}

			return ForwardDeltaDegree;
		}

		return 0.f;
	};

	Direction = 
		CalcDirection(Character->GetVelocity(), Character->GetActorRotation());

	FRotator MeshDirection = 
		(ControlDirection - Character->GetActorRotation()).GetNormalized();

	/*FRotator TempRotator =
		FMath::RInterpTo(FRotator(-AimPitch, AimYaw, 0)
			, MeshDirection
			, DeltaTime
			, 15.f);

	AimYaw = 
		FMath::Clamp(TempRotator.Yaw, -90, 90);

	AimPitch =
		FMath::Clamp(TempRotator.Pitch, -90, 90);*/

	bJumping =
		// CanJump(Character->bPressedJump); Wish this would work...
		Character->GetCharacterMovement()->IsFalling();

	bFalling = 
		Character->GetCharacterMovement()->IsFalling();

	bCrouching =
		Character->bIsCrouching;

	HeldWeaponType =
		Character->HeldWeaponType;

	MeshSpaceVelocity =
		UKismetMathLibrary::InverseTransformDirection(
			GetSkelMeshComponent()->GetComponentTransform(),
			Character->GetVelocity());

	BodyDirection =
		MeshSpaceVelocity.ForwardVector.Rotation();

	HeadRotation =
		FRotator(AimYaw, AimPitch, 0);

	if (bFirstPerson)
	{
		Character->bUseControllerRotationYaw = true;
	}
	else if (Speed > 0 && Character->GetVelocity().ForwardVector.X >= 0)
	{
		Character->bUseControllerRotationYaw = true;
	}
	else
	{
		Character->bUseControllerRotationYaw = false;
	}

	if (GetWorld())
	{
		bPerformLegIKTraces = true;
		if ((!bJumping) && (!bFalling) && bPerformLegIKTraces)
		{
			auto LineTrace = [&](FHitResult& Hit, const FVector& SocketLocation, float Distance)
			{
				const FVector& End = FVector(SocketLocation.X, SocketLocation.Y, SocketLocation.Z - Distance);
				const FVector& Start = FVector(SocketLocation.X, SocketLocation.Y, Character->GetActorLocation().Z);

				// DrawDebugLine(GetWorld(), Start, End, FColor::Red);
				if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility))
				{
					// DrawDebugPoint(GetWorld(), Hit.Location, 10, FColor::Green);
				}
				else
				{
					// DrawDebugPoint(GetWorld(), Hit.Location, 10, FColor::Red);
				} 

				return Hit.bBlockingHit;
			};

			FHitResult Hit_r;
			FHitResult Hit_l;

			const FVector& MeshLocation = GetSkelMeshComponent()->GetComponentLocation();

			const FVector& RightLocation = GetSkelMeshComponent()->GetSocketLocation(FName("foot_r"));
			const FVector& LeftLocation = GetSkelMeshComponent()->GetSocketLocation(FName("foot_l"));

			FVector RightHitLocation = FVector::ZeroVector;
			FVector LeftHitLocation = FVector::ZeroVector;

			float halfHeight = Character->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
			float rightOffset = 0;
			float leftOffset = 0;
			float pelvisOffset = 0;

			if (LineTrace(Hit_r, RightLocation, halfHeight))
			{
				// UE_LOG(LogTemp, Warning, TEXT("Right foot trace hit! %d"), *(RightLocation.ToString()));
			}

			if (LineTrace(Hit_l, LeftLocation, halfHeight)) 
			{
				// UE_LOG(LogTemp, Warning, TEXT("Left foot trace hit! %d"), *(LeftLocation.ToString()));
			}

			RightHitLocation = Hit_r.bBlockingHit ? FVector(0, 0, (Hit_r.Location - MeshLocation).Z) : FVector(0, 0, Hit_r.Location.Z);
			LeftHitLocation = Hit_l.bBlockingHit ? FVector(0, 0, (Hit_l.Location - MeshLocation).Z) : FVector(0, 0, Hit_l.Location.Z);

			rightOffset =
				FMath::FInterpTo(RightFootEffectorLocation.X
					, (Hit_r.Location.Z - MeshLocation.Z) - PelvisOffsetLocation.Z
					, DeltaTime
					, 15.f);

			leftOffset =
				FMath::FInterpTo(LeftFootEffectorLocation.X
					, (Hit_l.Location.Z - MeshLocation.Z) - PelvisOffsetLocation.Z
					, DeltaTime
					, 15.f);

			float LeftRightDifference = FMath::Abs(RightHitLocation.Z - LeftHitLocation.Z);
			pelvisOffset =
				FMath::FInterpTo(PelvisOffsetLocation.Z
					, (LeftRightDifference < 50 ? LeftRightDifference * -0.5 : 0)
					, DeltaTime
					, 15.f);

			RightFootEffectorLocation.X = -rightOffset;
			LeftFootEffectorLocation.X = leftOffset;
			PelvisOffsetLocation.Z = pelvisOffset;
		}
		else
		{
			RightFootEffectorLocation.X = 0;
			LeftFootEffectorLocation.X = 0;
			PelvisOffsetLocation.Z = 0;
		}

		if (true)
		{
			const FVector& Grip_r = GetSkelMeshComponent()->GetSocketLocation(FName("grip_r"));
			const FVector& Grip_l = GetSkelMeshComponent()->GetSocketLocation(FName("grip_l"));

			FHitResult Hit;
			const FVector& Start = GetSkelMeshComponent()->GetSocketLocation(FName("sight"));
			const FVector& End = Start + (ControlDirection.Vector() * 10000);
			GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility);

			DrawDebugLine(GetWorld(), Start, End, FColor::Blue);
			DrawDebugPoint(GetWorld(), Hit.bBlockingHit ? Hit.Location : End, 20.f, Hit.bBlockingHit ? FColor::Green : FColor::Red);

			const FRotator& LookAtRotation_r = (UKismetMathLibrary::FindLookAtRotation(Grip_r, Hit.bBlockingHit ? Hit.Location : End)).GetNormalized();
			const FRotator& LookAtRotation_l = (UKismetMathLibrary::FindLookAtRotation(Grip_l, Hit.bBlockingHit ? Hit.Location : End)).GetNormalized();

			FRotator OutRotation;
			FVector OutLocation;
			if (AGAActor* Item = Character->HeldItem)
			{
				GetSkelMeshComponent()->TransformToBoneSpace(FName("grip_l"), Item->GetSupportingHandOffset(), LookAtRotation_l, LeftHandEffectorLocation, OutRotation);
				GetSkelMeshComponent()->TransformToBoneSpace(FName("grip_r"), Item->GetHeldHandOffset(), LookAtRotation_r, RightHandEffectorLocation, HandRotation);

				if (bFirstPerson)
				{

				}
			}
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

