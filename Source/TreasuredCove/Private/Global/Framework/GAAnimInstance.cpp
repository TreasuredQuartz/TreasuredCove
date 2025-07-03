// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Framework/GAAnimInstance.h"
#include "Global/Actors/GAActor.h"
#include "Global/Actors/GACharacter.h"
#include "Global/Components/Characters/GACharacterMovementComponent.h"
#include "Global/Components/Items/AimOffsetComponent.h"
#include "Global/Components/FirstPersonComponent.h"
#include "Global/Enumerations/CustomMovementMode.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "KismetAnimationLibrary.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"

void UGAAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	Character = Cast<AGACharacter>(TryGetPawnOwner());

	if (!Character) return;

	Character->MovementModeChangedDelegate.AddDynamic(this, &UGAAnimInstance::OnMovementModeChanged);
}

void UGAAnimInstance::OnMovementModeChanged(class ACharacter* inCharacter, EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	CharacterMovementMode = inCharacter->GetCharacterMovement()->MovementMode;
	CharacterCustomMovementMode = inCharacter->GetCharacterMovement()->CustomMovementMode;
}

void UGAAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (!Character)
	{
		Character = Cast<AGACharacter>(TryGetPawnOwner());
		return;
	}

	/*bIsPlayerControlled =
		Character == UGameplayStatics::GetPlayerPawn(Character, 0);*/

	/*if (!bIsPlayerControlled)
	{
		bPerformIKTraces = false;
		bFirstPerson = false;
	}*/

	// Character is valid at this point

	// Camera mode
	bFirstPerson = false;
	if (Character->IsLocallyControlled())
	{
		if (UFirstPersonComponent* FirstPersonComp = Character->GetComponentByClass<UFirstPersonComponent>())
		{
			bFirstPerson = FirstPersonComp->IsFirstPerson();
		}
	}

	// Multiply by weapon's Twist/Turn multiplier
	if (bFirstPerson)
	{
		DominantHandTwist = FMath::RInterpTo(DominantHandTwist
			, (ControlDirection - Character->GetControlRotation())
			, DeltaTime
			, 20.f);
	}

	// Debug twist rate
	// if (Character == UGameplayStatics::GetPlayerCharacter(Character, 0)) UE_LOG(LogTemp, Warning, TEXT("Previous Direction: %d \nCurrent Direction: %d\nResulting twist: %d"), *ControlDirection.ToString(), *Character->GetControlRotation().ToString(), *DominantHandTwist.ToString());

	// The direction the camera is facing
	ControlDirection =
		Character->GetControlRotation();

	// Character's velocity size
	Speed = 
		Character->GetVelocity().Size2D();

	// AnimInstance throws warning "Use KismetAnimationLibrary instead"
	// - UKismetAnimationLibrary throws static linker error.
	// -- This lambda is simply the copy-pasted of either of the above.
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

	// Movement Direction
	Direction = 
		CalcDirection(Character->GetVelocity(), Character->GetActorRotation());

	// 
	FRotator MeshDirection = FRotator();
	if (!bFirstPerson) MeshDirection = (ControlDirection - Character->GetActorRotation()).GetNormalized();

	FRotator TempRotator =
		FMath::RInterpTo(FRotator(-AimPitch, AimYaw, 0)
			, MeshDirection
			, DeltaTime
			, 15.f);

	AimYaw = 
		FMath::Clamp(TempRotator.Yaw, -90, 90);

	AimPitch =
		FMath::Clamp(TempRotator.Pitch, -90, 90); // */

	bCrouching =
		Character->bIsCrouched;

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

	CharacterItem =
		Character->GetHeldItem();

	if (!GetSkelMeshComponent())
	{
		return;
	}

	UpdateIKPlacements(DeltaTime);
	
	if (!CharacterItem)
	{ 
		return; 
	}
	
	// The item is valid at this point
	
	UAimOffsetComponent* ItemAimOffsetComponent = 
		CharacterItem->GetComponentByClass<UAimOffsetComponent>();
	
	if (ItemAimOffsetComponent)
	{
		AimOffsetAlpha = 1 - ItemAimOffsetComponent->GetAlpha();
		AimOffsetAlphaLerp = FMath::Lerp(0.3, 1.f, AimOffsetAlpha);
		AimOffset = FMath::Lerp(ItemAimOffsetComponent->GetAimOffset(), ItemAimOffsetComponent->GetNeutralOffset(), AimOffsetAlpha);
	}
}

bool UGAAnimInstance::FloorLineTrace(FHitResult& Hit, const FVector& SocketLocation, float Distance) const
{
	const FVector& Start = FVector(SocketLocation.X, SocketLocation.Y, Character->GetActorLocation().Z);
	const FVector& End = FVector(SocketLocation.X, SocketLocation.Y, SocketLocation.Z - Distance);
	const FCollisionQueryParams Params(FName(), TStatId(), false, Character);
	// Params.AddIgnoredActor(Character);

	// DrawDebugLine(GetWorld(), Start, End, FColor::Red);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility, Params))
	{
		// DrawDebugPoint(GetWorld(), Hit.Location, 10, FColor::Green);
	}
	else
	{
		// DrawDebugPoint(GetWorld(), Hit.Location, 10, FColor::Red);
	}

	return Hit.bBlockingHit;
};

bool UGAAnimInstance::WallLineTrace(FHitResult& Hit, const FVector& SocketLocation, float Distance) const
{
	const FVector& Start = FVector(SocketLocation.X, SocketLocation.Y, SocketLocation.Z);
	const FVector& End = FVector(SocketLocation + (Character->GetActorForwardVector() * Distance));
	const FCollisionQueryParams Params(FName(), TStatId(), false, Character);
	// Params.AddIgnoredActor(Character);

	// DrawDebugLine(GetWorld(), Start, End, FColor::Red);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility, Params))
	{
		// DrawDebugPoint(GetWorld(), Hit.Location, 10, FColor::Green);
	}
	else
	{
		// DrawDebugPoint(GetWorld(), Hit.Location, 10, FColor::Red);
	}

	return Hit.bBlockingHit;
};

void UGAAnimInstance::UpdateIKPlacements(float DeltaTime)
{
	if (!bPerformIKTraces)
	{
		ResetIK();
		return;
	}

	if (GetWorld())
	{
		switch (CharacterMovementMode)
		{
		case MOVE_NavWalking:
		case MOVE_Walking:
			IKPlacements_Walking(DeltaTime);
			break;
		case MOVE_Falling:
		case MOVE_Swimming:
		case MOVE_Flying:
			break;
		case MOVE_Custom:
			switch(CharacterCustomMovementMode)
			{ 
			case MOVE_WallRunning:
				IKPlacements_WallRunning(DeltaTime);
				break;
			case MOVE_Climbing:
				IKPlacements_Climbing(DeltaTime);
				break;
			default:
				IKPlacements_Hanging(DeltaTime);
				break;
			}
		default:
			ResetIK();
			break;
		}

		if (bFirstPerson) IKPlacements_AimItem(DeltaTime);
	}
	else
	{
		ResetIK();
	}
}

void UGAAnimInstance::ResetIK()
{
	ResetBodyIK();
	ResetArmIK();
	ResetLegIK();
}

void UGAAnimInstance::ResetBodyIK()
{
	PelvisOffsetLocation = FVector::ZeroVector;
}

void UGAAnimInstance::ResetArmIK()
{
	HandRotation = ControlDirection;
	HandRotation.Yaw += 180;
	HandRotation.Pitch *= -1;
	LeftHandEffectorTransform = FTransform::Identity;
	RightHandEffectorTransform = FTransform::Identity;
}

void UGAAnimInstance::ResetLegIK()
{
	RightFootEffectorLocation = FVector::ZeroVector;
	LeftFootEffectorLocation = FVector::ZeroVector;
}

void UGAAnimInstance::IKPlacements_Walking(float DeltaTime)
{
	if (bFirstPerson)
	{
		return;
	}

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

	if (FloorLineTrace(Hit_r, RightLocation, halfHeight))
	{
		// UE_LOG(LogTemp, Warning, TEXT("Right foot trace hit! %d"), *(RightLocation.ToString()));
	}

	if (FloorLineTrace(Hit_l, LeftLocation, halfHeight))
	{
		// UE_LOG(LogTemp, Warning, TEXT("Left foot trace hit! %d"), *(LeftLocation.ToString()));
	}

	RightHitLocation = Hit_r.bBlockingHit ? FVector(0, 0, (Hit_r.Location - MeshLocation).Z) : FVector(0, 0, RightLocation.Z - halfHeight);
	LeftHitLocation = Hit_l.bBlockingHit ? FVector(0, 0, (Hit_l.Location - MeshLocation).Z) : FVector(0, 0, LeftLocation.Z - halfHeight);

	float LeftRightDifference = FMath::Abs(RightHitLocation.Z - LeftHitLocation.Z);
	pelvisOffset =
		FMath::FInterpTo(PelvisOffsetLocation.Z
			, (LeftRightDifference < 50 ? LeftRightDifference * -0.5 : 0)
			, DeltaTime
			, 10.f);
	PelvisOffsetLocation.Z = pelvisOffset;

	rightOffset =
		FMath::FInterpTo(RightFootEffectorLocation.X
			, -((Hit_r.Location.Z - MeshLocation.Z) - PelvisOffsetLocation.Z)
			, DeltaTime
			, 10.f);

	leftOffset =
		FMath::FInterpTo(LeftFootEffectorLocation.X
			, (Hit_l.Location.Z - MeshLocation.Z) - PelvisOffsetLocation.Z
			, DeltaTime
			, 10.f);

	RightFootEffectorLocation.X = rightOffset;
	LeftFootEffectorLocation.X = leftOffset;
}

void UGAAnimInstance::IKPlacements_WallRunning(float DeltaTime)
{

}

void UGAAnimInstance::IKPlacements_Climbing(float DeltaTime)
{
	if (bFirstPerson) return;

	FHitResult Hit_Hand_r;
	FHitResult Hit_Hand_l;
	FHitResult Hit_Foot_r;
	FHitResult Hit_Foot_l;

	const FVector& MeshLocation = GetSkelMeshComponent()->GetComponentLocation();

	const FVector& LocationHandRight = GetSkelMeshComponent()->GetSocketLocation(FName("hand_r"));
	const FVector& LocationHandLeft = GetSkelMeshComponent()->GetSocketLocation(FName("hand_l"));
	const FVector& LocationFootRight = GetSkelMeshComponent()->GetSocketLocation(FName("foot_r"));
	const FVector& LocationFootLeft = GetSkelMeshComponent()->GetSocketLocation(FName("foot_l"));

	FVector LocationHitHandRight = FVector::ZeroVector;
	FVector LocationHitHandLeft = FVector::ZeroVector;
	FVector LocationHitFootRight = FVector::ZeroVector;
	FVector LocationHitFootLeft = FVector::ZeroVector;

	float CapsuleRadius;
	float CapsuleHalfHeight;
	Character->GetCapsuleComponent()->GetScaledCapsuleSize(CapsuleRadius, CapsuleHalfHeight);

	float HandRightOffset = 0.f;
	float HandLeftOffset = 0.f;
	float FootRightOffset = 0.f;
	float FootLeftOffset = 0.f;
	float pelvisOffset = 0.f;

	if (WallLineTrace(Hit_Foot_r, LocationHandRight, CapsuleRadius))
	{
		// UE_LOG(LogTemp, Warning, TEXT("Right foot trace hit! %d"), *(RightLocation.ToString()));
	}
	if (WallLineTrace(Hit_Foot_l, LocationHandLeft, CapsuleRadius))
	{
		// UE_LOG(LogTemp, Warning, TEXT("Left foot trace hit! %d"), *(LeftLocation.ToString()));
	}
	if (WallLineTrace(Hit_Foot_r, LocationFootRight, CapsuleRadius))
	{
		// UE_LOG(LogTemp, Warning, TEXT("Right foot trace hit! %d"), *(RightLocation.ToString()));
	}
	if (WallLineTrace(Hit_Foot_l, LocationFootLeft, CapsuleRadius))
	{
		// UE_LOG(LogTemp, Warning, TEXT("Left foot trace hit! %d"), *(LeftLocation.ToString()));
	}

	LocationHitHandRight = Hit_Hand_r.bBlockingHit ? FVector(0, 0, (Hit_Hand_r.Location - MeshLocation).Z) : LocationHitHandRight;
	LocationHitHandLeft = Hit_Hand_l.bBlockingHit ? FVector(0, 0, (Hit_Hand_l.Location - MeshLocation).Z) : LocationHitHandLeft;
	LocationHitFootRight = Hit_Foot_r.bBlockingHit ? FVector(0, 0, (Hit_Foot_r.Location - MeshLocation).Z) : LocationHitFootRight;
	LocationHitFootLeft = Hit_Foot_l.bBlockingHit ? FVector(0, 0, (Hit_Foot_l.Location - MeshLocation).Z) : LocationHitFootLeft;

	float LeftRightDifference = FMath::Abs(LocationHitFootRight.Z - LocationHitFootLeft.Z);
	pelvisOffset =
		FMath::FInterpTo(PelvisOffsetLocation.Z
			, (LeftRightDifference < 50 ? LeftRightDifference * -0.5 : 0)
			, DeltaTime
			, 10.f);
	PelvisOffsetLocation.Z = pelvisOffset;

	FootRightOffset = 0.f; /*
		FMath::VInterpTo(RightFootEffectorLocation
			, -((LocationHitFootRight - MeshLocation))
			, DeltaTime
			, 10.f); */

	FootLeftOffset = 0.f; /*
		FMath::VInterpTo(LeftFootEffectorLocation
			, (LocationHitFootLeft - MeshLocation)
			, DeltaTime
			, 10.f); */

	RightFootEffectorLocation.X = FootRightOffset;
	LeftFootEffectorLocation.X = FootLeftOffset;
}

void UGAAnimInstance::IKPlacements_Hanging(float DeltaTime)
{

}

void UGAAnimInstance::IKPlacements_AimItem(float DeltaTime)
{
	if (!CharacterItem || !bFirstPerson)
	{
		ResetArmIK();
		RightHandEffectorTransform.SetLocation(AimOffset + ControlDirection.Vector() * 20);
		RightHandEffectorTransform.SetRotation(FRotator(0, 0, -90).Quaternion());
		LeftHandEffectorTransform.SetLocation(AimOffset + ControlDirection.Vector() * 20);
		LeftHandEffectorTransform.SetRotation(FRotator(180, 0, 90).Quaternion());
		return;
	}

	const FVector& Grip_r = GetSkelMeshComponent()->GetSocketLocation(FName("hand_r"));
	const FVector& Grip_l = GetSkelMeshComponent()->GetSocketLocation(FName("hand_l"));

	FHitResult Hit;
	const FVector& Start = GetSkelMeshComponent()->GetSocketLocation(FName("sight"));
	const FVector& End = Start + (ControlDirection.Vector() * 10000);
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility);

	// DrawDebugLine(GetWorld(), Start, End, FColor::Blue);
	// DrawDebugPoint(GetWorld(), Hit.bBlockingHit ? Hit.Location : End, 20.f, Hit.bBlockingHit ? FColor::Green : FColor::Red);

	FRotator LookAtRotation_r = (UKismetMathLibrary::FindLookAtRotation(Grip_r, Hit.bBlockingHit ? Hit.Location : End));
	const FRotator& LookAtRotation_l = (UKismetMathLibrary::FindLookAtRotation(Grip_l, Hit.bBlockingHit ? Hit.Location : End));

	// Rotation issues.
	// -- Pitch is inverted+
	// -- Yaw is turned around
	// I think they stem from DominantHandTwist (Difference between last ControlDirection and current ControlDirection)
	// -- This value is in world space and overrides the base rotation of the hand.
	// LookAtRotation_r.Pitch *= -1;
	// LookAtRotation_r.Yaw += 180;
	// LookAtRotation_r += DominantHandTwist;

	// Use Interp to reduce snapping between far and close objects
	HandRotation = FMath::RInterpTo(HandRotation
		, LookAtRotation_r
		, DeltaTime
		, 20.f);

	const FVector ArmOffset = ControlDirection.RotateVector(AimOffset); // + Character->GetActorLocation() + FVector(0, 0, 65) + (Character->GetActorForwardVector() * 20);
	{
		FRotator OutRotation;
		FVector OutLocation;

		FName hl = FName("hand_l");
		const FVector HandOffset = ArmOffset + GetSkelMeshComponent()->GetSocketLocation(hl);
		const FRotator HandRotationOffset = FRotator(0, 0, 0); // GetSkelMeshComponent()->GetSocketRotation(hl);

		GetSkelMeshComponent()->TransformToBoneSpace(hl, HandOffset, HandRotationOffset, OutLocation, OutRotation);
		LeftHandEffectorTransform.SetLocation(OutLocation);
		// OutRotation.Pitch *= -1;
		// OutRotation.Yaw += 180;
		LeftHandEffectorTransform.SetRotation(OutRotation.Quaternion());
	}

	{
		FRotator OutRotation;
		FVector OutLocation;

		FName hr = FName("hand_r");
		const FVector HandOffset = ArmOffset + GetSkelMeshComponent()->GetSocketLocation(hr);
		const FRotator HandRotationOffset = FRotator(0, 0, 0); // GetSkelMeshComponent()->GetSocketRotation(hr);

		GetSkelMeshComponent()->TransformToBoneSpace(hr, HandOffset, HandRotationOffset, OutLocation, OutRotation);
		RightHandEffectorTransform.SetLocation(OutLocation);
		// OutRotation.Pitch *= -1;
		// OutRotation.Yaw += 180;
		RightHandEffectorTransform.SetRotation(OutRotation.Quaternion());
	}
}

bool UGAAnimInstance::CanJump(bool bShouldJump)
{
	return bShouldJump && CharacterMovementMode != MOVE_Falling;
}

void UGAAnimInstance::AnimNotify_Jump()
{
	if (CharacterMovementMode == MOVE_Falling)
	{
		Character->Jump();
	}
}

/* void UGAAnimInstance::AnimNotify_IdleStart()
{
	bJumping = false;

	Character->bPressedJump = false;
} // */

void UGAAnimInstance::AnimNotify_JogStart()
{
	// bJumping = false;

	// Character->bPressedJump = false;
}

